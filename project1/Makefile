GPP=g++
FLEX=flex
BISON=bison
CXX_STD = -std=c++11

CXX_DEBUG = -g -Wall
CXX_WARN = -Wall -Wextra
MAKE_PATH=./make-build-dir
CXX_FLAGS = $(CXX_DEBUG) $(CXX_STD) $(CXX_WARN)
CPP = $(GPP) $(CXX_FLAGS)

.PHONY: clean
.lex:lex.l
	$(FLEX) lex.l
.syntax:syntax.y
	$(BISON) -t -d -v syntax.y
splc: .lex .syntax
	$(CPP) -c syntax.tab.c -o parser.o  -lfl -ly
	@ar -rc libparser.a parser.o
	$(CPP) main.cpp -static -L. -lparser -o splc
	@rm -rf $(MAKE_PATH)/ lex.yy.c syntax.tab.* *.out *.o *.a *.so syntax.output
clean:
	@rm -rf $(MAKE_PATH)/ lex.yy.c syntax.tab.* *.out *.o *.a *.so syntax.output splc