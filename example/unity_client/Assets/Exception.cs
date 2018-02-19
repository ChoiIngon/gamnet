namespace Gamnet
{
    public class ErrorCode
    {
        public const int Success = 0;
        public const int ConnectFailError = 1;
        public const int ReconnectFailError = 2;
        public const int HeartbeatFailError = 3;
        public const int SendMsgFailError = 4;
        public const int ConnectTimeoutError = 10;
        public const int InvalidArgumentError = 20;
        public const int InvalidSessionTokenError = 21;
        public const int InvalidSessionError = 22;
        public const int InvalidSessionKeyError = 23;
        public const int InvalidArrayRangeError = 30;
        public const int InvalidKeyError = 40;
        public const int InvalidAddressError = 41;
        public const int InvalidLinkManagerError = 42;
        public const int InvalidHandlerError = 43;
        public const int NotInitializedError = 50;
        public const int NullPointerError = 60;
        public const int NullPacketError = 61;
        public const int MessageFormatError = 70;
        public const int DuplicateConnectionError = 80;
        public const int CreateDirectoryFailError = 90;
        public const int CreateInstanceFailError = 91;
        public const int BufferOverflowError = 100;
        public const int BufferUnderflowError = 101;
        public const int MessageSeqOmittedError = 110;
        public const int BadLexicalCastError = 120;
        public const int UnhandledMsgError = 130;
        public const int UndefinedError = 999;
    }
    public class Exception : System.Exception
    {
        private int _error_code;
        public int ErrorCode
        {
            get
            {
                return _error_code;
            }
        }
        public Exception() : base() { }
        public Exception(int error_code) : base() { }
        public Exception(int error_code, string message) : base(message)
        {
            _error_code = error_code;
        }
    }
}