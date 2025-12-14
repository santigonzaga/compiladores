/*
 * Compiladores - etapa5 - tac.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Implementação das funções para Three Address Code (TAC)
 */

#include "tac.hpp"
#include <iostream>
#include <cstdio>
#include <vector>

// Criar um novo TAC
TAC* tacCreate(int type, SymbolNode* res, SymbolNode* op1, SymbolNode* op2) {
    TAC* newTac = new TAC();
    newTac->type = type;
    newTac->res = res;
    newTac->op1 = op1;
    newTac->op2 = op2;
    newTac->prev = nullptr;
    newTac->next = nullptr;
    return newTac;
}

// Unir duas listas de TACs (lista invertida)
// Retorna ponteiro para o último TAC da lista resultante
TAC* tacJoin(TAC* tac1, TAC* tac2) {
    if (!tac1) return tac2;
    if (!tac2) return tac1;

    // tac1 é o último da primeira lista (executado por último na lista 1)
    // tac2 é o último da segunda lista (executado por último na lista 2)
    // Queremos: primeiro(tac1) -> ... -> tac1 -> primeiro(tac2) -> ... -> tac2
    // Ou seja, tac2 vem DEPOIS de tac1 na ordem de execução

    // Encontrar o primeiro da segunda lista
    TAC* first2 = tac2;
    while (first2->prev) {
        first2 = first2->prev;
    }

    // Conectar o primeiro da segunda lista ao último da primeira
    first2->prev = tac1;

    // Retornar o último da segunda lista (que agora é o último de tudo)
    return tac2;
}

// Obter nome do tipo de TAC
const char* tacTypeName(int type) {
    switch(type) {
        case TAC_SYMBOL:     return "TAC_SYMBOL";
        case TAC_MOVE:       return "TAC_MOVE";
        case TAC_ADD:        return "TAC_ADD";
        case TAC_SUB:        return "TAC_SUB";
        case TAC_MUL:        return "TAC_MUL";
        case TAC_DIV:        return "TAC_DIV";
        case TAC_MOD:        return "TAC_MOD";
        case TAC_LT:         return "TAC_LT";
        case TAC_GT:         return "TAC_GT";
        case TAC_LE:         return "TAC_LE";
        case TAC_GE:         return "TAC_GE";
        case TAC_EQ:         return "TAC_EQ";
        case TAC_DIF:        return "TAC_DIF";
        case TAC_AND:        return "TAC_AND";
        case TAC_OR:         return "TAC_OR";
        case TAC_NOT:        return "TAC_NOT";
        case TAC_NEG:        return "TAC_NEG";
        case TAC_LABEL:      return "TAC_LABEL";
        case TAC_BEGINFUN:   return "TAC_BEGINFUN";
        case TAC_ENDFUN:     return "TAC_ENDFUN";
        case TAC_IFZ:        return "TAC_IFZ";
        case TAC_JUMP:       return "TAC_JUMP";
        case TAC_CALL:       return "TAC_CALL";
        case TAC_ARG:        return "TAC_ARG";
        case TAC_RET:        return "TAC_RET";
        case TAC_PRINT:      return "TAC_PRINT";
        case TAC_READ:       return "TAC_READ";
        case TAC_VEC_READ:   return "TAC_VEC_READ";
        case TAC_VEC_WRITE:  return "TAC_VEC_WRITE";
        case TAC_VEC_ACCESS: return "TAC_VEC_ACCESS";
        default:             return "TAC_UNKNOWN";
    }
}

// Imprimir uma instrução TAC
void tacPrintSingle(TAC* tac) {
    if (!tac) return;

    // TAC_SYMBOL é auxiliar, não deve ser impresso
    if (tac->type == TAC_SYMBOL) return;

    fprintf(stderr, "TAC(%-14s", tacTypeName(tac->type));

    if (tac->res)
        fprintf(stderr, ", %s", tac->res->text.c_str());
    else
        fprintf(stderr, ", NULL");

    if (tac->op1)
        fprintf(stderr, ", %s", tac->op1->text.c_str());
    else
        fprintf(stderr, ", NULL");

    if (tac->op2)
        fprintf(stderr, ", %s", tac->op2->text.c_str());
    else
        fprintf(stderr, ", NULL");

    fprintf(stderr, ");\n");
}

// Reverter lista de TACs (de lista invertida para lista normal)
TAC* tacReverse(TAC* last) {
    if (!last) return nullptr;

    TAC* current = last;
    TAC* newLast = last;
    TAC* temp = nullptr;

    // Inverter os ponteiros prev/next
    while (current) {
        temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        current = temp;
    }

    // temp está no elemento anterior ao primeiro (NULL)
    // newLast está no antigo último (agora é o primeiro)

    // Encontrar o novo primeiro (que era o antigo último)
    TAC* first = newLast;
    while (first->prev) {
        first = first->prev;
    }

    return first;
}

// Imprimir lista de TACs (lista invertida)
// Coleta todos os TACs em um vetor, depois imprime na ordem correta
void tacPrintBackwards(TAC* last) {
    if (!last) return;

    // Coletar todos os TACs em ordem inversa
    std::vector<TAC*> tacs;
    TAC* current = last;
    while (current) {
        tacs.push_back(current);
        current = current->prev;
    }

    // Imprimir na ordem correta (do último para o primeiro do vetor)
    std::cerr << "\n========== CÓDIGO TAC GERADO ==========" << std::endl;
    for (int i = tacs.size() - 1; i >= 0; i--) {
        tacPrintSingle(tacs[i]);
    }
    std::cerr << "========================================\n" << std::endl;
}

// Imprimir lista de TACs (lista normal - forward)
void tacPrintForward(TAC* first) {
    if (!first) return;

    std::cerr << "\n========== CÓDIGO TAC GERADO ==========" << std::endl;
    TAC* current = first;
    while (current) {
        tacPrintSingle(current);
        current = current->next;
    }
    std::cerr << "========================================\n" << std::endl;
}

// Liberar memória da lista de TACs
void tacFree(TAC* first) {
    TAC* current = first;
    while (current) {
        TAC* temp = current->next;
        delete current;
        current = temp;
    }
}
