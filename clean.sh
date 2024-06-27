#!/bin/sh

rm -r ./build
rm -r ./Lib/Linux

cd ./Depend/lua/
make clean
cd ../..

cd ./Depend/http-parser/
make clean
cd ../..
