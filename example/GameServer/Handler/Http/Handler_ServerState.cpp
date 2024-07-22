#include "Handler_ServerState.h"
#include "../../UserSession.h"

namespace Handler { namespace Http {

Handler_ServerState::Handler_ServerState()
{
}

Handler_ServerState::~Handler_ServerState() 
{
}

void Handler_ServerState::Recv_ServerState_Req(const std::shared_ptr<Gamnet::Network::Http::Session>& session, const Gamnet::Network::Http::Request& request)
{
	Gamnet::Network::Http::Response res;

	time_t logtime_;
	struct tm when;
	time(&logtime_);

	char date_time[22] = { 0 };
#ifdef _WIN32
	localtime_s(&when, &logtime_);
	_snprintf_s(date_time, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#else
	localtime_r(&logtime_, &when);
	snprintf(date_time, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#endif
	
	bool order = true;
	try {
		if("desc" == (std::string)request["order"])
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
		if(columns.end() != itr)
		{
			sortIndex = itr->second;
		}
	}
	catch(const Gamnet::Exception& /*e*/) 
	{
		sortIndex = -1;
	}
	
	Json::Value root = Gamnet::Network::Tcp::ServerState<UserSession>();
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
	res.context += Gamnet::Format("['date_time', '", date_time, "'],\n");
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
	res.context += Gamnet::Format("table.draw(data, { showRowNumber:true, width:'100%', sortColumn:", sortIndex, ", sortAscending:", order,"});\n");
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

//http://localhost:8080/server_state?sort=max_time&order=desc
GAMNET_BIND_HTTP_HANDLER(
	"server_state", Handler_ServerState, Recv_ServerState_Req
);

}}