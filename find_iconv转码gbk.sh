find . -type f -name "*.txt" -exec iconv -f gbk -t utf8 {} -o {} \;
find . -type f -name "Makefile" -exec iconv -f gbk -t utf8 {} -o {} \;
find . -type f -name "*.c" -exec iconv -f gbk -t utf8 {} -o {} \;
find . -type f -name "*.h" -exec iconv -f gbk -t utf8 {} -o {} \;
find . -type f -name "*.cpp" -exec iconv -f gbk -t utf8 {} -o {} \;


