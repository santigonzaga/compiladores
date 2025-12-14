CXX = g++
CC = gcc
CXXFLAGS = -std=c++11 -Wall
CFLAGS = -Wall

# Arquivos objeto
OBJS = lex.yy.o parser.tab.o main.o symbols.o ast.o semantic.o tac.o

# Alvo principal
target: etapa5

# Compilação do executável
etapa5: $(OBJS)
	$(CXX) $(OBJS) -o etapa5

# Compilação dos arquivos objeto C++
main.o: main.cpp symbols.hpp ast.hpp semantic.hpp tac.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

symbols.o: symbols.cpp symbols.hpp
	$(CXX) $(CXXFLAGS) -c symbols.cpp

ast.o: ast.cpp ast.hpp symbols.hpp tac.hpp
	$(CXX) $(CXXFLAGS) -c ast.cpp

semantic.o: semantic.cpp semantic.hpp ast.hpp symbols.hpp
	$(CXX) $(CXXFLAGS) -c semantic.cpp

tac.o: tac.cpp tac.hpp symbols.hpp
	$(CXX) $(CXXFLAGS) -c tac.cpp

# Geração do parser com bison (gera arquivos .c e .h)
parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

# Compilação do parser (arquivo C)
parser.tab.o: parser.tab.c parser.tab.h symbols.hpp ast.hpp
	$(CXX) $(CXXFLAGS) -c parser.tab.c

# Geração do scanner com flex
lex.yy.c: scanner.l parser.tab.h
	flex scanner.l

# Compilação do scanner (arquivo C gerado pelo flex)
lex.yy.o: lex.yy.c parser.tab.h symbols.hpp ast.hpp
	$(CXX) $(CXXFLAGS) -c lex.yy.c

# Limpeza
clean:
	rm -f etapa5 etapa4 lex.yy.c parser.tab.c parser.tab.h *.o

.PHONY: target clean