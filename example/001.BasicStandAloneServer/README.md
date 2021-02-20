# BasicStandAloneServer
BasicStandAloneServer 예제에서는 클라이언트로 부터 "Hello World" 텍스트를 전송 받아 그대로 돌려주는 간단한 에코 서버 예제를 통해 gamnet 프레임워크의 기본 사용법을 알아 보도록한다.

## Visual Studio 프로젝트 셋팅
1. 비주얼스튜디오에서 빈 C++ 프로젝트를 생성한다.
1. 구성 속성 > C/C++ > 일반 > 추가 포함 디렉터리 메뉴에서 아래에 나열된 include 디렉토리들을 추가한다.
	* ../../ (gamnet 헤더 디렉토리, 예제 프로젝트와 Gamnet 프로젝트의 상대 경로)
	* C:\local\boost_1_71_0 (boost 헤더 디렉토리)
	* C:\local\curl-7.43.0-win64\include (curl 헤더 디렉토리)
	* C:\OpenSSL-Win64\include (open ssl 헤더 디렉토리)
	* C:\Program Files\MySQL\MySQL Connector C 6.1\include (MySQL Connector 헤더 디렉토리)
1. 구성 속성 > C/C++ > 미리 컴파일 된 헤더 : '사용 안함'으로 설정
1. 구성 속성 > C/C++ > 출력 파일 > 개체 파일 이름 : $(IntDir)/%(RelativeDir) 로 설정하여 동일한 파일 이름을 가지더라도 디렉토리에 따라 다른 오브젝트 파일을 생성하도록 설정
1. 구성 속성 > 링커 > 일반 > 추가 라이브러리 디렉터리 메뉴에서 아래에 나열된 라이브러리 디렉토리들을 추가한다.
	* ../../x64/Debug (gamnet.lib 디렉토리, 예제 프로젝트와 Gamnet 프로젝트의 상대 경로)
	* C:\local\boost_1_71_0\stage\lib (boobst 라이브러리 디렉토리)
	* C:\local\curl-7.43.0-win64\lib (curl 라이브러리 디렉토리)
	* C:\OpenSSL-Win64\lib (open ssl 라이브러리 디렉토리)
	* C:\Program Files\MySQL\MySQL Connector C 6.1\lib (MySQL Connector 라이브러리 디렉토리)
1. 구성 속성 > 링커 > 입력 > 추가 종속성 
	* Gamnet.lib
	* libcurl.lib
	* ssleay32.lib
	* libeay32.lib
	* libmysql.lib

## Configuration
* gamnet 프레임워크를 이용하여 개발된 서버를 구동하기 위해서는, 특정한 포멧을 가진 xml 파일이 필요하다. 
* 설정 파일에서는 gamnet 프레임워크 서버가 사용하게 될 tcp 리슨 포트, 로그 디렉토리, 데이터베이스 연결 정보 등등의 정보를 저장한다. 
* 포멧만 맞다면 파일이름에 대한 제약 사항은 없으며 본 예제에서는 'config.xml'라는이름을 사용한다.
* 비주얼스튜디오프 로젝트에 config.xml 파일을 추가 하고 아래와 같이 작성 해보자.

### config.xml
```xml
<?xml version="1.0" encoding="utf-8" ?>
<server>
	<log path="log" prefix="BasicStandAloneServer" max_file_size="5">
		<dev console="yes" file="yes" sys="no"/>
		<inf console="yes" file="yes" sys="no"/>
		<wrn console="yes" file="yes" sys="no"/>
		<err console="yes" file="yes" sys="no"/>
	</log>

	<tcp port="10001" max_count="8000" accept_queue="4000" keep_alive="30"/>
</server>
```
### log
log 항목은 gamnet 프레임워크 서버가 실행 중에 생성하는 로그를 기록하는 위치, 파일의 네이밍, 로그 레벨에 따른 기록 방식에 대한 차이들을 서술 한다.
- log.path
	* 로그 파일이 생성 될 디렉토리 지정. 절대 경로 상대 경로 모두 가능하다. 
	* 예제에서는 log라는 상대 경로를 설정했고, 서버를 실행하면 실행 위치에 log라는 디렉토리와 내부에 로그 파일들이 생성되는 것을 볼수 있다.
- log.prefix : 
	* 생성 되는 로그 파일명 앞에 붙는 문자열. 
	* 예제에서는 BasicStandAloneServer를 사용하였으므로 BasicStandAloneServer_YYYYMMDD.txt 형식으로 파일 이름이 정해진다.
- log.max_file_size : 
	* 로그 파일의 크기가 너무 커지면 파일을 열지 못하거나 오랜 시간이 걸릴 수 있다. 그것을 방지하기 위해 지정된 사이즈 보다 커지면 파일 이름 뒤에 생성 시각을 추가하여 신규 파일을 생성하여 계속 로깅을 한다.
	* max_file_size는 메가바이트 단위다. 예제에서는 5메가 바이트 이상이 되면 새로운 로그 파일을 생성하도록 설정 했다.
- 로그 레벨 : 
	* gamnet 에서는 아래와 같이 네가지 로그 레벨을 제공하고 있으며 각 로그 레벨에 따라 출력 방식을 개별 설정 할 수 있다. 예를 들어 개발 환경에서는 dev레벨 로그를 활성화 시키고, 실제 서비스 환경에서는 비활성화 시키는 방식으로 불필요한 로그들이 생성되는 것을 방지 할 수 있다.
	* dev : 디버깅, 개발용 레벨, inf : 정보 출력 레벨, wrn : 경고 레벨, err : 에러 레벨
- 로그 레벨 별 출력
	* console : "yes" 또는 "no" 를 지정 할 수 있으며 콘솔 출력 여부를 지정한다.
	* file : "yes" 또는 "no" 를 지정 할 수 있으며 파일 출력 여부를 지정한다.
	* sys : "yes" 또는 "no" 를 지정 할 수 있으며 윈도우의 경우 event 로그, 리눅스의 경우 syslog 출력 여부를 지정한다.
	
### tcp 리스너 설정
```xml
<tcp port="10001" max_count="8000" accept_queue="4000" keep_alive="30"/>
```
- tcp.port : 서버가 대기하게 될 리슨 포트
- tcp.max_count : 서버가 동시에 받아 들일 수 있는 최대 connection 개수
- tcp.accept_queue : 접속 대기 큐 사이즈
- tcp.keep_alive : 
	* tcp 접속이 끊어 졌을 때 세션을 삭제하지 않고 유지 시켜주는 시간(초).
	* 접속 네트워크가 변경되어 순단 현상이 빈번한 모바일 환경을 위한 기능이다. 접속 종료와 함께 세션을 삭제하고 싶다면 0으로 설정한다.

이상 gamnet프레임워크 서버를 구동하기 위해 필요한 기본 설정들을 알아 보았다. 위에 언급된것들 말고도 데이터베이스 사용과 서버간 통신을 위해 필요한 설정들이 있지만 해당 내용들은 다른 예제에서 살펴 보도록 하자.

## Writing Code
기본적인 설정을 완료했으니 이제 본격적으로 gamnet프레임워크를 이용한 서버 코드 작성에 대해서 알아 보도록 한다.
### include
gamnet을 사용하기 위해서는 가장 먼저 gamnet의 헤더파일을 인클루드 할 필요가 있다. main 함수를 작성하기 위해 프로젝트에 Server.cpp 파일을 추가하고 gamnet 헤더를 인클루드 한다.
```cpp
#include <Gamnet/Gamnet.h>
```
### Session
* Session이란 gamnet 서버에 접속하는 클라이언트 연결을 추상화한 클래스다. 이 클래스에 서비스를 위해 각 클라이언트 마다 저장이 필요한 데이터들을 추가한다. 
* Gamnet::Network::Tcp::Session 클래스를 상속 받아 아래의 이벤트 함수들을 오버라이드하면 특정 이벤트가 발생 할 시 지정된 함수들이 호출 된다. 이벤트에 대한 설명들은 아래 코드에 주석으로 설명했다.
```cpp
class Session : public Gamnet::Network::Tcp::Session
{
public:
	virtual void OnCreate() override; // tcp connection이 맺어지고 Session 객체가 생성 되면 호출
	virtual void OnAccept() override; // 세션의 핸드쉐이크가 완료 되거나, 순단 이후 재접속이 이루어지면 호출
	virtual void OnClose(int reason) override; // 세션의 tcp connection이 끊어지면 호출
	virtual void OnDestroy() override;  // 세션의 tcp connection 종료 이후 keep_alive 시간이 초과하거나 
	                                    // 클라이언트에서 명시적으로 접속을 종료 하면 호출
};
```
### 예외 발생시 덤프 처리
* 세션 클래스의 작성이 완료 되었으면 main 함수에 gamnet 프레임워크 초기화에 필요한 코드들을 해보자. 가장 먼저 추가 할 것은 서버 실행 중 예외가 발생 했을 때 덤프를 남기는 InitCrashDump() 함수다. 
* 예외가 발생 하면 실행 위치에 덤프 파일이 생성 되게 되며, 윈도우 운영체제에서만 동작한다. 
* 리눅스의 경우는 os에서 제공하는 coredump를 이용한다.
```cpp
Gamnet::InitCrashDump();
```
### 로그 라이브러리 초기화
로그 라이브러리 초기화에선 위에서 작성한 config.xml의 경로를 인자로 넘겨 준다. 로그 라이브러리는 config 파일에 적힌 정보들을 이용하여 로그를 남길 디렉토리, 로그 파일의 이름 형식에 따라 로그를 생성한다.
```cpp
const std::string config_path = "config.xml";
Gamnet::Log::ReadXml(config_path);
```
### tcp 리스너 초기화
역시 config.xml을 이용하여 tcp 리스너에게 리슨 포트, 최대 접속 가능 개수 등을 알려 준다
```cpp
Gamnet::Network::Tcp::ReadXml<UserSession>(config_path);
```
### 서버 실행
서버에서 유지하는 스레드 풀의 크기와 함께 서버 구동을 시작한다.
```cpp
int thread_count = std::thread::hardware_concurrency();
Gamnet::Run(thread_count);
```
이렇게 해서 빌드가 성공하면 기본적인 서버 실행이 가능하다. 하지만 아직 tcp 접속을 받아 들이는것 외에는 아무것도 할 없는 반쪽 짜리 서버다. 다시 비주얼스튜디오로 돌아가 프로젝트에 HelloWorld 메시지를 받고, 응답을 보낼 수 있는 기능을 추가해 보자.
### 메시지 추가
* gamnet 프레임워크는 메시지를 위해 idl(interface definition language) 컴파일러를 사용한다. 
* C++ 구조체 선언과 비슷한 형식의 메시지 구조체를 선언해주면 idl 컴파일러는 해당 메시지를 C++ 구조체와 그것을 시리얼라이즈/디시리얼라이즈 할 수 있는 코드를 생성 한다. 
* 프로젝트에 Message.idl 이라는 파일을 추가하고 메시지 구조체를 정의한다. (자세한 idl 문법은 다음 링크 참조)
```cpp
// 클라이언트에서 서버로 요청하는 메시지
// 구조체 뒤에 : 0000002 은 메시지 아이디다.
message MsgCliSvr_HelloWorld_Req : 0000001
{
	string greeting;
};
// 서버에서 클라이언트로 응답하는 메시지
message MsgSvrCli_HelloWorld_Ans : 0000001
{
	string answer;
};
```
메시지 구조체 정의가 완료 되었으니 이제 프로젝트 빌드시 idl 파일도 함께 컴파일 될 수 있도록 설정 한다.

"파일 속성 > 일반 > 항목 형식 > 사용자 빌드 도구 선택 > 확인" 사용자 커스텀 빌드 도구를 사용 할 수 있도록 설정 하고 창을 닫은 후, 다시 "파일 속성 > 사용자 지정 빌드 도구" 를 선택하여 아래와 같이 입력한다.
* 명령줄 : ..\..\x64\Debug\idlc.exe -lcpp  %(Filename).idl
* 설명 : idl build : ..\..\x64\Debug\idlc.exe -lcpp  %(Filename).idl
* 출력 : %(Filename).h

프로젝트를 빌드하면 Message.idl 파일이 사용자 지정 빌드를 통해 idlc 컴파일러에 의해 컴파일 되고 결과 파일로 Message.h 파일을 생성한다. Message.h 파일을 인클루드 하도록 한다.
```cpp
#include "Message.h"
```
### 메시지 핸들러 추가
gamnet 프레임워크를 이용해 클라이언트로 부터 전송되어 오는 메시지를 처리하기 위해서는 Gamnet::Network::IHandler를 상속 받은 핸들러 클래스를 작성하고 어떤 메시지 수신시 해당 핸들러 클래스의 어떤함수를 호출해서 처리하면 되는지 알려주어야 한다.
```cpp
class Handler_HelloWorld : public Gamnet::Network::IHandler {
public:
    // 클라이언트로 부터 MsgCliSvr_HelloWorld_Req 메시지 수신 시 처리하는 핸들러 함수
	void Recv_CliSvr_Req(const std::shared_ptr<Session>& session, const MsgCliSvr_HelloWorld_Req& req)
	{
	    // 서버가 클라이언트로 전송할 응답 메시지
		MsgSvrCli_HelloWorld_Ans ans;
		try {
			LOG(DEV, "MsgCliSvr_HelloWorld_Req(session_key:", session->session_key, ", message:", req.greeting, ")");
			ans.answer = req.greeting; // 받은 문자열을 그대로 복사해 돌려 준다
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
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
```
#### 핸들러 함수
클라이언트로 부터 수신한 메시지를 처리하는 핸들러 함수는 아래와 같은 형식을 가진다 :
```cpp
void handler(const std::shared_ptr<SessionType>& session, const MessageType& message);
```
* SessionType : 위에서 Gamnet::Network::Tcp::Session를 상속 받아 작성한 Session 클래스의 타입. 메시지를 전송한 세션의 인스턴스가 넘어 온다.
* MessageType : idl 파일에 작성한, 클라이언트로 부터 수신하는 메시지 타입. 위의 경우는 MsgCliSvr_HelloWorld_Req 다.

#### GAMNET_BIND_TCP_HANDLER
```cpp
GAMNET_BIND_TCP_HANDLER(session_type, message_type, class_type, func, policy)
```
메시지와 핸들러 함수를 바인딩 시켜주는 매크로
위의 의미는 Session으로 등록된 포트에서 MsgCliSvr_HelloWorld_Req 메시지가 수신 되면 Handler_HelloWorld 클래스의 Recv_CliSvr_Req 함수를 호출 하라는 뜻이다.
HandlerStatic는 요청을 처리하기 위해 싱글톤 핸들러 객체를 사용한다는 뜻이고, 매 요청마다 각각의 핸들러 객체를 사용하기 위해서는 HandlerCreate를 사용한다.

이제 서버는 어느 정도 완성 되었다. 예상 대로라면 클라이언트가 서버에 접속해서 MsgCliSvr_HelloWorld_Req 메시지를 보내면 서버는 그에 대한 응답으로 MsgSvrCli_HelloWorld_Ans를 전송 할 것이다. 하지만 아직 클라이언트가 없기에 실제 서버가 정상 동작하는지 알 길이 없다. 
gamnet 프레임워크는 클라이언트 없이도 서버 코드 상에서 메시지 테스트를 할 수 있는 유틸리티들을 제공하고 있다. 아래 부터는 스트 코드를 작성하는 방법에 대해서 알아 보도록 하자.

### TestSession
테스트를 추가 하기 위해서는 위에 Session 클래스를 작성 했던 것 처럼, 클라이언트를 대신하는 Gamnet::Test::Session를 상속한 클라이언트 세션이 필요하다.
```cpp
class TestSession : public Gamnet::Test::Session {
public:
	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};
```
### 테스트 메시지 핸들러 추가
테스트 세션 작성을 마쳤으면 이번에는 테스트 함수를 만들어 보자. 예제에서는 간단하게 메시지를 보내고 받기만 하는 코드를 작성 할 것이다. 필요에 따라 테스트 함수 내에서 복잡한 연산을 수행할 수 도 있다.
```cpp
// MsgCliSvr_HelloWorld_Req 전송을 담당하는 함수
void Test_CliSvr_HelloWorld_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_HelloWorld_Req req;
	req.greeting = "World Hello";
	Gamnet::Test::SendMsg(session, req);
}

// MsgSvrCli_HelloWorld_Ans 수신시 호출 되는 함수
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
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next(); // 다음 테스트 케이스로 진행
}

// "Test_HelloWorld"라는 이름에 테스트 함수를 바인딩
GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_HelloWorld", // 테스트 케이스 이름
	MsgCliSvr_HelloWorld_Req, Test_CliSvr_HelloWorld_Req,
	MsgSvrCli_HelloWorld_Ans, Test_SvrCli_HelloWorld_Ans
);
```
### 테스트를 위한 config.xml 수정
이전에 작성 했던 config.xml에 아래의 내용을 추가 하자. 위에서 작성한 테스트 케이스를 config.xml을 통해 등록한다. 만일 더 이상 테스트 할 필요가 없는 테스트 케이스라면 설정 파일에서 지워주기만 하면 된다.
```xml
<test host="127.0.0.1" port="10001" session_count="1" loop_count="1">
	<message name="Test_HelloWorld"/>
</test>
```
* test.host : 테스트 클라이언트가 접속하게 될 서버의 주소와 포트
* test.session_count : 테스트 클라이언트의 동시 생성 갯수. 대량 접속 테스트를 할 경우 session_count를 증가하여 테스트 한다.
* test.loop_count : 테스트 횟수. 
* message : 테스트를 수행 할 테스트 케이스 이름

예를 들어 session_count를1000, loop_count를 10000이라고 적으면 동시에 1000개의 클라이언트가 생성되어 Test_HelloWorld 테스트를 진행하고 접속 종료하는 행위를 10000번 반복 한다는 의미이다.

### 테스트 라이브러리 초기화
다시 코드로 돌아와 main 함수에 테스트 케이스들을 초기화하고 실행하는 코드를 추가한다.
```cpp
Gamnet::Test::ReadXml<TestSession>(config);
```
이제 서버를 실행하면 봇 클라이언트가 생성되어 서버와 메시지를 주고 받는 것을 볼 수 있다.

### request/answer 구조가 아닌 가 아닌 서버에서 notification을 보내는 메시지 테스트
클라이언트로 부터 메시지를 받고 그에 대한 응답을 보내는 경우도 있지만 서버에서 능동적으로 클라이언트에게 메시지를 보내는 경우도 있다. 이런 경우에는 GAMNET_BIND_TEST_RECV_HANDLER를 이용해 수신 전용 핸들러를 테스트 코드에 추가해야 한다.

먼저 서버에서 클라이언트로 능동적으로 보내는 메시지를 Message.idl에 추가하자.
```cpp
message MsgSvrCli_Welcome_Ntf : 0000002
{
	string greeting;
};
```
그리고 Session 클래스의 OnAccept 함수에 클라이언트로 MsgSvrCli_Welcome_Ntf를 보내는 코드를 추가한다. 클라이언트와 서버간에 접속이 완료 되면 서버는 클라이언트에게 MsgSvrCli_Welcome_Ntf를 전송한다.
```cpp
class Session : public Gamnet::Network::Tcp::Session
{
    // ... 생략 ...
	virtual void OnAccept() override
	{
		LOG(DEV, "session_key:", session_key);
		MsgSvrCli_Welcome_Ntf ntf;
		ntf.greeting = "Welcome";
		Gamnet::Network::Tcp::SendMsg(std::static_pointer_cast<Session>(shared_from_this()), ntf);
	}
	// ... 생략 ...
};
```
테스트 클라이언트가 위 메시지를 처리 할 수 있도록 핸들러 함수를 작성하고 메시지와 바인드 시키는 작업이 필요하다.
```cpp
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
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	MsgSvrCli_Welcome_Ntf, Test_SvrCli_Welcome_Ntf
);
```
이번에는 서버로 부터 메시지 수신만을 하기 때문에 수신 테스트 핸들러 전용 바인딩 매크로인 GAMNET_BIND_TEST_RECV_HANDLER를 이용한다. 테스트 케이스의 이름을 지정한다면 지정한 테스트케이스가 수행된느 단계에서면 핸들러가 유효하고 다른 단계에서 바인딩된 메시지를 수신하면 에러로 취급한다. 만일 같은 메시지라도 테스트케이스마다 다른 처리를 해줘야 하는 경우 이름을 명시적으로 지정하여 유용하게 사용할 수 있으며, 위 예제 처럼 ""를 설정하게 되면 세션이 살아 있는 동안 항상 유효하다.

이제 빌드하고 서버를 다시 시작하면 테스트 클라이언트가 서버에 접속하자 마자 서버로 부터 Welcome 메시지를 수신하게 된다.

이상 gamnet 프레임워크를 이용해 서버를 간단한 스탠드 얼론 서버를 작성하는 방법을 알아 보았다. 이 외에도 데이터베이스 사용 서버간 통신, 백엔드 서버(구글, ios 기타 인증 서버 같은 것들)와 http 통신등 다양한 기능을 제공하고 있으니 다른 예제들도 살펴 볼 것을 권한다. 문의 사항은 kukuta@gmail.com으로 보내면 성심성의것 답변 드리겠다.
