#!/bin/sh

# 声明: 本auto_build.sh only for debian 10



# 安装依赖
apt-get install libhiredis-dev 
apt-get install libmariadb-dev libmariadb++-dev
apt-get install openssl libssl-dev 



# 先build 依赖
cd ./Depend/lua
make -j4
cd ../..

cd ./Depend/http-parser
make package -j4
cd ../..



# 补全依赖的头文件(需要根据linux 系统安装到位才行)
cd ./Depend
ln -s /usr/include/mysql MySQL
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
ln -s ../../Depend/http-parser/src/libhttp-parser.a



# 这个project 还是半成品, 很多东西未完善!! (不可靠, 慎用)
# 依赖list
:<<!
* mysql
* redis
* openssl
* lua5.4
!
