#!/bin/sh

find . -name "*.c" -exec dos2unix {} \;
find . -name "*.h" -exec dos2unix {} \;
find . -name "*.hpp" -exec dos2unix {} \;
find . -name "*.cpp" -exec dos2unix {} \;
find . -name "*.md" -exec dos2unix {} \;
find . -name "*.txt" -exec dos2unix {} \;
find . -name "*.lua" -exec dos2unix {} \;
find . -name "Makefile" -exec dos2unix {} \;
find . -name "makefile" -exec dos2unix {} \;
find . -name "*.sh" -exec dos2unix {} \;
find . -name "*.css" -exec dos2unix {} \;
find . -name "*.js" -exec dos2unix {} \;
find . -name "*.html" -exec dos2unix {} \;
find . -name "*.htm" -exec dos2unix {} \;
find . -name "*.json" -exec dos2unix {} \;
