bison -d 5.y
lex 5.lex
clang++ lex.yy.c 5.tab.c `llvm-config --cppflags --ldflags --libs` -lSDL2 -o lex5
cat 5.txt 
cat 5.txt | ./lex5