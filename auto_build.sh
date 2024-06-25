#!/bin/sh

# 声明: 本auto_build.sh only for debian 10
apt-get install libhiredis-dev 
apt-get install libmariadb-dev libmariadb++-dev
apt-get install openssl libssl-dev 



# 先build 依赖
cd ./Depend/lua
make -j4
cd ../..

# 补全依赖
cd ./Depend
ln -s /usr/include/mysql

# 未改名
ln -s /usr/include/hiredis 
ln -s /usr/include/openssl 
cd ..



# 再build project
mkdir build
cd ./build
cmake -G "Unix Makefiles" .. -DCMAKE_BUILD_TYPE:STRING=RELEASE
make -j4
cd ..



# 补全lib 库
cd ./Lib/Linux
ln -s ../../Depend/lua/src/liblua.a



# 这个project 还是半成品, 很多东西未完善!! (不可靠, 慎用)
