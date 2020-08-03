# Gamnet::Test

Gamnet에서는 기능 및 스트레스 테스트를 위해 테스트 클라이언트 코드를 추가 할 수 있다.

# Classes, Functions & Macros

* `Gamnet::Test::Session` : 서버에 접속하여 테스트를 수행하는 클라이언트 클래스
* `Gamnet::Test::Session::Next()` : 다음 테스트 스텝으로 진행
* `template <class MSG> void SendMsg(const std::shared_ptr<Session>& session, const MSG& msg, bool reliable = true)` : 메시지를 서버로 전송
* `void send_function(const std::shared_ptr<TestSession>& session)` : 
* `void recv_function(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)` :
* `GAMNET_BIND_TEST_HANDLER(<test session type>, "<test name>", <send message type>, <send function>, <receive message type>, <receive function>)` :

## Gamnet::Test::Session 
Gamnet::Test::Session은 서버에 접속하는 클라이언트를 표현하는 클래스다. Gamnet::Test::Session 클래스를 상속 받아 서버에 접속하는 클라이언트를 구현 하도록 한다.

```cpp
#include <Gamnet/Test/Session.h>
class TestSession : public Gamnet::Test::Session {
public :

	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};
```
Gamnet::Test::Session은 OnCreate, OnConnect, OnClose, OnDestroy 가상 멤버 함수를 각 이벤트에 맞게 구현해 주어야 하며 자세한 설명은 아래와 같다.

### Member Functions

| 이름 | 설명 |
|----|----|
|OnCreate() | 테스트 세션이 생성 되면 호출. 객체가 생성 되었을 때 필요한 초기화 코드를 작성한다.|
|OnConnect() | 테스트 세션이 서버 접속에 성공하면 호출. 접속 성공 시 필요한 초기화 코드를 작성한다.|
|OnClose(int reason) | 테스트 세션의 접속이 종료되면 호출. 접속 종료 시 필요한 종료 코드를 작성한다.|
|OnDestroy() | 테스트 세션의 객체가 소멸 될 때 호출. 객체 소멸 시 필요한 종료 코드를 작성한다.|

그외 세션이 유지되는 동안 필요한 변수나 멤버 함수가 있다면 상속 받은 클래스에 자유롭게 추가 하도록 한다.

## 테스트 함수 추가

서버 테스트를 위해 메시지를 보내는 함수와 받는 함수를 각각 작성한다. 함수의 형식은 아래와 같다. Gamnet::Test::Session을 상속 받아 구현한 테스트 세션의 타입은 'TestSession'이라고 가정한다.

### send function

```
void send_function(const std::shared_ptr<test session type>& session);
```

### example
```cpp
void Test_CliSvr_SendMessage_Ntf(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_SendMessage_Ntf ntf;
	ntf.text = "Hello World";
	Gamnet::Test::SendMsg(session, ntf); // send to server
}
```

### receive function

```
void recv_function(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
```

### example

```cpp
void Test_SvrCli_SendMessage_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}
```

## 테스트 케이스 등록

GAMNET_BIND_TEST_HANDLER(
	TestSession, "<test case name>",
	<send message type>, <send function> 
	<receive message type>, <receive handler function>
);

```cpp
GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_SendMessage",
	MsgCliSvr_SendMessage_Ntf, Test_CliSvr_SendMessage_Ntf, 
	MsgSvrCli_SendMessage_Ntf, Test_SvrCli_SendMessage_Ntf
);
```

### config.xml 에 테스트 정보 추가

```xml
<test host="127.0.0.1" port="40000" session_count="1" loop_count="20">
	<message name="Test_SendMessage"/>
</test>
```

* 'test' element :
** host : 테스트 세션이 접속 할 서버의 주소
** port : 테스트 세션이 접속 할 서버의 포트
** session_count : 동시에 구동 될 테스트 세션의 개수
** loop_count : 각 세션당 반복 테스트 횟수

* 'message' element :
** name : 구동 시킬 테스트 케이스의 이름. 3번 과정 테스트 케이스 등록에서 <test case name> 에 등록된 이름을 사용한다.

## 테스트 구동

Gamnet::Test::ReadXml을 main() 함수 내에 호출 하여 config.xml에 정의 된 테스트 케이스들을 구동 시킨다.

```cpp
Gamnet::Network::Tcp::ReadXml<UserSession>(config_path);
Gamnet::Network::Http::ReadXml(config_path);
Gamnet::Test::ReadXml<TestSession>(config_path);
Gamnet::Run(vm["thread"].as<int>());
```
