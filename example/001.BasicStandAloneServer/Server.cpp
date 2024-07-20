#include <Gamnet/Gamnet.h>
#include "Message.h"

class Session : public Gamnet::Network::Tcp::Session
{
public:
	Session() = default;
	virtual ~Session() = default;

	virtual void OnCreate() override
	{
		LOG(DEV, "session_key:", session_key);
	}
	virtual void OnAccept() override
	{
		LOG(DEV, "session_key:", session_key);

        MsgSvrCli_Welcome_Ntf ntf;
		ntf.greeting = "Welcome";
		Gamnet::Network::Tcp::SendMsg(std::static_pointer_cast<Session>(shared_from_this()), ntf);
	}
	virtual void OnClose(int reason) override
	{
		LOG(DEV, "session_key:", session_key, ", reason:", reason);
	}
	virtual void OnDestroy() override
	{
		LOG(DEV, "session_key:", session_key);
	}
};

class Handler_HelloWorld : public Gamnet::Network::IHandler {
public:
	Handler_HelloWorld() = default;
	virtual ~Handler_HelloWorld() = default;

	void Recv_CliSvr_Req(const std::shared_ptr<Session>& session, const MsgCliSvr_HelloWorld_Req& req)
	{
		MsgSvrCli_HelloWorld_Ans ans;
		try {
			LOG(DEV, "MsgCliSvr_HelloWorld_Req(session_key:", session->session_key, ", message:", req.greeting, ")");
			ans.answer = req.greeting;
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(Gamnet::Log::LOG_LEVEL_TYPE::LOG_LEVEL_ERR, e.what());
			ans.answer = e.what();
		}
		LOG(DEV, "MsgSvrCli_HelloWorld_Ans(session_key:", session->session_key, ", message:", ans.answer, ")");
		Gamnet::Network::Tcp::SendMsg(session, ans);
	}
};

GAMNET_BIND_TCP_HANDLER(
	Session,
	MsgCliSvr_HelloWorld_Req,
	Handler_HelloWorld, Recv_CliSvr_Req,
	HandlerStatic
);

class Handler_ServerState : public Gamnet::Network::IHandler {
public:
    Handler_ServerState() = default;
    virtual ~Handler_ServerState() = default;

    void Recv_Req(const std::shared_ptr<Gamnet::Network::Http::Session>& session, const Gamnet::Network::Http::Request& request)
    {
        Gamnet::Network::Http::Response res;
        Gamnet::Time::DateTime now = Gamnet::Time::Local::Now();

        bool order = true;
        try {
            if ("desc" == (std::string)request["order"])
            {
                order = false;
            }
        }
        catch (const Gamnet::Exception& /*e*/)
        {
        }

        int sortIndex = -1;
        try {
            std::map<std::string, int> columns = {
                { "msg_id",			0},
                { "name",			1},
                {"begin_count",		2},
                {"finish_count",	3},
                {"total_time",		4},
                {"max_time",		5},
                {"total_time",		6},
            };
            auto itr = columns.find((std::string)request["sort"]);
            if (columns.end() != itr)
            {
                sortIndex = itr->second;
            }
        }
        catch (const Gamnet::Exception& /*e*/)
        {
            sortIndex = -1;
        }

        Json::Value root = Gamnet::Network::Tcp::ServerState<Session>();
        const Json::Value& messages = root["messages"];
        res.context += "<html>\n";
        res.context += "<head>\n";
        res.context += "<script type='text/javascript' src='https://www.gstatic.com/charts/loader.js'></script>\n";
        res.context += "<script type='text/javascript'>\n";
        res.context += "google.charts.load('current', {'packages':['table']});\n";


        res.context += "google.charts.setOnLoadCallback(drawInfoTable);\n";
        res.context += "function drawInfoTable() {\n";
        res.context += "	var data = new google.visualization.DataTable();\n";
        res.context += "	data.addColumn('string', 'key');\n";
        res.context += "	data.addColumn('string', 'value');\n";
        res.context += "	data.addRows([\n";
        res.context += Gamnet::Format("['boost_version', '", BOOST_LIB_VERSION, "'],\n");
        res.context += Gamnet::Format("['build_date', '", __DATE__, " ", __TIME__, "'],\n");
        res.context += Gamnet::Format("['date_time', '", now.ToString(), "'],\n");
        const Json::Value& acceptor = root["acceptor"];
        res.context += Gamnet::Format("['acceptor.port', '", acceptor["port"].asInt(), "'],\n");
        res.context += Gamnet::Format("['acceptor.accept_count', '", acceptor["accept_count"].asInt(), "'],\n");
        res.context += Gamnet::Format("['acceptor.close_count', '", acceptor["close_count"].asInt(), "'],\n");
        res.context += Gamnet::Format("['acceptor.fail_count', '", acceptor["fail_count"].asInt(), "'],\n");
        res.context += Gamnet::Format("['acceptor.max_socket_count', '", acceptor["max_socket_count"].asInt(), "'],\n");
        res.context += Gamnet::Format("['acceptor.active_socket_count', '", acceptor["active_socket_count"].asInt(), "'],\n");
        res.context += Gamnet::Format("['acceptor.idle_socket_count', '", acceptor["idle_socket_count"].asInt(), "'],\n");
        const Json::Value& sessions = root["session"];
        res.context += Gamnet::Format("['session.active_count', '", sessions["active_count"].asInt(), "'],\n");
        res.context += Gamnet::Format("['session.idle_count', '", sessions["idle_count"].asInt(), "'],\n");
        res.context += Gamnet::Format("['session.max_count', '", sessions["max_count"].asInt(), "'],\n");
        res.context += "	]);\n";
        res.context += "	var table = new google.visualization.Table(document.getElementById('table_div_1'));\n";
        res.context += "	table.draw(data, { showRowNumber:false, width : '500px' });\n";
        res.context += "}\n";

        res.context += "google.charts.setOnLoadCallback(drawMessageTable);\n";
        res.context += "function drawMessageTable() {\n";
        res.context += "	var data = new google.visualization.DataTable();\n";
        res.context += "	data.addColumn('number', 'msg_id');\n";
        res.context += "	data.addColumn('string', 'name');\n";
        res.context += "	data.addColumn('number', 'begin_count');\n";
        res.context += "	data.addColumn('number', 'finsh_count');\n";
        res.context += "	data.addColumn('number', 'average_time');\n";
        res.context += "	data.addColumn('number', 'max_time');\n";
        res.context += "	data.addColumn('number', 'total_time');\n";
        res.context += "	data.addRows([\n";
        std::string rows = "";
        for (const auto& message : messages)
        {
            rows += Gamnet::Format(
                "[", message["msg_id"].asUInt(), ","
                , "'", message["name"].asString(), "',"
                , message["begin_count"].asInt64(), ","
                , message["finish_count"].asInt64(), ","
                , 0 == message["finish_count"].asInt64() ? 0 : message["total_time"].asInt64() / message["finish_count"].asInt64(), ","
                , message["max_time"].asInt64(), ","
                , message["total_time"].asInt64(), "],"
            );
        }
        res.context += rows;
        res.context += "\n	]);\n";
        res.context += "var table = new google.visualization.Table(document.getElementById('table_div_2'));\n";
        res.context += Gamnet::Format("table.draw(data, { showRowNumber:true, width:'100%', sortColumn:", sortIndex, ", sortAscending:", order, "});\n");
        res.context += "}";
        res.context += "</script>\n";
        res.context += "</head>\n";
        res.context += "<body>\n";
        res.context += "<div id='table_div_1'></div>\n";
        res.context += "<br/>\n";
        res.context += "<div id='table_div_2'></div>\n";
        res.context += "</body>\n";
        res.context += "</html>\n";
        res.error_code = 200;
        Gamnet::Network::Http::SendMsg(session, res);
    }
};

GAMNET_BIND_HTTP_HANDLER(
    "server_state", Handler_ServerState, Recv_Req
);

class TestSession : public Gamnet::Test::Session {
public:
	virtual void OnCreate() override
	{
	}
	virtual void OnConnect() override
	{
	}
	virtual void OnClose(int reason) override
	{
	}
	virtual void OnDestroy() override
	{
	}
};

void Test_CliSvr_HelloWorld_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_HelloWorld_Req req;
	req.greeting = "World Hello";
	Gamnet::Test::SendMsg(session, req);
}

void Test_SvrCli_HelloWorld_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_HelloWorld_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(-1, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::LOG_LEVEL_TYPE::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_HelloWorld",
	MsgCliSvr_HelloWorld_Req, Test_CliSvr_HelloWorld_Req,
	MsgSvrCli_HelloWorld_Ans, Test_SvrCli_HelloWorld_Ans
);

void Test_SvrCli_Welcome_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Welcome_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(-1, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::LOG_LEVEL_TYPE::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	MsgSvrCli_Welcome_Ntf, Test_SvrCli_Welcome_Ntf
);

int main(int argc, char** argv)
{
	const std::string config = "config.xml";

	Gamnet::InitCrashDump();
	try {
		Gamnet::Log::ReadXml(config);
		LOG(INF, "file:", argv[0]);
		LOG(INF, "build date:", __DATE__, " ", __TIME__);
		LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

		Gamnet::Network::Tcp::ReadXml<Session>(config);
        Gamnet::Network::Http::ReadXml(config);
		Gamnet::Test::ReadXml<TestSession>(config);
		Gamnet::Run(std::thread::hardware_concurrency());
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(ERR, e.what(), "(error_code:", e.error_code(), ")");
		return -1;
	}

	return 0;
}
