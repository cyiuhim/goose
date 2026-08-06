CXX=g++
CXXFLAGS= -std=c++20 -Wall -g -MMD
INCLUDES=-I../../include -I../../include/ast_nodes -I../../include/lexer -I../../include/parser -I../../include/tree_transformer

CPPFILES=${wildcard *.cpp}
OBJFILES=${CPPFILES:.cpp=.o}
DEPENDS=${CPPFILES:.cpp=.d}
OUTDIR=../
OBJ= ${OUTDIR}tree_transformer.o

all: ${OBJ}

${OBJ}: ${OBJFILES}
	${CXX} -r $^ -o $@

%.o: %.cpp
	${CXX} ${CXXFLAGS} ${INCLUDES} -c $< -o $@

-include ${DEPENDS}

.PHONY: all clean
clean:
	rm -f ${OBJ} ${OBJFILES} ${DEPENDS}
