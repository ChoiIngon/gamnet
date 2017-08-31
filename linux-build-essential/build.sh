#!/bin/bash
# install config

pkg_name=gamnet-linux-build-essential

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

if [ ! -f "$pkg_name/usr/local/bin/idlc" ]; then
    cd ../idlc/Debug
    make clean; make
    cd -
    cp ../idlc/Debug/idlc $pkg_name/usr/local/bin
fi

dpkg -b $pkg_name

#cp ./$pkg_name.deb ../down

