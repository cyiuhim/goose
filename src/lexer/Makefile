CXX=g++ 
CXXFLAGS= -std=c++20 -Wall -g -MMD
INCLUDES=-I../../include -I../../include/lexer

CPPFILES=${wildcard *.cpp}
OBJFILES=${CPPFILES:.cpp=.o}
DEPENDS=${CPPFILES:.cpp=.d}
OUTDIR=../
OBJ= ${OUTDIR}lexer.o

all: ${OBJ}

${OBJ}: ${OBJFILES}
	${CXX} -r $^ -o $@

%.o: %.cpp 
	${CXX} ${CXXFLAGS} ${INCLUDES} -c $< -o $@

-include ${DEPENDS}

.PHONY: all clean 
clean:
	rm ${OBJ} ${OBJFILES} ${DEPENDS}