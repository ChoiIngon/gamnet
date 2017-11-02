#include "Connection.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database {	namespace Redis {

	static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

	Connection::Connection() : socket_(io_service_), deadline_(io_service_)
	{
	}

	Connection::~Connection() 
	{
	}

	bool Connection::Connect(const ConnectionInfo& connInfo)
	{
		deadline_.expires_at(boost::posix_time::pos_infin);
		deadline_.async_wait([this](boost::system::error_code) {
			if (this->deadline_.expires_at() <= boost::asio::deadline_timer::traits_type::now())
			{
				// The deadline has passed. The socket is closed so that any outstanding
				// asynchronous operations are cancelled. This allows the blocked
				// connect(), read_line() or write_line() functions to return.
				boost::system::error_code ignored_ec;
				this->socket_.close(ignored_ec);

				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
				this->deadline_.expires_at(boost::posix_time::pos_infin);
			}
		}); 
		boost::asio::ip::tcp::resolver resolver_(io_service_);
		boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({ connInfo.host, Format(connInfo.port).c_str() }));

		LOG(INF, "[Redis] connect...(host:", connInfo.host, ", port:", connInfo.port, ")");

		boost::system::error_code ec;
		socket_.connect(endpoint_, ec);
		if (0 != ec)
		{
			LOG(GAMNET_ERR, "[Redis] connect fail(host:", connInfo.host, ", port:", connInfo.port, ")");
			return false;
		}
		return true;
	}

	std::shared_ptr<ResultSetImpl> Connection::Execute(const std::string& query)
	{	
		if (false == socket_.is_open())
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::ConnectFailError));
		}
	
		socket_.write_some(boost::asio::buffer(query + "\r\n", query.length()+2));
		
		std::list<std::string> tokens;
		std::string token = "";
		while (true)
		{
			boost::asio::streambuf streambuf;
			boost::asio::read_until(socket_, streambuf, "\r\n");
			size_t size = streambuf.size();
			const char* data = boost::asio::buffer_cast<const char*>(streambuf.data());
			std::istream response_stream(&streambuf);
			for (auto itr = std::istream_iterator<std::string>(response_stream); itr != std::istream_iterator<std::string>(); itr++)
			{
				switch ((*itr)[0])
				{
				case '+':
				case '-':
				case ':':
				case '$':
				case '*':
					if ("" != token)
					{
						tokens.push_back(token);
						token = "";
					}
					break;
				}

				token += *itr;
				tokens.push_back(token);
				token = "";
			}
				
			if (true == tokens.empty())
			{
				break;
			}

			if ('\n' == data[size - 1])
			{
				break;
			}

			token = tokens.back();
			tokens.pop_back();
		}

		std::shared_ptr<ResultSetImpl> impl(new ResultSetImpl());
		for (auto itr = tokens.begin(); itr != tokens.end(); itr++)
		{
			Parse(impl, itr);
		}
			
		return impl;
	}

	void Connection::Parse(const std::shared_ptr<ResultSetImpl>& impl, std::list<std::string>::iterator& itr_token)
	{
		const std::string& token = *itr_token;
		switch (token[0])
		{
		case '+':
			impl->push_back(token.substr(1));
			break;
		case '-':
			impl->error = token;
			break;
		case ':':
			impl->push_back(token.substr(1));
			break;
		case '$':
			if ("$-1" == token) // empty
			{
				break;
			}
			impl->push_back(*(++itr_token));
			break;
		case '*': 
			{
				int count = atoi(token.substr(1).c_str());
				for (int i = 0; i < count; i++)
				{
					Parse(impl, ++itr_token);
				}
			}
			break;
		default:
			impl->error += " " + token;
			break;
		}
	}
}}}
