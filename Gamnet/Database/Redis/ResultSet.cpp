#include "ResultSet.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database {	namespace Redis {
	ResultSetImpl::ResultSetImpl()
		: conn_(nullptr), read_index(0)
	{
	}

	ResultSetImpl::ResultSetImpl(const std::shared_ptr<Connection> conn) 
		: conn_(conn), read_index(0)
	{
	}

	bool ResultSetImpl::ParseString(Json::Value& output, const std::string& input)
	{
		std::size_t length = input.find("\r\n", read_index);
		if (std::string::npos == length)
		{
			LOG(ERR, "can not find terminate string");
			return false;
		}
		length -= read_index;
		output = input.substr(read_index, length);
		read_index += length + 2;
		return true;
	}

	bool ResultSetImpl::ParseInt(int& output, const std::string& input)
	{
		size_t length = input.find("\r\n", read_index);
		if (std::string::npos == length)
		{
			LOG(ERR, "can not find terminate string");
			return false;
		}
		length -= read_index;
		output = boost::lexical_cast<int>(input.substr(read_index, length));
		read_index += length + 2;
		return true;
	}

	bool ResultSetImpl::ParseBulkString(Json::Value& output, const std::string& input)
	{
		int length = 0;
		if (false == ParseInt(length, input))
		{
			return false;
		}

		if (0 > length)
		{
			output = Json::nullValue;
			return true;
		}

		if (input.length() - read_index < (size_t)length + 2)
		{
			LOG(ERR, "can not find terminate string");
			return false;
		}
		output = input.substr(read_index, length);
		read_index += length + 2;
		return true;
	}
	bool ResultSetImpl::ParseArray(Json::Value& output, const std::string& input)
	{
		int count = 0;
		if (false == ParseInt(count, input))
		{
			return false;
		}

		if(0 > count)
		{
			output = Json::nullValue;
			return true;
		}

		for (int i = 0; i<count; i++)
		{
			Json::Value element;
			if (false == Parse(element, input))
			{
				return false;
			}
			output[i] = element;
		}

		return true;
	}

	bool ResultSetImpl::Parse(Json::Value& result, const std::string& input)
	{
		char dataType = input[read_index];
		switch (dataType)
		{
		case '+': // simple string
		{
			read_index++;
			Json::Value output;
			if (false == ParseString(output, input))
			{
				return false;
			}
			result = output;
			break;
		}
		case '-': // error
		{
			read_index++;
			Json::Value output;
			if (false == ParseString(output, input))
			{
				return false;
			}
			error = output.asString();
			break;
		}
		case ':': // integer
		{
			read_index++;
			int output = 0;
			if (false == ParseInt(output, input))
			{
				return false;
			}
			result = output;
			break;
		}
		case '$': // bulk string
		{
			read_index++;
			Json::Value output;
			if (false == ParseBulkString(output, input))
			{
				return false;
			}
			result = output;
			break;
		}
		case '*':  // array
		{
			read_index++;
			//Json::Value output;
			if (false == ParseArray(result, input))
			{
				return false;
			}
			//*(Json::Value*)(this) = output;
			break;
		}
		default:
			LOG(WRN, "undefined parse identifier (input_size:", input.size(), ", read_index:", read_index, ", identifier:", dataType, ")");
			return false;
		}

		return true;
	}

	bool ResultSetImpl::Parse(const std::string& input)
	{
		*(Json::Value*)this = Json::nullValue;
		read_index = 0;
		return Parse(*(Json::Value*)this, input);
	}

	ResultSet::ResultSet() : impl_(nullptr)
	{
	}

	ResultSet::iterator ResultSet::begin() {
		if (nullptr == impl_)
		{
			return iterator();
		}

		return impl_->begin();
	}

	ResultSet::iterator ResultSet::end() const {
		if (nullptr == impl_)
		{
			return iterator();
		}

		return impl_->end();
	}

	const Json::Value& ResultSet::operator[] (unsigned int index)
	{
		if (index >= impl_->size()) // no neet to compare with 0. size() returns unsigned int
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidArrayRangeError, "invalid index(index:", index, ")");
		}
		return (*impl_)[index];
	}

	//const std::string& ResultSet::error() const
	//{
	//	if (nullptr == impl_)
	//	{
	//		return "empty";
	//	}

	//	return impl_->error;
	//}

	Json::ArrayIndex ResultSet::size() const
	{
		if (nullptr == impl_)
		{
			return 0;
		}

		return impl_->size();
	}

	bool ResultSet::empty() const
	{
		if (nullptr == impl_)
		{
			return true;
		}

		return impl_->empty();
	}

	Json::Value ResultSet::value()
	{
		if (nullptr == impl_)
		{
			return Json::Value();
		}
		
		return *impl_;
	}

} } }
