#!/bin/sh

# 声明: 本auto_build.sh only for debian 10



# 先build 依赖
cd ./Depend
cd lua
make -j4
cd ../..

# 再build project
mkdir build
cmake ..
make -j4
cd ..

# 补全依赖
cd ./Lib/Linux
ln -s ../../Depend/lua/src/liblua.a
