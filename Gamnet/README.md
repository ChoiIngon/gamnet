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

### Others

- you should install the package for yourself.
- boost - http://www.boost.org/(version 1.55)
- libmysqlclient-dev, libmysql++-dev - http://dev.mysql.com/
- libcurl4-openssl-dev - https://www.openssl.org/
- json-spirit - http://www.codeproject.com/Articles/20027/JSON-Spirit-A-C-JSON-Parser-Generator-Implemented

## Build 'Gamnet'
'''
$ cd gamnet/Gamnet/Debug
$ make
'''

## Create your project and Import Libarary
