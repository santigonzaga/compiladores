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

// Definições dos tipos de símbolos na tabela (literais)
#define SYMBOL_LIT_INT 1
#define SYMBOL_LIT_CHAR 2
#define SYMBOL_LIT_FLOAT 3
#define SYMBOL_LIT_BOOL 4
#define SYMBOL_LIT_STRING 5
#define SYMBOL_IDENTIFIER 7

// Natureza dos identificadores (para verificação semântica)
#define NATURE_SCALAR 1
#define NATURE_VECTOR 2
#define NATURE_FUNCTION 3

// Tipos de dados (para verificação semântica)
#define DATATYPE_INT 1
#define DATATYPE_CHAR 2
#define DATATYPE_FLOAT 3
#define DATATYPE_BOOL 4

// Forward declaration para lista de parâmetros
struct ASTNode;

// Estrutura de um nodo na tabela de símbolos
struct SymbolNode {
  int type;         // Tipo do símbolo (usando as constantes acima)
  std::string text; // Texto do lexema
  int nature;       // Natureza: escalar, vetor, função (0 = não definido)
  int dataType;     // Tipo de dado: int, char, float, bool (0 = não definido)
  ASTNode* parameterList; // Lista de parâmetros (apenas para funções)

  SymbolNode() : type(0), text(""), nature(0), dataType(0), parameterList(nullptr) {}
  SymbolNode(int t, const std::string &txt) : type(t), text(txt), nature(0), dataType(0), parameterList(nullptr) {}
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

  // Obter nome da natureza
  std::string getNatureName(int nature);

  // Obter nome do tipo de dado
  std::string getDataTypeName(int dataType);
};

// Instância global da tabela de símbolos
extern SymbolTable *symbolTable;

// Função para inicializar a tabela de símbolos
void initSymbolTable();

// Função para finalizar a tabela de símbolos
void finalizeSymbolTable();

// Função para criar símbolo temporário (para geração de código TAC)
SymbolNode* makeTemp();

// Função para criar label (para geração de código TAC)
SymbolNode* makeLabel();

#endif // SYMBOLS_HPP