/*
 * Compiladores - etapa6 - asm.hpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Definições para geração de código assembly x86-64 AT&T
 */

#ifndef ASM_HPP
#define ASM_HPP

#include "tac.hpp"
#include "symbols.hpp"
#include <cstdio>

// Função principal para gerar código assembly
// Recebe a lista de TACs (invertida) e o arquivo de saída
void generateAsm(TAC* tacList, FILE* output);

#endif // ASM_HPP
