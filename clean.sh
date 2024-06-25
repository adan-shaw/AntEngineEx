#!/bin/sh

rm -r ./build

cd ./Depend/lua/
make clean
cd ../..

cd ./Depend/http-parser/
make clean
cd ../..
