namespace Gamnet {
	public class ErrorCode {
		public const int Success = 0;
		public const int ConnectFailError = 1;              // 접속 실패(접속 실패)
		public const int ReconnectFailError = 2;            // 재접속 실패(재접속 실패)
        public const int HeartbeatFailError = 3;            // 일정 시간동안 아무런 메시지도 없음(접속 종료)
		public const int SendMsgFailError = 4;              // 메시지 전송 실패(접속 종료)
		public const int ConnectTimeoutError = 10;          // 일정 시간 내에 접속 실패(접속 실패)
        public const int InvalidArgumentError = 20;         // 알순 없지만 뭔가 오류가 남(알수 없는 오류)
        public const int InvalidSessionTokenError = 21;     // 세션 인증키 오류(재접속 실패)
        public const int InvalidSessionError = 22;          // 찾고 있던 세션이 없음(재접속 실패)
        public const int InvalidSessionKeyError = 23;       // 찾고 있던 세션 키가 없음(재접속 실패)
        public const int InvalidArrayRangeError = 30;       // 배열 사이즈 보다 큼(알수 없는 오류)
        public const int InvalidKeyError = 40;              // 찾고 있는 키가 없음(알수 없는 오류)
        public const int InvalidAddressError = 41;          // 접속 하려는 주소가 없음(접속 실패)
        public const int InvalidLinkManagerError = 42;      // (알수 없는 오류)
        public const int InvalidHandlerError = 43;          // 메시지 핸들러가 없음(클라이언트와 서버 버젼 맞지 않음)
        public const int NotInitializedError = 50;          // 객체가 아직 초기화 되지 않음(알수 없는 오류)
        public const int NullPointerError = 60;             // 객체가 없음(알수 없는 오류)
        public const int NullPacketError = 61;              // 메시지 버퍼 할당 실패(알수 없는 오류)
		public const int MessageFormatError = 70;           // 메시지 포멧 에러(클라이언트와 서버 버젼 맞지 않음)
        public const int MessageTimeoutError = 71;          // 일정 시간 동안 메시지 응답이 오지 않음(메시지 응답 타임아웃)
        public const int DuplicateConnectionError = 80;     // 중복 접속(중복 접속)
        public const int CreateDirectoryFailError = 90;     // 디렉토리 생성 실패(알수 없는 오류)
        public const int CreateInstanceFailError = 91;      // 객체 생성 실패(알수 없는 오류)
        public const int BufferOverflowError = 100;         // 버퍼 오버 플로우 에러(알수 없는 오류)
        public const int BufferUnderflowError = 101;        // 버퍼 언더 플로우 에러(알수 없는 오류)
        public const int MessageSeqOmittedError = 110;      // 메시지 시퀀스 누락 오류(알수 없는 오류)
        public const int BadLexicalCastError = 120;         // 문자열 캐스팅 오류(알수 없는 오류)
        public const int UnhandledMsgError = 130;           // 처리 되지 않는 메시지 오류(알수 없는 오류)
		public const int UndefinedError = 999;              // 진짜 알수 없는 오류(알수 없는 오류)
	}
	public class Exception : System.Exception {
		private int _error_code;
		public int ErrorCode {
			get {
				return _error_code;
			}
		}
		public Exception() : base() {}
		public Exception(int error_code) : base()
        {
            _error_code = error_code;
        }
		public Exception(int error_code, string message) : base(message)
        {
			_error_code = error_code;
		}
	}
}