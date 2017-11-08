namespace Gamnet {
	public class ErrorCode {
		public const int Success = 0;
		public const int ConnectFailError = 1;
		public const int ReconnectFailError = 2;
		public const int HeartbeatFailError = 3;
		public const int SendMsgFailError = 4;
		public const int ConnectTimeoutError = 10;
		public const int MessageFormatError = 70;
		public const int DuplicateConnectionError = 80;
        public const int UnhandledMsgError = 101;
		public const int UndefinedError = 999;
	}
	public class Exception : System.Exception {
		private int _error_code;
		public int ErrorCode {
			get {
				return _error_code;
			}
		}
		public Exception() : base() {}
		public Exception(int error_code) : base() {}
		public Exception(int error_code, string message) : base(message) {
			_error_code = error_code;
		}
	}
}