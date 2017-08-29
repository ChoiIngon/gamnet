# Step 1. Gamnet.h 인클루드 하기
리눅스 환경에서 'Gamnet 라이브러리 빌드 및 설치'를 완료 하셨다고 가정하고 설명 하도록 하겠습니다. 
Gamnet.h 파일은 Gamnet에서 제공하는 모든 헤더 파일들을 인클루드 하고 있으므로 Gamnet 사용을 위해서는 Gament.h 파일 하나만 인클루드 하면 됩니다.
```cpp
#include <Gamnet/Gamnet.h>
```
Remark :
리눅스 환경에서 설치가 완료되면 헤더 파일들은 디폴트 인클루드 패스에 복사가 되어 별다른 설정이 필요 없습니다만 디폴트 경로가 아닌 다른 곳에 헤더파일들이 위치한다면 해당 경로에 있는 'Gamnet.h'파일을 인클루드 하시면 됩니다.

# Step 2. 유저 세션 만들기
Gamnet에서는 유저와의 연결과 각 연결에 종속 되는 데이터들을 관리하기 위해 'Session' 이라는 추상 클래스를 상속 받아 등록해야 합니다


아래와 같이 make와  튜토리얼을

```cpp
class UserSession : public Gamnet::Network::Tcp::Session {
public :
	// 세션 객체가 생성 될때 호출 됩니다(사실은 풀에서 꺼내져 올때 호출 됩니다)
	void OnCreate() {
	}
	// 클라이언트로 부터 접속이 발생하면 호출 됩니다
	void OnAccept() {
	}
	// 세션의 접속이 종료 되면 호출 됩니다
	void OnClose(int reason) 
	{
	}
};
```
Remark :
Session 클래스를 상속 받은 클래스를 Gamnet 네트워크 리스너에 등록하는 것은 선택이 아닌 필수 입니다. 아무것도 하지 않는 Session이라도 반드시 만들어야 하며, 순수 가상함수들을 구현 해주어야 합니다.


# Step 3. 리스너 등록하기

```cpp
int main() {
	Gamnet::Log::ReadXml("config.xml");
	LOG(INF, "Server Starts..");
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	try {
		Gamnet::Network::Tcp::Listen<UserSession>(20000, 1024, 300);
		Gamnet::Run(std::thread::hardware_concurrency());
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
	}
	return 0;
}
