/*
 * Compiladores - etapa5 - tac.hpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Definições para Three Address Code (TAC) - Código de três endereços
 */

#ifndef TAC_HPP
#define TAC_HPP

#include "symbols.hpp"

// Tipos de instruções TAC
#define TAC_SYMBOL      1   // Símbolo (utilitário, não gera instrução assembly)
#define TAC_MOVE        2   // Atribuição: op1 = op2
#define TAC_ADD         3   // Soma: res = op1 + op2
#define TAC_SUB         4   // Subtração: res = op1 - op2
#define TAC_MUL         5   // Multiplicação: res = op1 * op2
#define TAC_DIV         6   // Divisão: res = op1 / op2
#define TAC_MOD         7   // Módulo: res = op1 % op2
#define TAC_LT          8   // Menor que: res = op1 < op2
#define TAC_GT          9   // Maior que: res = op1 > op2
#define TAC_LE          10  // Menor ou igual: res = op1 <= op2
#define TAC_GE          11  // Maior ou igual: res = op1 >= op2
#define TAC_EQ          12  // Igual: res = op1 == op2
#define TAC_DIF         13  // Diferente: res = op1 != op2
#define TAC_AND         14  // E lógico: res = op1 && op2
#define TAC_OR          15  // Ou lógico: res = op1 || op2
#define TAC_NOT         16  // Negação lógica: res = !op1
#define TAC_NEG         17  // Negação aritmética: res = -op1
#define TAC_LABEL       18  // Label (rótulo para desvios)
#define TAC_BEGINFUN    19  // Início de função
#define TAC_ENDFUN      20  // Fim de função
#define TAC_IFZ         21  // Desvio condicional: if op1 == 0 goto res
#define TAC_JUMP        22  // Desvio incondicional: goto res
#define TAC_CALL        23  // Chamada de função: res = call op1
#define TAC_ARG         24  // Argumento de função: arg op1
#define TAC_RET         25  // Retorno de função: return op1
#define TAC_PRINT       26  // Impressão: print op1
#define TAC_READ        27  // Leitura: read res
#define TAC_VEC_READ    28  // Leitura de vetor: res[op1] = input
#define TAC_VEC_WRITE   29  // Escrita em vetor: res[op1] = op2
#define TAC_VEC_ACCESS  30  // Acesso a vetor: res = op1[op2]

// Estrutura de um TAC (Three Address Code)
struct TAC {
    int type;               // Tipo da instrução TAC
    SymbolNode* res;        // Resultado (endereço)
    SymbolNode* op1;        // Operando 1 (endereço)
    SymbolNode* op2;        // Operando 2 (endereço)
    struct TAC* prev;       // Ponteiro para instrução anterior (lista invertida)
    struct TAC* next;       // Ponteiro para próxima instrução (lista normal)

    TAC() : type(0), res(nullptr), op1(nullptr), op2(nullptr), prev(nullptr), next(nullptr) {}
};

// Funções para manipulação de TACs

// Criar um novo TAC
TAC* tacCreate(int type, SymbolNode* res, SymbolNode* op1, SymbolNode* op2);

// Unir duas listas de TACs (lista invertida)
// Retorna ponteiro para o último TAC da lista resultante
TAC* tacJoin(TAC* tac1, TAC* tac2);

// Imprimir uma instrução TAC
void tacPrintSingle(TAC* tac);

// Imprimir lista de TACs (lista invertida - será revertida antes de imprimir)
void tacPrintBackwards(TAC* last);

// Imprimir lista de TACs (lista normal - forward)
void tacPrintForward(TAC* first);

// Reverter lista de TACs (de lista invertida para lista normal)
TAC* tacReverse(TAC* last);

// Liberar memória da lista de TACs
void tacFree(TAC* first);

// Obter nome do tipo de TAC
const char* tacTypeName(int type);

#endif // TAC_HPP
