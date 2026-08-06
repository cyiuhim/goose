CXX=g++
CXXFLAGS= -std=c++20 -Wall -g -MMD
EXEC=../goose
INCLUDES=-I../include -I../include/ast_nodes -I../include/lexer -I../include/parser -I../include/tree_transformer

CPPFILES=$(wildcard *.cpp)
OBJECTS=${CPPFILES:.cpp=.o}
DEPENDS=${CPPFILES:.cpp=.d}

SUBDIRS = lexer parser ast_nodes tree_transformer
SUBOBJS = $(addsuffix .o, $(SUBDIRS))

.PHONY: all clean ${SUBDIRS}

all: ${EXEC}

%.o: %.cpp 
	${CXX} ${CXXFLAGS} ${INCLUDES} -c $< -o $@

$(SUBOBJS):
	$(MAKE) -C $(basename $@)

${EXEC}:${OBJECTS} ${SUBOBJS}
	${CXX} ${CXXFLAGS} ${INCLUDES} ${OBJECTS} ${SUBOBJS} -o ${EXEC}

-include ${DEPENDS}

clean:
	${RM} ${EXEC} ${OBJECTS} ${SUBOBJS} ${DEPENDS}
	find . \( -name '*.o' -o -name '*.d' \) -exec rm -f {} +
