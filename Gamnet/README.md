## Getting source code

```
https://github.com/ChoiIngon/gamnet
```
- Github provides several way(svn, git) you can get the source code of gamnet. 

## Setting build enviroment

### Ubuntu 64bit

- gamnet provides pre-compiled debian package for your convenient
- included debian package contains pre-compiled boost library and some other needed tools.
- if your developement environment is 'Ubuntu', you just download and execute apt-get and gdebi. Frist, get the package
- to install provided package in 'Ubuntu' environment you need gdebi

```
$ sudo apt-get update 
$ sudo apt-get install gdebi-core
```

- move to 'gamnet' directory and execute 'gdebi' to install build essential

```
$ cd gamnet/linux-build-essential
$ ls build.sh control gamnet-linux-build-essential.deb postinst src
$ sudo gdebi gamnet-linux-build-essential.deb
```

### Windows
- Download and install 'boost'(It is too big to commit github. so you should download from SourceForge)
 - get binary install version : https://sourceforge.net/projects/boost/files/boost-binaries/1.61.0/
 - install boost under 'C:\local\boost_1_61_0'
- install mysql connector
 - execute '$GAMNET_ROOT/windows-build-essential/mysql-connector-c-6.1.6-winx64.msi'
 - click 'Typical' button. It will install mysql under 'C:\Program Files\MySQL\MySQL Connector C 6.1\'
- install openssl
 - execute '$GAMNET_ROOT/windows-build-essential/Win64OpenSSL-1_0_2c.exe'
 - It will install openssl under 'C:\local\OpenSSL-Win64'
- install curl
 - unzip '$GAMNET_ROOT/windows-build-essential/curl-7.43.0-win64.zip'
 - copy 'curl-7.43.0-win64' directory to 'C:\local\curl-7.43.0-win64' dirctory

### Others

- you should install the package for yourself.
- boost - http://www.boost.org/(version 1.55)
- libmysqlclient-dev, libmysql++-dev - http://dev.mysql.com/
- libcurl4-openssl-dev - https://www.openssl.org/
- json-spirit - http://www.codeproject.com/Articles/20027/JSON-Spirit-A-C-JSON-Parser-Generator-Implemented

## Build 'Gamnet'
```
$ cd gamnet/Gamnet/Debug
$ make
```

## Create your project and Import Libarary

### Ubuntu 64bit
- required library
 - Gamnet
 - boost_filesystem
 - boost_system
 - boost_thread
 - json_sprit
 - curl
 - pthread
- optional library
 - crypto
 - mysqlclient_r
