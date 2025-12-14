/*
 * Compiladores - etapa6 - asm.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Implementação da geração de código assembly ARM64 para macOS (Apple Silicon)
 */

#include "asm.hpp"
#include "ast.hpp"
#include <vector>
#include <cstring>
#include <string>
#include <map>

// Declaração externa da tabela de símbolos
extern SymbolTable* symbolTable;

// Função auxiliar para verificar se um símbolo é um literal inteiro
static bool isIntLiteral(SymbolNode* sym) {
    if (!sym) return false;
    return sym->type == SYMBOL_LIT_INT;
}

// Função auxiliar para verificar se um símbolo é um literal char
static bool isCharLiteral(SymbolNode* sym) {
    if (!sym) return false;
    return sym->type == SYMBOL_LIT_CHAR;
}

// Função auxiliar para verificar se um símbolo é um literal booleano
static bool isBoolLiteral(SymbolNode* sym) {
    if (!sym) return false;
    return sym->type == SYMBOL_LIT_BOOL;
}

// Função auxiliar para verificar se um símbolo é um literal string
static bool isStringLiteral(SymbolNode* sym) {
    if (!sym) return false;
    return sym->type == SYMBOL_LIT_STRING;
}

// Função auxiliar para verificar se é uma função
static bool isFunction(SymbolNode* sym) {
    if (!sym) return false;
    return sym->nature == NATURE_FUNCTION;
}

// Contador global para strings
static int stringCounter = 0;
static std::map<std::string, std::string> stringNames;

// Função auxiliar para criar nome de variável seguro para assembly
// macOS requer prefixo _ para símbolos globais
static std::string makeAsmName(const std::string& name) {
    // Se é uma string literal (começa com "), usar nome único
    if (!name.empty() && name[0] == '"') {
        // Verificar se já tem um nome atribuído
        auto it = stringNames.find(name);
        if (it != stringNames.end()) {
            return it->second;
        }
        // Criar novo nome
        std::string newName = "_str" + std::to_string(stringCounter++);
        stringNames[name] = newName;
        return newName;
    }

    std::string result = "_";
    for (char c : name) {
        if (c == '\'') {
            result += "char";
        } else if (c == '.') {
            result += "dot";
        } else if (c == '-') {
            result += "neg";
        } else if (c == ' ') {
            result += "_";
        } else if (c == '!' || c == '?' || c == ':' || c == '=' ||
                   c == '\\' || c == '\n' || c == '\t') {
            // skip special chars
        } else {
            result += c;
        }
    }
    return result;
}

// Nome para função - macOS requer prefixo _ em todas as funções
static std::string makeFunctionName(const std::string& name) {
    return "_" + name;
}

// Função auxiliar para verificar se é um label
static bool isLabel(SymbolNode* sym) {
    if (!sym) return false;
    return sym->text.find("__label") == 0;
}

// Função para gerar seção .data com variáveis e constantes
static void generateDataSection(FILE* output) {
    fprintf(output, "// Seção de dados\n");
    fprintf(output, ".data\n\n");

    // Formato para print de inteiros
    fprintf(output, "_printint:\n");
    fprintf(output, "\t.asciz \"%%d\"\n\n");

    // Formato para print de inteiros com newline
    fprintf(output, "_printintln:\n");
    fprintf(output, "\t.asciz \"%%d\\n\"\n\n");

    // Formato para read de inteiros
    fprintf(output, "_readint:\n");
    fprintf(output, "\t.asciz \"%%d\"\n\n");
}

// Função auxiliar para carregar valor em registrador w0
// Retorna true se o valor já está em w0
static void loadToW0(SymbolNode* sym, FILE* output) {
    if (!sym) {
        fprintf(output, "\tmov w0, #0\n");
        return;
    }

    if (isIntLiteral(sym)) {
        int val = atoi(sym->text.c_str());
        if (val >= 0 && val < 65536) {
            fprintf(output, "\tmov w0, #%d\n", val);
        } else {
            fprintf(output, "\tmov w0, #%d\n", val & 0xFFFF);
            if (val > 65535 || val < 0) {
                fprintf(output, "\tmovk w0, #%d, lsl #16\n", (val >> 16) & 0xFFFF);
            }
        }
    } else if (isCharLiteral(sym)) {
        if (sym->text.length() >= 3) {
            char c = sym->text[1];
            fprintf(output, "\tmov w0, #%d\n", (int)c);
        } else {
            fprintf(output, "\tmov w0, #0\n");
        }
    } else if (isBoolLiteral(sym)) {
        if (sym->text == "true") {
            fprintf(output, "\tmov w0, #1\n");
        } else {
            fprintf(output, "\tmov w0, #0\n");
        }
    } else {
        // Variável - carregar do endereço
        std::string name = makeAsmName(sym->text);
        fprintf(output, "\tadrp x1, %s@PAGE\n", name.c_str());
        fprintf(output, "\tadd x1, x1, %s@PAGEOFF\n", name.c_str());
        fprintf(output, "\tldr w0, [x1]\n");
    }
}

// Função auxiliar para carregar valor em registrador w1
static void loadToW1(SymbolNode* sym, FILE* output) {
    if (!sym) {
        fprintf(output, "\tmov w1, #0\n");
        return;
    }

    if (isIntLiteral(sym)) {
        int val = atoi(sym->text.c_str());
        if (val >= 0 && val < 65536) {
            fprintf(output, "\tmov w1, #%d\n", val);
        } else {
            fprintf(output, "\tmov w1, #%d\n", val & 0xFFFF);
            if (val > 65535 || val < 0) {
                fprintf(output, "\tmovk w1, #%d, lsl #16\n", (val >> 16) & 0xFFFF);
            }
        }
    } else if (isCharLiteral(sym)) {
        if (sym->text.length() >= 3) {
            char c = sym->text[1];
            fprintf(output, "\tmov w1, #%d\n", (int)c);
        } else {
            fprintf(output, "\tmov w1, #0\n");
        }
    } else if (isBoolLiteral(sym)) {
        if (sym->text == "true") {
            fprintf(output, "\tmov w1, #1\n");
        } else {
            fprintf(output, "\tmov w1, #0\n");
        }
    } else {
        // Variável - carregar do endereço
        std::string name = makeAsmName(sym->text);
        fprintf(output, "\tadrp x2, %s@PAGE\n", name.c_str());
        fprintf(output, "\tadd x2, x2, %s@PAGEOFF\n", name.c_str());
        fprintf(output, "\tldr w1, [x2]\n");
    }
}

// Função auxiliar para armazenar w0 em variável
static void storeW0To(SymbolNode* sym, FILE* output) {
    if (!sym) return;
    std::string name = makeAsmName(sym->text);
    fprintf(output, "\tadrp x1, %s@PAGE\n", name.c_str());
    fprintf(output, "\tadd x1, x1, %s@PAGEOFF\n", name.c_str());
    fprintf(output, "\tstr w0, [x1]\n");
}

// Função para gerar uma instrução TAC em assembly ARM64
static void generateTacAsm(TAC* tac, FILE* output) {
    if (!tac) return;

    switch(tac->type) {
        case TAC_SYMBOL:
            // Não gera código
            break;

        case TAC_MOVE:
            // res = op1
            if (tac->res && tac->op1) {
                fprintf(output, "\t// MOVE %s = %s\n", tac->res->text.c_str(), tac->op1->text.c_str());
                loadToW0(tac->op1, output);
                storeW0To(tac->res, output);
            }
            break;

        case TAC_ADD:
            // res = op1 + op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// ADD %s = %s + %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tadd w0, w0, w1\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_SUB:
            // res = op1 - op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// SUB %s = %s - %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tsub w0, w0, w1\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_MUL:
            // res = op1 * op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// MUL %s = %s * %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tmul w0, w0, w1\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_DIV:
            // res = op1 / op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// DIV %s = %s / %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tsdiv w0, w0, w1\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_MOD:
            // res = op1 % op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// MOD %s = %s %% %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tsdiv w2, w0, w1\n");
                fprintf(output, "\tmsub w0, w2, w1, w0\n");  // w0 = w0 - w2*w1
                storeW0To(tac->res, output);
            }
            break;

        case TAC_LT:
            // res = op1 < op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// LT %s = %s < %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tcmp w0, w1\n");
                fprintf(output, "\tcset w0, lt\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_GT:
            // res = op1 > op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// GT %s = %s > %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tcmp w0, w1\n");
                fprintf(output, "\tcset w0, gt\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_LE:
            // res = op1 <= op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// LE %s = %s <= %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tcmp w0, w1\n");
                fprintf(output, "\tcset w0, le\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_GE:
            // res = op1 >= op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// GE %s = %s >= %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tcmp w0, w1\n");
                fprintf(output, "\tcset w0, ge\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_EQ:
            // res = op1 == op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// EQ %s = %s == %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tcmp w0, w1\n");
                fprintf(output, "\tcset w0, eq\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_DIF:
            // res = op1 != op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// DIF %s = %s != %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tcmp w0, w1\n");
                fprintf(output, "\tcset w0, ne\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_AND:
            // res = op1 && op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// AND %s = %s & %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\tand w0, w0, w1\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_OR:
            // res = op1 || op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// OR %s = %s | %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());
                loadToW0(tac->op1, output);
                loadToW1(tac->op2, output);
                fprintf(output, "\torr w0, w0, w1\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_NOT:
            // res = !op1
            if (tac->res && tac->op1) {
                fprintf(output, "\t// NOT %s = ~%s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str());
                loadToW0(tac->op1, output);
                fprintf(output, "\tcmp w0, #0\n");
                fprintf(output, "\tcset w0, eq\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_NEG:
            // res = -op1
            if (tac->res && tac->op1) {
                fprintf(output, "\t// NEG %s = -%s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str());
                loadToW0(tac->op1, output);
                fprintf(output, "\tneg w0, w0\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_LABEL:
            // Label para desvios
            if (tac->res) {
                fprintf(output, "%s:\n", makeAsmName(tac->res->text).c_str());
            }
            break;

        case TAC_BEGINFUN:
            // Início de função
            if (tac->res) {
                std::string funcName = makeFunctionName(tac->res->text);
                fprintf(output, "\n// Função %s\n", tac->res->text.c_str());
                fprintf(output, ".globl %s\n", funcName.c_str());
                fprintf(output, ".p2align 2\n");
                fprintf(output, "%s:\n", funcName.c_str());
                // Prólogo da função - salvar frame pointer e link register
                fprintf(output, "\tstp x29, x30, [sp, #-16]!\n");
                fprintf(output, "\tmov x29, sp\n");

                // Se a função tiver parâmetros, salvá-los
                if (tac->res->parameterList) {
                    ASTNode* paramList = tac->res->parameterList;
                    if (paramList && paramList->type == AST_PARAMETER_LIST) {
                        ASTNode* firstParam = paramList->child[0];
                        if (firstParam && firstParam->type == AST_PARAMETER) {
                            ASTNode* paramId = firstParam->child[1];
                            if (paramId && paramId->symbol) {
                                fprintf(output, "\t// Salvando parâmetro %s\n", paramId->symbol->text.c_str());
                                std::string name = makeAsmName(paramId->symbol->text);
                                fprintf(output, "\tadrp x1, %s@PAGE\n", name.c_str());
                                fprintf(output, "\tadd x1, x1, %s@PAGEOFF\n", name.c_str());
                                fprintf(output, "\tstr w0, [x1]\n");
                            }
                        }
                    }
                }
            }
            break;

        case TAC_ENDFUN:
            // Fim de função
            if (tac->res) {
                fprintf(output, "\t// Fim da função %s\n", tac->res->text.c_str());
                fprintf(output, "\tmov w0, #0\n");
                fprintf(output, "\tldp x29, x30, [sp], #16\n");
                fprintf(output, "\tret\n");
            }
            break;

        case TAC_IFZ:
            // if op1 == 0 goto res
            if (tac->res && tac->op1) {
                fprintf(output, "\t// IFZ: if %s == 0 goto %s\n",
                        tac->op1->text.c_str(), tac->res->text.c_str());
                loadToW0(tac->op1, output);
                fprintf(output, "\tcbz w0, %s\n", makeAsmName(tac->res->text).c_str());
            }
            break;

        case TAC_JUMP:
            // goto res
            if (tac->res) {
                fprintf(output, "\t// JUMP: goto %s\n", tac->res->text.c_str());
                fprintf(output, "\tb %s\n", makeAsmName(tac->res->text).c_str());
            }
            break;

        case TAC_CALL:
            // res = call op1
            if (tac->res && tac->op1) {
                fprintf(output, "\t// CALL %s = %s()\n",
                        tac->res->text.c_str(), tac->op1->text.c_str());
                fprintf(output, "\tbl %s\n", makeFunctionName(tac->op1->text).c_str());
                storeW0To(tac->res, output);
            }
            break;

        case TAC_ARG:
            // Argumento de função - usa registrador w0 (primeiro arg em ARM64)
            if (tac->res) {
                fprintf(output, "\t// ARG %s\n", tac->res->text.c_str());
                loadToW0(tac->res, output);
            }
            break;

        case TAC_RET:
            // return op1
            fprintf(output, "\t// RET\n");
            if (tac->op1) {
                loadToW0(tac->op1, output);
            } else {
                fprintf(output, "\tmov w0, #0\n");
            }
            fprintf(output, "\tldp x29, x30, [sp], #16\n");
            fprintf(output, "\tret\n");
            break;

        case TAC_PRINT:
            // print op1
            if (tac->op1) {
                fprintf(output, "\t// PRINT %s\n", tac->op1->text.c_str());

                if (isStringLiteral(tac->op1)) {
                    // Print string - usar puts
                    std::string name = makeAsmName(tac->op1->text);
                    fprintf(output, "\tadrp x0, %s@PAGE\n", name.c_str());
                    fprintf(output, "\tadd x0, x0, %s@PAGEOFF\n", name.c_str());
                    fprintf(output, "\tbl _puts\n");
                } else {
                    // Print inteiro - usar printf
                    // Em ARM64 macOS, argumentos variádicos são passados na pilha
                    // Alocar espaço na pilha para o argumento
                    fprintf(output, "\tsub sp, sp, #16\n");

                    // Carregar o valor para w8
                    if (isIntLiteral(tac->op1)) {
                        int val = atoi(tac->op1->text.c_str());
                        fprintf(output, "\tmov w8, #%d\n", val);
                    } else if (isCharLiteral(tac->op1)) {
                        if (tac->op1->text.length() >= 3) {
                            char c = tac->op1->text[1];
                            fprintf(output, "\tmov w8, #%d\n", (int)c);
                        } else {
                            fprintf(output, "\tmov w8, #0\n");
                        }
                    } else if (isBoolLiteral(tac->op1)) {
                        fprintf(output, "\tmov w8, #%d\n", tac->op1->text == "true" ? 1 : 0);
                    } else {
                        std::string name = makeAsmName(tac->op1->text);
                        fprintf(output, "\tadrp x9, %s@PAGE\n", name.c_str());
                        fprintf(output, "\tadd x9, x9, %s@PAGEOFF\n", name.c_str());
                        fprintf(output, "\tldr w8, [x9]\n");
                    }

                    // Colocar argumento na pilha (estendido para 64-bit)
                    fprintf(output, "\tstr x8, [sp]\n");

                    // Carregar formato em x0
                    fprintf(output, "\tadrp x0, _printintln@PAGE\n");
                    fprintf(output, "\tadd x0, x0, _printintln@PAGEOFF\n");
                    fprintf(output, "\tbl _printf\n");

                    // Restaurar pilha
                    fprintf(output, "\tadd sp, sp, #16\n");
                }
            }
            break;

        case TAC_READ:
            // read res
            if (tac->res) {
                fprintf(output, "\t// READ %s\n", tac->res->text.c_str());
                std::string name = makeAsmName(tac->res->text);

                // Carregar formato em x0
                fprintf(output, "\tadrp x0, _readint@PAGE\n");
                fprintf(output, "\tadd x0, x0, _readint@PAGEOFF\n");
                // Carregar endereço da variável em x1
                fprintf(output, "\tadrp x1, %s@PAGE\n", name.c_str());
                fprintf(output, "\tadd x1, x1, %s@PAGEOFF\n", name.c_str());
                fprintf(output, "\tbl _scanf\n");
            }
            break;

        case TAC_VEC_ACCESS:
            // res = op1[op2]
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// VEC_ACCESS %s = %s[%s]\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());

                // Carregar índice
                loadToW0(tac->op2, output);
                fprintf(output, "\tsxtw x0, w0\n");  // Sign extend to 64-bit

                // Carregar endereço base do vetor
                std::string vecName = makeAsmName(tac->op1->text);
                fprintf(output, "\tadrp x1, %s@PAGE\n", vecName.c_str());
                fprintf(output, "\tadd x1, x1, %s@PAGEOFF\n", vecName.c_str());

                // Acessar elemento: base + índice * 4
                fprintf(output, "\tldr w0, [x1, x0, lsl #2]\n");
                storeW0To(tac->res, output);
            }
            break;

        case TAC_VEC_WRITE:
            // res[op1] = op2
            if (tac->res && tac->op1 && tac->op2) {
                fprintf(output, "\t// VEC_WRITE %s[%s] = %s\n",
                        tac->res->text.c_str(), tac->op1->text.c_str(), tac->op2->text.c_str());

                // Carregar índice
                loadToW0(tac->op1, output);
                fprintf(output, "\tsxtw x0, w0\n");  // Sign extend to 64-bit

                // Carregar valor
                loadToW1(tac->op2, output);

                // Carregar endereço base do vetor
                std::string vecName = makeAsmName(tac->res->text);
                fprintf(output, "\tadrp x2, %s@PAGE\n", vecName.c_str());
                fprintf(output, "\tadd x2, x2, %s@PAGEOFF\n", vecName.c_str());

                // Escrever elemento: base + índice * 4
                fprintf(output, "\tstr w1, [x2, x0, lsl #2]\n");
            }
            break;

        default:
            fprintf(output, "\t// TAC DESCONHECIDO tipo=%d\n", tac->type);
            break;
    }
}

// Função auxiliar para coletar todos os símbolos usados
static void collectSymbols(TAC* tacList, std::vector<SymbolNode*>& symbols) {
    std::vector<TAC*> tacs;
    TAC* current = tacList;
    while (current) {
        tacs.push_back(current);
        current = current->prev;
    }

    std::vector<std::string> seen;

    for (int i = tacs.size() - 1; i >= 0; i--) {
        TAC* t = tacs[i];

        auto addSymbol = [&](SymbolNode* sym) {
            if (!sym) return;
            if (isFunction(sym)) return;
            if (isLabel(sym)) return;

            for (const auto& s : seen) {
                if (s == sym->text) return;
            }
            seen.push_back(sym->text);
            symbols.push_back(sym);
        };

        addSymbol(t->res);
        addSymbol(t->op1);
        addSymbol(t->op2);
    }
}

// Função principal para gerar código assembly
void generateAsm(TAC* tacList, FILE* output) {
    if (!output) return;

    // Resetar contadores globais
    stringCounter = 0;
    stringNames.clear();

    fprintf(output, "// Código assembly gerado pelo compilador\n");
    fprintf(output, "// Etapa 6 - Compiladores UFRGS 2025/2\n");
    fprintf(output, "// Autor: Santiago Gonzaga\n");
    fprintf(output, "// Arquitetura: ARM64 (Apple Silicon / macOS)\n\n");

    // Gerar seção de dados
    generateDataSection(output);

    // Coletar símbolos usados
    std::vector<SymbolNode*> symbols;
    collectSymbols(tacList, symbols);

    // Declarar variáveis e literais na seção .data
    fprintf(output, "// Declaração de variáveis e literais\n");
    for (SymbolNode* sym : symbols) {
        if (!sym) continue;

        std::string name = makeAsmName(sym->text);

        if (isIntLiteral(sym) || isCharLiteral(sym) || isBoolLiteral(sym)) {
            continue;
        }

        if (isStringLiteral(sym)) {
            fprintf(output, "%s:\n", name.c_str());
            fprintf(output, "\t.asciz %s\n", sym->text.c_str());
        } else {
            fprintf(output, "%s:\n", name.c_str());
            fprintf(output, "\t.long 0\n");
        }
    }

    fprintf(output, "\n");

    // Gerar seção de código
    fprintf(output, "// Seção de código\n");
    fprintf(output, ".text\n\n");

    // Coletar todos os TACs em ordem correta
    std::vector<TAC*> tacs;
    TAC* current = tacList;
    while (current) {
        tacs.push_back(current);
        current = current->prev;
    }

    // Separar TACs de inicialização global (antes de BEGINFUN) e o resto
    std::vector<TAC*> initTacs;
    std::vector<TAC*> funcTacs;
    bool inFunction = false;
    bool foundMain = false;

    for (int i = tacs.size() - 1; i >= 0; i--) {
        TAC* t = tacs[i];
        if (t->type == TAC_BEGINFUN) {
            inFunction = true;
            if (t->res && t->res->text == "main") {
                foundMain = true;
            }
        }
        if (!inFunction) {
            initTacs.push_back(t);
        } else {
            funcTacs.push_back(t);
        }
        if (t->type == TAC_ENDFUN) {
            inFunction = false;
        }
    }

    // Gerar código das funções, inserindo inicializações no início do main
    bool mainPrologueDone = false;
    for (size_t i = 0; i < funcTacs.size(); i++) {
        TAC* t = funcTacs[i];
        generateTacAsm(t, output);

        // Após o prólogo do main, inserir código de inicialização global
        if (t->type == TAC_BEGINFUN && t->res && t->res->text == "main" && !mainPrologueDone) {
            mainPrologueDone = true;
            if (!initTacs.empty()) {
                fprintf(output, "\t// Inicialização de variáveis globais\n");
                for (size_t j = 0; j < initTacs.size(); j++) {
                    generateTacAsm(initTacs[j], output);
                }
            }
        }
    }

    fprintf(output, "\n// Fim do código assembly\n");
}
