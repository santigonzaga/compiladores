/*
 * Compiladores - etapa2 - symbols.hpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Definições para a tabela de símbolos
 */

#ifndef SYMBOLS_HPP
#define SYMBOLS_HPP

#include <map>
#include <string>

// Definições dos tipos de símbolos na tabela
#define SYMBOL_LIT_INT 1
#define SYMBOL_LIT_CHAR 2
#define SYMBOL_LIT_FLOAT 3
#define SYMBOL_LIT_BOOL 4
#define SYMBOL_LIT_STRING 5
#define SYMBOL_IDENTIFIER 7

// Estrutura de um nodo na tabela de símbolos
struct SymbolNode {
  int type;         // Tipo do símbolo (usando as constantes acima)
  std::string text; // Texto do lexema

  SymbolNode() : type(0), text("") {}
  SymbolNode(int t, const std::string &txt) : type(t), text(txt) {}
};

// Classe para gerenciar a tabela de símbolos
class SymbolTable {
private:
  std::map<std::string, SymbolNode *> table;

public:
  SymbolTable();
  ~SymbolTable();

  // Inserir um símbolo na tabela
  SymbolNode *insert(const std::string &lexeme, int type);

  // Buscar um símbolo na tabela
  SymbolNode *lookup(const std::string &lexeme);

  // Limpar a tabela
  void clear();

  // Verificar se um símbolo existe
  bool exists(const std::string &lexeme);

  // Imprimir a tabela de símbolos
  void printTable();

  // Obter nome do tipo
  std::string getTypeName(int type);
};

// Instância global da tabela de símbolos
extern SymbolTable *symbolTable;

// Função para inicializar a tabela de símbolos
void initSymbolTable();

// Função para finalizar a tabela de símbolos
void finalizeSymbolTable();

#endif // SYMBOLS_HPP