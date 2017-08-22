#include "Response.h"

namespace Gamnet { namespace Network { namespace Http {

std::map<int, std::string> mapErrorCode;

static bool InitErrorCode()
{
	mapErrorCode[100] = "Continue";
	mapErrorCode[101] = "Switching Protocols";
	mapErrorCode[200] = "OK";
	mapErrorCode[201] = "Created";
	mapErrorCode[202] = "Accepted";
	mapErrorCode[203] = "Non-Authoritative Information (since HTTP/1.1)";
	mapErrorCode[204] = "No Content";
	mapErrorCode[205] = "Reset Content";
	mapErrorCode[401] = "Unauthorized";
	mapErrorCode[402] = "Payment Required";
	mapErrorCode[403] = "Forbidden";
	mapErrorCode[404] = "Not Found";
	mapErrorCode[405] = "Method Not Allowed";
	return true;
}

bool bool_InitErrorCode_ = InitErrorCode();

Response::Response() : nErrorCode(0) {
}

Response::~Response() {
}

const char* GetErrorStr(int error_code)
{
	return mapErrorCode[error_code].c_str();
}

}}}
