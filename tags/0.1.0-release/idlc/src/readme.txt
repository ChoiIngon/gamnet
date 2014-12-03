flex --nounistd --header-file=lexer.h -o lexer.cpp lexer.l
bison --defines=parser.h -o parser.cpp parser.y
