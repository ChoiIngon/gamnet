# Gamnet

'Gamnet' is boost asio based TCP/IP server framwork. It supports automatic client connction mananging, server to server communication by using logical address(not ip and port number), structured message format(..and serialize/deserialize compiler) and some helpful functions for your service.

# Features

- managing automatic create/delete client session resource
- managing network event(connect, close, receive, send) and timer event
- supports structured message and serialize/deserialize code generate compiler
- supports MySQL, SQLite, Redis
- supports server to server communication
- supports log library
- supports unit test tool
- supports functions for string, base64, md5..
- using C++ 11

# Usage
- [How to build 'Gamnet' library](https://github.com/ChoiIngon/gamnet/blob/master/Gamnet/README.md)
- [Write 'IDL(Interface Definition Language)' for data serialize/de-serialize](https://github.com/ChoiIngon/gamnet/blob/master/idlc/README.md)
- [Usage of 'Gamnet::Database'](https://github.com/ChoiIngon/gamnet/blob/master/Gamnet/Database/README.md)
- [Sample project](https://github.com/ChoiIngon/gamnet/tree/master/example)
