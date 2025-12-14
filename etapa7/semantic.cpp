/*
 * Compiladores - etapa4 - semantic.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Implementação da análise semântica
 */

#include "semantic.hpp"
#include <iostream>

using namespace std;

SemanticAnalyzer::SemanticAnalyzer() : errorCount(0) {}

void SemanticAnalyzer::reportError(const string& message) {
    cerr << "ERRO SEMÂNTICO: " << message << endl;
    errorCount++;
}

int SemanticAnalyzer::getDataTypeFromString(const string& typeStr) {
    if (typeStr == "int") return DATATYPE_INT;
    if (typeStr == "char") return DATATYPE_CHAR;
    if (typeStr == "float") return DATATYPE_FLOAT;
    if (typeStr == "bool") return DATATYPE_BOOL;
    return 0;
}

int SemanticAnalyzer::getDataTypeFromLiteral(int literalType) {
    switch (literalType) {
        case SYMBOL_LIT_INT:
        case SYMBOL_LIT_CHAR:
            return DATATYPE_INT; // char e int são compatíveis
        case SYMBOL_LIT_FLOAT:
            return DATATYPE_FLOAT;
        case SYMBOL_LIT_BOOL:
            return DATATYPE_BOOL;
        default:
            return 0;
    }
}

bool SemanticAnalyzer::areTypesCompatible(int type1, int type2) {
    // char e int são compatíveis entre si
    if ((type1 == DATATYPE_INT || type1 == DATATYPE_CHAR) &&
        (type2 == DATATYPE_INT || type2 == DATATYPE_CHAR)) {
        return true;
    }
    // Outros tipos devem ser exatamente iguais
    return type1 == type2;
}

bool SemanticAnalyzer::isArithmeticOperator(int op) {
    return (op == AST_ADD || op == AST_SUB || op == AST_MUL ||
            op == AST_DIV || op == AST_MOD);
}

bool SemanticAnalyzer::isRelationalOperator(int op) {
    return (op == AST_LT || op == AST_GT || op == AST_LE ||
            op == AST_GE || op == AST_EQ || op == AST_DIF);
}

bool SemanticAnalyzer::isLogicalOperator(int op) {
    return (op == AST_AND || op == AST_OR || op == AST_NOT);
}

int SemanticAnalyzer::inferExpressionType(ASTNode* node) {
    if (!node) return 0;

    // Se é um literal, retorna o tipo do literal
    if (node->type == AST_LITERAL && node->symbol) {
        return getDataTypeFromLiteral(node->symbol->type);
    }

    // Se é um identificador, retorna o dataType da tabela de símbolos
    // MAS apenas se não for um vetor (vetores sem indexação são inválidos)
    if (node->type == AST_IDENTIFIER && node->symbol) {
        if (node->symbol->nature == NATURE_VECTOR) {
            // Vetor usado sem indexação é erro - retorna 0 para indicar tipo inválido
            return 0;
        }
        return node->symbol->dataType;
    }

    // Se é acesso a vetor, retorna o tipo do vetor
    if (node->type == AST_VECTOR_ACCESS && node->child[0] && node->child[0]->symbol) {
        return node->child[0]->symbol->dataType;
    }

    // Se é chamada de função, retorna o tipo de retorno da função
    if (node->type == AST_FUNCTION_CALL && node->child[0] && node->child[0]->symbol) {
        return node->child[0]->symbol->dataType;
    }

    // Se é uma expressão binária
    if (node->type == AST_EXPRESSION_BINOP) {
        int op = node->operator_type;

        // Operadores relacionais sempre retornam bool
        if (isRelationalOperator(op)) {
            return DATATYPE_BOOL;
        }

        // Operadores lógicos sempre retornam bool
        if (isLogicalOperator(op)) {
            return DATATYPE_BOOL;
        }

        // Operadores aritméticos: retornam o tipo dos operandos
        // (assumindo que foram verificados e são compatíveis)
        if (isArithmeticOperator(op)) {
            int leftType = inferExpressionType(node->child[0]);
            int rightType = inferExpressionType(node->child[1]);

            // Se algum é float, retorna float
            if (leftType == DATATYPE_FLOAT || rightType == DATATYPE_FLOAT) {
                return DATATYPE_FLOAT;
            }
            // Senão, retorna int (char também vira int)
            return DATATYPE_INT;
        }
    }

    // Se é uma expressão unária (NOT)
    if (node->type == AST_EXPRESSION_UNOP && node->operator_type == AST_NOT) {
        return DATATYPE_BOOL;
    }

    return 0;
}

void SemanticAnalyzer::firstPass(ASTNode* node) {
    if (!node) return;

    // Processa declaração de variável
    if (node->type == AST_VAR_DECLARATION) {
        if (node->child[0] && node->child[1] && node->child[1]->symbol) {
            string typeStr = node->child[0]->symbol->text;
            SymbolNode* varSymbol = node->child[1]->symbol;

            // Verifica redeclaração
            if (varSymbol->nature != 0) {
                reportError("Variável '" + varSymbol->text + "' redeclarada");
            } else {
                varSymbol->nature = NATURE_SCALAR;
                varSymbol->dataType = getDataTypeFromString(typeStr);
            }
        }
    }

    // Processa declaração de vetor
    if (node->type == AST_VECTOR_DECLARATION) {
        if (node->child[0] && node->child[1] && node->child[1]->symbol) {
            string typeStr = node->child[0]->symbol->text;
            SymbolNode* vecSymbol = node->child[1]->symbol;

            // Verifica redeclaração
            if (vecSymbol->nature != 0) {
                reportError("Vetor '" + vecSymbol->text + "' redeclarado");
            } else {
                vecSymbol->nature = NATURE_VECTOR;
                vecSymbol->dataType = getDataTypeFromString(typeStr);
            }

            // Verifica inicialização do vetor se houver
            if (node->child[3]) {
                checkVectorInitialization(node);
            }
        }
    }

    // Processa declaração de função
    if (node->type == AST_FUNCTION_DECLARATION) {
        if (node->child[0] && node->child[1] && node->child[1]->symbol) {
            string typeStr = node->child[0]->symbol->text;
            SymbolNode* funcSymbol = node->child[1]->symbol;

            // Verifica redeclaração
            if (funcSymbol->nature != 0) {
                reportError("Função '" + funcSymbol->text + "' redeclarada");
            } else {
                funcSymbol->nature = NATURE_FUNCTION;
                funcSymbol->dataType = getDataTypeFromString(typeStr);
                funcSymbol->parameterList = node->child[2]; // Salva lista de parâmetros
            }

            // Processa parâmetros
            if (node->child[2]) {
                firstPass(node->child[2]);
            }

            // Processa variáveis locais e corpo
            if (node->child[3]) {
                firstPass(node->child[3]);
            }
        }
        return; // Não precisa processar filhos novamente
    }

    // Processa parâmetro
    if (node->type == AST_PARAMETER) {
        if (node->child[0] && node->child[1] && node->child[1]->symbol) {
            string typeStr = node->child[0]->symbol->text;
            SymbolNode* paramSymbol = node->child[1]->symbol;

            // Verifica redeclaração
            if (paramSymbol->nature != 0) {
                reportError("Parâmetro '" + paramSymbol->text + "' redeclarado");
            } else {
                paramSymbol->nature = NATURE_SCALAR;
                paramSymbol->dataType = getDataTypeFromString(typeStr);
            }
        }
    }

    // Processa recursivamente os filhos
    for (int i = 0; i < 4; i++) {
        firstPass(node->child[i]);
    }
}

void SemanticAnalyzer::checkVectorInitialization(ASTNode* node) {
    if (!node || node->type != AST_VECTOR_DECLARATION) return;
    if (!node->child[2] || !node->child[2]->symbol) return; // Sem tamanho
    if (!node->child[3]) return; // Sem inicialização

    int vectorSize = atoi(node->child[2]->symbol->text.c_str());
    int vectorDataType = getDataTypeFromString(node->child[0]->symbol->text);

    // Conta elementos na lista de inicialização
    int initCount = 0;
    ASTNode* current = node->child[3];
    while (current && current->type == AST_LITERAL_LIST) {
        initCount++;
        // Verifica o literal no child[0] se é o primeiro elemento
        // ou no child[1] se é um elemento seguinte (estrutura recursiva)
        ASTNode* literal = nullptr;
        if (current->child[0] && current->child[0]->type == AST_LITERAL) {
            literal = current->child[0];
        } else if (current->child[1] && current->child[1]->type == AST_LITERAL) {
            literal = current->child[1];
        }

        if (literal && literal->symbol) {
            // Verifica tipo do literal
            int litType = getDataTypeFromLiteral(literal->symbol->type);
            if (!areTypesCompatible(vectorDataType, litType)) {
                reportError("Tipo incompatível na inicialização do vetor '" +
                           node->child[1]->symbol->text + "'");
            }
        }

        // Avança para o próximo na lista
        if (current->child[0] && current->child[0]->type == AST_LITERAL_LIST) {
            current = current->child[0];
        } else {
            break;
        }
    }

    // Verifica se o número de inicializadores é compatível com o tamanho
    if (initCount > vectorSize) {
        reportError("Número de inicializadores maior que o tamanho do vetor '" +
                   node->child[1]->symbol->text + "'");
    }
}

void SemanticAnalyzer::secondPass(ASTNode* node, int functionReturnType) {
    if (!node) return;

    // Processa comandos verificando tipos
    switch (node->type) {
        case AST_ASSIGNMENT:
            checkAssignment(node);
            break;

        case AST_VECTOR_ASSIGNMENT:
            // Verifica se é um vetor
            if (node->child[0] && node->child[0]->symbol) {
                SymbolNode* sym = node->child[0]->symbol;
                if (sym->nature == 0) {
                    reportError("Identificador '" + sym->text + "' não declarado");
                } else if (sym->nature != NATURE_VECTOR) {
                    reportError("Identificador '" + sym->text + "' não é um vetor");
                }
                // Verifica índice
                if (node->child[1]) {
                    checkVectorAccess(node);
                }
                // Verifica tipo da expressão
                if (node->child[2]) {
                    int exprType = inferExpressionType(node->child[2]);
                    if (exprType != 0 && sym->dataType != 0 &&
                        !areTypesCompatible(sym->dataType, exprType)) {
                        reportError("Tipo incompatível em atribuição a '" + sym->text + "'");
                    }
                    secondPass(node->child[2], functionReturnType);
                }
            }
            break;

        case AST_READ:
            // Verifica se o identificador existe e é escalar
            if (node->child[0] && node->child[0]->symbol) {
                SymbolNode* sym = node->child[0]->symbol;
                if (sym->nature == 0) {
                    reportError("Identificador '" + sym->text + "' não declarado");
                } else if (sym->nature != NATURE_SCALAR) {
                    reportError("Read requer variável escalar, '" + sym->text +
                              "' não é escalar");
                }
            }
            break;

        case AST_RETURN:
            if (node->child[0]) {
                int exprType = inferExpressionType(node->child[0]);
                if (exprType != 0 && functionReturnType != 0 &&
                    !areTypesCompatible(functionReturnType, exprType)) {
                    reportError("Tipo de retorno incompatível");
                }
                secondPass(node->child[0], functionReturnType);
            }
            break;

        case AST_VECTOR_ACCESS:
            checkVectorAccess(node);
            break;

        case AST_FUNCTION_CALL:
            checkFunctionCall(node);
            break;

        case AST_EXPRESSION_BINOP:
        case AST_EXPRESSION_UNOP:
            checkExpression(node);
            break;

        case AST_IDENTIFIER:
            // Verifica se está declarado
            if (node->symbol) {
                if (node->symbol->nature == 0) {
                    reportError("Identificador '" + node->symbol->text + "' não declarado");
                } else if (node->symbol->nature == NATURE_FUNCTION) {
                    reportError("Função '" + node->symbol->text +
                              "' usada sem chamada (faltam parênteses)");
                } else if (node->symbol->nature == NATURE_VECTOR) {
                    reportError("Vetor '" + node->symbol->text + "' usado sem indexação");
                }
            }
            break;

        case AST_VAR_DECLARATION:
        case AST_VECTOR_DECLARATION:
            // Não processa declarações na segunda passagem
            // (já foram processadas na primeira passagem)
            return;

        case AST_FUNCTION_DECLARATION:
            // Processa corpo da função com tipo de retorno
            if (node->child[0] && node->child[3]) {
                int retType = getDataTypeFromString(node->child[0]->symbol->text);
                secondPass(node->child[3], retType);
            }
            return; // Não processar filhos novamente

        default:
            // Processa recursivamente os filhos
            for (int i = 0; i < 4; i++) {
                secondPass(node->child[i], functionReturnType);
            }
            break;
    }
}

void SemanticAnalyzer::checkAssignment(ASTNode* node) {
    if (!node || node->type != AST_ASSIGNMENT) return;
    if (!node->child[0] || !node->child[0]->symbol) return;

    SymbolNode* sym = node->child[0]->symbol;

    // Verifica se está declarado
    if (sym->nature == 0) {
        reportError("Identificador '" + sym->text + "' não declarado");
        return;
    }

    // Verifica se é escalar
    if (sym->nature != NATURE_SCALAR) {
        if (sym->nature == NATURE_VECTOR) {
            reportError("Vetor '" + sym->text + "' usado sem indexação");
        } else if (sym->nature == NATURE_FUNCTION) {
            reportError("Função '" + sym->text + "' não pode ser usada como variável");
        }
        return;
    }

    // Verifica tipo da expressão
    if (node->child[1]) {
        int exprType = inferExpressionType(node->child[1]);
        if (exprType != 0 && sym->dataType != 0 &&
            !areTypesCompatible(sym->dataType, exprType)) {
            reportError("Tipo incompatível em atribuição a '" + sym->text + "'");
        }
        secondPass(node->child[1], 0);
    }
}

void SemanticAnalyzer::checkVectorAccess(ASTNode* node) {
    if (!node) return;

    ASTNode* vecNode = node->type == AST_VECTOR_ACCESS ? node->child[0] : node->child[0];
    ASTNode* indexNode = node->type == AST_VECTOR_ACCESS ? node->child[1] : node->child[1];

    if (!vecNode || !vecNode->symbol || !indexNode) return;

    SymbolNode* sym = vecNode->symbol;

    // Verifica se está declarado
    if (sym->nature == 0) {
        reportError("Identificador '" + sym->text + "' não declarado");
        return;
    }

    // Verifica se é vetor
    if (sym->nature != NATURE_VECTOR) {
        if (sym->nature == NATURE_SCALAR) {
            reportError("Variável escalar '" + sym->text + "' usada com indexação");
        } else if (sym->nature == NATURE_FUNCTION) {
            reportError("Função '" + sym->text + "' não pode ser indexada");
        }
        return;
    }

    // Verifica tipo do índice (deve ser inteiro)
    int indexType = inferExpressionType(indexNode);
    if (indexType != 0 && indexType != DATATYPE_INT && indexType != DATATYPE_CHAR) {
        reportError("Índice de vetor deve ser inteiro");
    }

    secondPass(indexNode, 0);
}

void SemanticAnalyzer::checkExpression(ASTNode* node) {
    if (!node) return;

    if (node->type == AST_EXPRESSION_BINOP) {
        int op = node->operator_type;

        // Verifica operandos
        if (node->child[0]) secondPass(node->child[0], 0);
        if (node->child[1]) secondPass(node->child[1], 0);

        int leftType = inferExpressionType(node->child[0]);
        int rightType = inferExpressionType(node->child[1]);

        // Operadores aritméticos: operandos devem ser numéricos compatíveis
        if (isArithmeticOperator(op)) {
            if (leftType == DATATYPE_BOOL || rightType == DATATYPE_BOOL) {
                reportError("Operador aritmético não pode ser aplicado a booleanos");
            }
            if (leftType != 0 && rightType != 0 && !areTypesCompatible(leftType, rightType)) {
                reportError("Operandos de tipos incompatíveis em operação aritmética");
            }
        }

        // Operadores relacionais: operandos devem ser compatíveis
        if (isRelationalOperator(op)) {
            if (leftType != 0 && rightType != 0 && !areTypesCompatible(leftType, rightType)) {
                reportError("Operandos de tipos incompatíveis em operação relacional");
            }
        }

        // Operadores lógicos: operandos devem ser booleanos
        if (isLogicalOperator(op)) {
            if (leftType != 0 && leftType != DATATYPE_BOOL) {
                reportError("Operador lógico requer operandos booleanos");
            }
            if (rightType != 0 && rightType != DATATYPE_BOOL) {
                reportError("Operador lógico requer operandos booleanos");
            }
        }
    } else if (node->type == AST_EXPRESSION_UNOP && node->operator_type == AST_NOT) {
        if (node->child[0]) secondPass(node->child[0], 0);

        int operandType = inferExpressionType(node->child[0]);
        if (operandType != 0 && operandType != DATATYPE_BOOL) {
            reportError("Operador NOT requer operando booleano");
        }
    }
}

int SemanticAnalyzer::countParameters(ASTNode* paramList) {
    if (!paramList) return 0;
    int count = 0;
    ASTNode* current = paramList;
    while (current && current->type == AST_PARAMETER_LIST) {
        count++;
        current = current->child[0];
    }
    return count;
}

int SemanticAnalyzer::countArguments(ASTNode* argList) {
    if (!argList) return 0;
    int count = 0;
    ASTNode* current = argList;
    while (current && current->type == AST_ARGUMENT_LIST) {
        count++;
        current = current->child[0];
    }
    return count;
}

bool SemanticAnalyzer::checkParameterArgumentCompatibility(ASTNode* params, ASTNode* args) {
    if (!params && !args) return true;
    if (!params || !args) return false;

    ASTNode* currentParam = params;
    ASTNode* currentArg = args;

    while (currentParam && currentParam->type == AST_PARAMETER_LIST &&
           currentArg && currentArg->type == AST_ARGUMENT_LIST) {

        // Determine where the actual parameter and argument are
        // If child[1] exists and is not null, it's there (multiple params case)
        // Otherwise, it's in child[0] (single param case)
        ASTNode* param = currentParam->child[1] ? currentParam->child[1] : currentParam->child[0];
        ASTNode* arg = currentArg->child[1] ? currentArg->child[1] : currentArg->child[0];

        if (param && param->type == AST_PARAMETER) {
            int paramType = getDataTypeFromString(param->child[0]->symbol->text);
            int argType = inferExpressionType(arg);

            // Verifica compatibilidade
            if (paramType != 0 && argType != 0 && !areTypesCompatible(paramType, argType)) {
                return false;
            }
        }

        currentParam = currentParam->child[0];
        currentArg = currentArg->child[0];
    }

    return true;
}

void SemanticAnalyzer::checkFunctionCall(ASTNode* node) {
    if (!node || node->type != AST_FUNCTION_CALL) return;
    if (!node->child[0] || !node->child[0]->symbol) return;

    SymbolNode* funcSym = node->child[0]->symbol;

    // Verifica se está declarado
    if (funcSym->nature == 0) {
        reportError("Função '" + funcSym->text + "' não declarada");
        return;
    }

    // Verifica se é função
    if (funcSym->nature != NATURE_FUNCTION) {
        if (funcSym->nature == NATURE_SCALAR) {
            reportError("Variável '" + funcSym->text + "' usada como função");
        } else if (funcSym->nature == NATURE_VECTOR) {
            reportError("Vetor '" + funcSym->text + "' usado como função");
        }
        return;
    }

    // Verifica número e tipos de argumentos
    int paramCount = countParameters(funcSym->parameterList);
    int argCount = countArguments(node->child[1]);

    if (paramCount != argCount) {
        reportError("Número incorreto de argumentos na chamada de '" + funcSym->text + "'");
    } else if (!checkParameterArgumentCompatibility(funcSym->parameterList, node->child[1])) {
        reportError("Tipos de argumentos incompatíveis na chamada de '" + funcSym->text + "'");
    }

    // Processa os argumentos
    if (node->child[1]) {
        secondPass(node->child[1], 0);
    }
}

bool SemanticAnalyzer::analyze(ASTNode* root) {
    errorCount = 0;

    cout << "\n========== ANÁLISE SEMÂNTICA ==========" << endl;

    // Primeira passagem: registra declarações
    cout << "Primeira passagem: registrando declarações..." << endl;
    firstPass(root);

    // Segunda passagem: verifica usos e tipos
    cout << "Segunda passagem: verificando usos e tipos..." << endl;
    secondPass(root, 0);

    if (errorCount == 0) {
        cout << "Análise semântica concluída: SEM ERROS" << endl;
    } else {
        cout << "Análise semântica concluída: " << errorCount << " ERRO(S) encontrado(s)" << endl;
    }
    cout << "========================================\n" << endl;

    return (errorCount == 0);
}
