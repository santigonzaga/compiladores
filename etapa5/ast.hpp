/*
 * Compiladores - etapa3 - ast.hpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Definições para a Árvore Sintática Abstrata (AST)
 */

#ifndef AST_HPP
#define AST_HPP

#include "symbols.hpp"
#include <string>

// Tipos de nodos da AST
#define AST_PROGRAM 1
#define AST_DECLARATION_LIST 2
#define AST_VAR_DECLARATION 3
#define AST_VECTOR_DECLARATION 4
#define AST_FUNCTION_DECLARATION 5
#define AST_PARAMETER_LIST 6
#define AST_PARAMETER 7
#define AST_BLOCK 8
#define AST_COMMAND_LIST 9
#define AST_ASSIGNMENT 10
#define AST_VECTOR_ASSIGNMENT 11
#define AST_READ 12
#define AST_PRINT 13
#define AST_RETURN 14
#define AST_IF 15
#define AST_IF_ELSE 16
#define AST_WHILE 17
#define AST_EXPRESSION_BINOP 18
#define AST_EXPRESSION_UNOP 19
#define AST_FUNCTION_CALL 20
#define AST_VECTOR_ACCESS 21
#define AST_IDENTIFIER 22
#define AST_LITERAL 23
#define AST_LITERAL_LIST 24
#define AST_ARGUMENT_LIST 25
#define AST_PRINT_LIST 26

// Operadores para expressões
#define AST_ADD 100
#define AST_SUB 101
#define AST_MUL 102
#define AST_DIV 103
#define AST_MOD 104
#define AST_LT 105
#define AST_GT 106
#define AST_LE 107
#define AST_GE 108
#define AST_EQ 109
#define AST_DIF 110
#define AST_AND 111
#define AST_OR 112
#define AST_NOT 113

// Estrutura do nodo da AST
struct ASTNode {
    int type;                 // Tipo do nodo
    SymbolNode* symbol;       // Ponteiro para a tabela de símbolos (para folhas)
    int operator_type;        // Tipo do operador (para expressões)
    struct ASTNode* child[4]; // Ponteiros para filhos (máximo 4)

    ASTNode() : type(0), symbol(nullptr), operator_type(0) {
        for (int i = 0; i < 4; i++) {
            child[i] = nullptr;
        }
    }
};

// Funções para manipulação da AST
ASTNode* createNode(int type, SymbolNode* symbol, ASTNode* c0, ASTNode* c1, ASTNode* c2, ASTNode* c3);
ASTNode* createOperatorNode(int operator_type, ASTNode* c0, ASTNode* c1);
ASTNode* createLeafNode(int type, SymbolNode* symbol);

// Funções para impressão da AST
void printNode(ASTNode* node, int level);
void printAST(ASTNode* root);

// Função para geração de código
void generateCode(ASTNode* node, FILE* output);

// Função para liberar memória da AST
void freeAST(ASTNode* node);

// Função auxiliar para obter nome do tipo de nodo
std::string getNodeTypeName(int type);

// Função auxiliar para obter nome do operador
std::string getOperatorName(int op);

// Forward declaration for TAC
struct TAC;

// Função para gerar código TAC a partir da AST
TAC* generateTAC(ASTNode* node);

#endif // AST_HPP