# 安装
# apt-get install unix2dos dos2unix



# 换行符号: win to unix
find . -name "*.cpp" -exec dos2unix {} \;
find . -name "*.hpp" -exec dos2unix {} \;
find . -name "Makefile" -exec dos2unix {} \;
find . -name "CMakeLists.txt" -exec dos2unix {} \;

