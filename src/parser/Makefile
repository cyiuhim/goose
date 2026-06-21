CXX=g++
CXXFLAGS= -std=c++20 -Wall -g -MMD
INCLUDES=-I../../include -I../../include/lexer -I../../include/parser

CPPFILES=${wildcard *.cpp}
OBJFILES=${CPPFILES:.cpp=.o}
DEPENDS=${CPPFILES:.cpp=.d}
OUTDIR=../
OBJ= ${OUTDIR}parser.o

all: ${OBJ}

${OBJ}: ${OBJFILES}
	${CXX} -r $^ -o $@

%.o: %.cpp 
	${CXX} ${CXXFLAGS} ${INCLUDES} -c $< -o $@

-include ${DEPENDS}

.PHONY: all clean 
clean:
	rm ${OBJ} ${OBJFILES} ${DEPENDS}