#!/bin/bash
# install config

pkg_name=gamnet-linux-build-essential
boost_dir=boost_1_55_0
#mysqlconnector_dir=mysql-connector-c++-1.1.3-linux-glibc2.3-x86-64bit
idlc=idlc

mkdir -p $pkg_name/usr/local/include
echo "create $pkg_name/usr/local/include"
mkdir -p $pkg_name/usr/local/lib
echo "create $pkg_name/usr/local/lib"
mkdir -p $pkg_name/usr/local/bin
echo "create $pkg_name/usr/local/bin"
mkdir -p $pkg_name/DEBIAN
echo "create $pkg_name/DEBIAN"
cp control $pkg_name/DEBIAN
cp postinst $pkg_name/DEBIAN

if [ ! -d "src/$boost_dir" ]; then
	cd src
	tar -xvf $boost_dir.tar.gz
	cd -
fi

#if [ ! -d "src/$mysqlconnector_dir" ]; then
#	cd src
#	tar -xvf $mysqlconnector_dir.tar.gz
#	cd -
#fi

if [ ! -d "$pkg_name/usr/local/include/boost" ]; then
	cd src/$boost_dir/
	./bootstrap.sh --prefix=../../$pkg_name/usr/local/
	./b2 install #./bjam link=static
	cd - 
fi

#if [ ! -f "$pkg_name/usr/local/include/mysql_driver.h" ]; then
#	cp -r src/$mysqlconnector_dir/include $pkg_name/usr/local
#	cp -r src/$mysqlconnector_dir/lib $pkg_name/usr/local
#fi

#if [ ! -d "src/json_spirit_v4.06" ]; then
#	cd src;	tar -xvf json_spirit_v4.06.tar.gz
#	cd -
#fi

#if [ ! -f "$pkg_name/usr/local/lib/libjson_spirit.a" ]; then
#	cd src/json_spirit_v4.06; cmake CMakeLists.txt
#	cd json_spirit; make
#	cd ../../../
#	cp src/json_spirit_v4.06/json_spirit/*.h $pkg_name/usr/local/include 
#	cp src/json_spirit_v4.06/json_spirit/*.a $pkg_name/usr/local/lib
#fi

if [ ! -f "$pkg_name/usr/local/bin/idlc" ]; then
    cd src/idlc/Debug
    make clean; make
    cd -
    cp src/idlc/Debug/idlc $pkg_name/usr/local/bin
fi

dpkg -b $pkg_name

#cp ./$pkg_name.deb ../down

