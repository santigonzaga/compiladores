/*
 * Compiladores - etapa3 - ast.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Implementação da Árvore Sintática Abstrata (AST)
 */

#include "ast.hpp"
#include <iostream>
#include <vector>

// Função para criar um nodo da AST
ASTNode *createNode(int type, SymbolNode *symbol, ASTNode *c0, ASTNode *c1,
                    ASTNode *c2, ASTNode *c3) {
  ASTNode *node = new ASTNode();
  node->type = type;
  node->symbol = symbol;
  node->child[0] = c0;
  node->child[1] = c1;
  node->child[2] = c2;
  node->child[3] = c3;
  return node;
}

// Função para criar um nodo de operador
ASTNode *createOperatorNode(int operator_type, ASTNode *c0, ASTNode *c1) {
  ASTNode *node = new ASTNode();
  node->type = AST_EXPRESSION_BINOP;
  node->operator_type = operator_type;
  node->child[0] = c0;
  node->child[1] = c1;
  return node;
}

// Função para criar um nodo folha
ASTNode *createLeafNode(int type, SymbolNode *symbol) {
  ASTNode *node = new ASTNode();
  node->type = type;
  node->symbol = symbol;
  return node;
}

// Função para obter nome do tipo de nodo
std::string getNodeTypeName(int type) {
  switch (type) {
  case AST_PROGRAM:
    return "PROGRAM";
  case AST_DECLARATION_LIST:
    return "DECLARATION_LIST";
  case AST_VAR_DECLARATION:
    return "VAR_DECLARATION";
  case AST_VECTOR_DECLARATION:
    return "VECTOR_DECLARATION";
  case AST_FUNCTION_DECLARATION:
    return "FUNCTION_DECLARATION";
  case AST_PARAMETER_LIST:
    return "PARAMETER_LIST";
  case AST_PARAMETER:
    return "PARAMETER";
  case AST_BLOCK:
    return "BLOCK";
  case AST_COMMAND_LIST:
    return "COMMAND_LIST";
  case AST_ASSIGNMENT:
    return "ASSIGNMENT";
  case AST_VECTOR_ASSIGNMENT:
    return "VECTOR_ASSIGNMENT";
  case AST_READ:
    return "READ";
  case AST_PRINT:
    return "PRINT";
  case AST_RETURN:
    return "RETURN";
  case AST_IF:
    return "IF";
  case AST_IF_ELSE:
    return "IF_ELSE";
  case AST_WHILE:
    return "WHILE";
  case AST_EXPRESSION_BINOP:
    return "EXPRESSION_BINOP";
  case AST_EXPRESSION_UNOP:
    return "EXPRESSION_UNOP";
  case AST_FUNCTION_CALL:
    return "FUNCTION_CALL";
  case AST_VECTOR_ACCESS:
    return "VECTOR_ACCESS";
  case AST_IDENTIFIER:
    return "IDENTIFIER";
  case AST_LITERAL:
    return "LITERAL";
  case AST_LITERAL_LIST:
    return "LITERAL_LIST";
  case AST_ARGUMENT_LIST:
    return "ARGUMENT_LIST";
  case AST_PRINT_LIST:
    return "PRINT_LIST";
  default:
    return "UNKNOWN";
  }
}

// Função para obter nome do operador
std::string getOperatorName(int op) {
  switch (op) {
  case AST_ADD:
    return "+";
  case AST_SUB:
    return "-";
  case AST_MUL:
    return "*";
  case AST_DIV:
    return "/";
  case AST_MOD:
    return "%";
  case AST_LT:
    return "<";
  case AST_GT:
    return ">";
  case AST_LE:
    return "<=";
  case AST_GE:
    return ">=";
  case AST_EQ:
    return "==";
  case AST_DIF:
    return "!=";
  case AST_AND:
    return "&";
  case AST_OR:
    return "|";
  case AST_NOT:
    return "~";
  default:
    return "?";
  }
}

// Função para imprimir um nodo da AST
void printNode(ASTNode *node, int level) {
  if (!node)
    return;

  // Indentação baseada no nível
  for (int i = 0; i < level; i++) {
    std::cout << "  ";
  }

  // Imprime informações do nodo
  std::cout << getNodeTypeName(node->type);

  if (node->type == AST_EXPRESSION_BINOP) {
    std::cout << " (" << getOperatorName(node->operator_type) << ")";
  } else if (node->symbol) {
    std::cout << " (" << node->symbol->text << ")";
  }

  std::cout << std::endl;

  // Imprime filhos recursivamente
  for (int i = 0; i < 4; i++) {
    if (node->child[i]) {
      printNode(node->child[i], level + 1);
    }
  }
}

// Função para imprimir a AST completa
void printAST(ASTNode *root) {
  std::cout << "\n========== ÁRVORE SINTÁTICA ABSTRATA ==========" << std::endl;
  if (root) {
    printNode(root, 0);
  } else {
    std::cout << "Árvore vazia" << std::endl;
  }
  std::cout << "===============================================" << std::endl
            << std::endl;
}

// Função para gerar código fonte a partir da AST
void generateCode(ASTNode *node, FILE *output) {
  if (!node || !output)
    return;

  switch (node->type) {
  case AST_PROGRAM:
    if (node->child[0]) {
      generateCode(node->child[0], output);
    }
    break;

  case AST_DECLARATION_LIST:
    if (node->child[0]) {
      generateCode(node->child[0], output);
    }
    if (node->child[1]) {
      generateCode(node->child[1], output);
    }
    break;

  case AST_VAR_DECLARATION:
    // type identifier = literal ;
    if (node->child[0] && node->child[1] && node->child[2]) {
      generateCode(node->child[0], output); // type
      fprintf(output, " ");
      generateCode(node->child[1], output); // identifier
      fprintf(output, " = ");
      generateCode(node->child[2], output); // literal
      fprintf(output, ";\n");
    }
    break;

  case AST_VECTOR_DECLARATION:
    // type identifier [ size ] = literal_list ;
    if (node->child[0] && node->child[1] && node->child[2]) {
      generateCode(node->child[0], output); // type
      fprintf(output, " ");
      generateCode(node->child[1], output); // identifier
      fprintf(output, "[");
      generateCode(node->child[2], output); // size
      fprintf(output, "]");
      if (node->child[3]) {
        fprintf(output, " = ");
        generateCode(node->child[3], output); // literal_list
      }
      fprintf(output, ";\n");
    }
    break;

  case AST_FUNCTION_DECLARATION:
    // type identifier ( parameters ) locals block
    if (node->child[0] && node->child[1] && node->child[3]) {
      generateCode(node->child[0], output); // type
      fprintf(output, " ");
      generateCode(node->child[1], output); // identifier
      fprintf(output, "(");
      if (node->child[2]) {
        generateCode(node->child[2], output); // parameters
      }
      fprintf(output, ")\n");
      if (node->child[3]) {
        generateCode(node->child[3], output); // locals + block
      }
    }
    break;

  case AST_PARAMETER_LIST:
    if (node->child[0]) {
      generateCode(node->child[0], output);
    }
    if (node->child[1]) {
      fprintf(output, ", ");
      generateCode(node->child[1], output);
    }
    break;

  case AST_PARAMETER:
    if (node->child[0] && node->child[1]) {
      generateCode(node->child[0], output); // type
      fprintf(output, " ");
      generateCode(node->child[1], output); // identifier
    }
    break;

  case AST_BLOCK:
    fprintf(output, "{\n");
    if (node->child[0]) {
      generateCode(node->child[0], output); // command_list
    }
    fprintf(output, "}\n");
    break;

  case AST_COMMAND_LIST:
    if (node->child[0]) {
      generateCode(node->child[0], output);
    }
    if (node->child[1]) {
      generateCode(node->child[1], output);
    }
    break;

  case AST_ASSIGNMENT:
    if (node->child[0] && node->child[1]) {
      generateCode(node->child[0], output); // identifier
      fprintf(output, " = ");
      generateCode(node->child[1], output); // expression
      fprintf(output, ";\n");
    }
    break;

  case AST_VECTOR_ASSIGNMENT:
    if (node->child[0] && node->child[1] && node->child[2]) {
      generateCode(node->child[0], output); // identifier
      fprintf(output, "[");
      generateCode(node->child[1], output); // index
      fprintf(output, "] = ");
      generateCode(node->child[2], output); // expression
      fprintf(output, ";\n");
    }
    break;

  case AST_READ:
    fprintf(output, "read ");
    if (node->child[0]) {
      generateCode(node->child[0], output); // identifier
    }
    fprintf(output, ";\n");
    break;

  case AST_PRINT:
    fprintf(output, "print ");
    if (node->child[0]) {
      generateCode(node->child[0], output); // print_list
    }
    fprintf(output, ";\n");
    break;

  case AST_PRINT_LIST:
    if (node->child[0]) {
      generateCode(node->child[0], output);
    }
    if (node->child[1]) {
      fprintf(output, " ");
      generateCode(node->child[1], output);
    }
    break;

  case AST_RETURN:
    fprintf(output, "return ");
    if (node->child[0]) {
      generateCode(node->child[0], output); // expression
    }
    fprintf(output, ";\n");
    break;

  case AST_IF:
    fprintf(output, "if (");
    if (node->child[0]) {
      generateCode(node->child[0], output); // expression
    }
    fprintf(output, ")\n");
    if (node->child[1]) {
      generateCode(node->child[1], output); // command
    }
    break;

  case AST_IF_ELSE:
    fprintf(output, "if (");
    if (node->child[0]) {
      generateCode(node->child[0], output); // expression
    }
    fprintf(output, ")\n");
    if (node->child[1]) {
      generateCode(node->child[1], output); // command
    }
    fprintf(output, "else\n");
    if (node->child[2]) {
      generateCode(node->child[2], output); // else command
    }
    break;

  case AST_WHILE:
    fprintf(output, "while (");
    if (node->child[0]) {
      generateCode(node->child[0], output); // expression
    }
    fprintf(output, ")\n");
    if (node->child[1]) {
      generateCode(node->child[1], output); // command
    }
    break;

  case AST_EXPRESSION_BINOP:
    if (node->child[0] && node->child[1]) {
      generateCode(node->child[0], output);
      fprintf(output, " %s ", getOperatorName(node->operator_type).c_str());
      generateCode(node->child[1], output);
    }
    break;

  case AST_EXPRESSION_UNOP:
    fprintf(output, "~");
    if (node->child[0]) {
      generateCode(node->child[0], output);
    }
    break;

  case AST_FUNCTION_CALL:
    if (node->child[0]) {
      generateCode(node->child[0], output); // identifier
    }
    fprintf(output, "(");
    if (node->child[1]) {
      generateCode(node->child[1], output); // arguments
    }
    fprintf(output, ")");
    break;

  case AST_VECTOR_ACCESS:
    if (node->child[0] && node->child[1]) {
      generateCode(node->child[0], output); // identifier
      fprintf(output, "[");
      generateCode(node->child[1], output); // index
      fprintf(output, "]");
    }
    break;

  case AST_ARGUMENT_LIST:
    if (node->child[0]) {
      generateCode(node->child[0], output);
    }
    if (node->child[1]) {
      fprintf(output, ", ");
      generateCode(node->child[1], output);
    }
    break;

  case AST_LITERAL_LIST:
    if (node->child[0]) {
      generateCode(node->child[0], output);
    }
    if (node->child[1]) {
      fprintf(output, " ");
      generateCode(node->child[1], output);
    }
    break;

  case AST_IDENTIFIER:
  case AST_LITERAL:
    if (node->symbol) {
      if (node->symbol->text == "char") {
        fprintf(output, "char");
      } else if (node->symbol->text == "int") {
        fprintf(output, "int");
      } else if (node->symbol->text == "float") {
        fprintf(output, "float");
      } else if (node->symbol->text == "bool") {
        fprintf(output, "bool");
      } else {
        fprintf(output, "%s", node->symbol->text.c_str());
      }
    }
    break;

  default:
    break;
  }
}

// Função para liberar memória da AST
void freeAST(ASTNode *node) {
  if (!node)
    return;

  for (int i = 0; i < 4; i++) {
    if (node->child[i]) {
      freeAST(node->child[i]);
    }
  }

  delete node;
}

// ==================== GERAÇÃO DE CÓDIGO TAC ====================

#include "tac.hpp"

// Funções auxiliares para geração de TAC

// Gerar código para operadores binários
TAC* generateBinOp(ASTNode* node) {
  if (!node) return nullptr;

  TAC* code0 = generateTAC(node->child[0]);
  TAC* code1 = generateTAC(node->child[1]);

  // Mapear tipo de operador AST para tipo TAC
  int tacType;
  switch(node->operator_type) {
    case AST_ADD: tacType = TAC_ADD; break;
    case AST_SUB: tacType = TAC_SUB; break;
    case AST_MUL: tacType = TAC_MUL; break;
    case AST_DIV: tacType = TAC_DIV; break;
    case AST_MOD: tacType = TAC_MOD; break;
    case AST_LT:  tacType = TAC_LT;  break;
    case AST_GT:  tacType = TAC_GT;  break;
    case AST_LE:  tacType = TAC_LE;  break;
    case AST_GE:  tacType = TAC_GE;  break;
    case AST_EQ:  tacType = TAC_EQ;  break;
    case AST_DIF: tacType = TAC_DIF; break;
    case AST_AND: tacType = TAC_AND; break;
    case AST_OR:  tacType = TAC_OR;  break;
    default:      tacType = TAC_MOVE; break;
  }

  SymbolNode* result = makeTemp();
  TAC* newTac = tacCreate(tacType, result,
                          code0 ? code0->res : nullptr,
                          code1 ? code1->res : nullptr);

  return tacJoin(tacJoin(code0, code1), newTac);
}

// Gerar código para operadores unários
TAC* generateUnOp(ASTNode* node) {
  if (!node) return nullptr;

  TAC* code0 = generateTAC(node->child[0]);

  int tacType = (node->operator_type == AST_NOT) ? TAC_NOT : TAC_NEG;
  SymbolNode* result = makeTemp();
  TAC* newTac = tacCreate(tacType, result, code0 ? code0->res : nullptr, nullptr);

  return tacJoin(code0, newTac);
}

// Gerar código para IF
TAC* generateIf(ASTNode* node) {
  if (!node) return nullptr;

  TAC* codeCondition = generateTAC(node->child[0]);  // Condição
  TAC* codeThen = generateTAC(node->child[1]);       // Bloco then

  SymbolNode* labelElse = makeLabel();
  TAC* tacIfz = tacCreate(TAC_IFZ, labelElse,
                          codeCondition ? codeCondition->res : nullptr, nullptr);
  TAC* tacLabel = tacCreate(TAC_LABEL, labelElse, nullptr, nullptr);

  // Juntar: condição -> ifz -> then -> label
  return tacJoin(tacJoin(tacJoin(codeCondition, tacIfz), codeThen), tacLabel);
}

// Gerar código para IF-ELSE
TAC* generateIfElse(ASTNode* node) {
  if (!node) return nullptr;

  TAC* codeCondition = generateTAC(node->child[0]);  // Condição
  TAC* codeThen = generateTAC(node->child[1]);       // Bloco then
  TAC* codeElse = generateTAC(node->child[2]);       // Bloco else

  SymbolNode* labelElse = makeLabel();
  SymbolNode* labelEnd = makeLabel();

  TAC* tacIfz = tacCreate(TAC_IFZ, labelElse,
                          codeCondition ? codeCondition->res : nullptr, nullptr);
  TAC* tacJumpEnd = tacCreate(TAC_JUMP, labelEnd, nullptr, nullptr);
  TAC* tacLabelElse = tacCreate(TAC_LABEL, labelElse, nullptr, nullptr);
  TAC* tacLabelEnd = tacCreate(TAC_LABEL, labelEnd, nullptr, nullptr);

  // Juntar: condição -> ifz -> then -> jump -> labelElse -> else -> labelEnd
  return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(
         codeCondition, tacIfz), codeThen), tacJumpEnd),
         tacLabelElse), codeElse), tacLabelEnd);
}

// Gerar código para WHILE
TAC* generateWhile(ASTNode* node) {
  if (!node) return nullptr;

  TAC* codeCondition = generateTAC(node->child[0]);  // Condição
  TAC* codeBody = generateTAC(node->child[1]);       // Corpo do loop

  SymbolNode* labelLoop = makeLabel();
  SymbolNode* labelEnd = makeLabel();

  TAC* tacLabelLoop = tacCreate(TAC_LABEL, labelLoop, nullptr, nullptr);
  TAC* tacIfz = tacCreate(TAC_IFZ, labelEnd,
                          codeCondition ? codeCondition->res : nullptr, nullptr);
  TAC* tacJumpLoop = tacCreate(TAC_JUMP, labelLoop, nullptr, nullptr);
  TAC* tacLabelEnd = tacCreate(TAC_LABEL, labelEnd, nullptr, nullptr);

  // Juntar: labelLoop -> condição -> ifz -> body -> jump -> labelEnd
  return tacJoin(tacJoin(tacJoin(tacJoin(tacJoin(
         tacLabelLoop, codeCondition), tacIfz), codeBody),
         tacJumpLoop), tacLabelEnd);
}

// Gerar código para função
TAC* generateFunction(ASTNode* node) {
  if (!node) return nullptr;

  // Estrutura da função:
  // child[0] = tipo de retorno (não gera código)
  // child[1] = identificador da função (AST_IDENTIFIER)
  // child[2] = lista de parâmetros
  // child[3] = DECLARATION_LIST (variáveis locais e bloco)

  SymbolNode* funcSymbol = node->child[1] ? node->child[1]->symbol : nullptr;
  if (!funcSymbol) return nullptr;

  TAC* codeParams = generateTAC(node->child[2]);  // Parâmetros
  TAC* codeBody = generateTAC(node->child[3]);    // Declarações locais e corpo

  TAC* tacBegin = tacCreate(TAC_BEGINFUN, funcSymbol, nullptr, nullptr);
  TAC* tacEnd = tacCreate(TAC_ENDFUN, funcSymbol, nullptr, nullptr);

  // Juntar: begin -> params -> body -> end
  return tacJoin(tacJoin(tacJoin(tacBegin, codeParams), codeBody), tacEnd);
}

// Gerar código para chamada de função
TAC* generateFunctionCall(ASTNode* node) {
  if (!node) return nullptr;

  // Estrutura: child[0] = IDENTIFIER, child[1] = argument_list
  SymbolNode* funcSymbol = node->child[0] ? node->child[0]->symbol : nullptr;
  if (!funcSymbol) return nullptr;

  // Percorrer argumentos e criar TAC_ARG para cada um
  TAC* argTacs = nullptr;
  ASTNode* argList = node->child[1];
  while (argList) {
    if (argList->type == AST_ARGUMENT_LIST) {
      TAC* argCode = generateTAC(argList->child[0]);
      if (argCode) {
        TAC* argTac = tacCreate(TAC_ARG, argCode->res, nullptr, nullptr);
        argTacs = tacJoin(tacJoin(argTacs, argCode), argTac);
      }
      argList = argList->child[1];
    } else {
      TAC* argCode = generateTAC(argList);
      if (argCode) {
        TAC* argTac = tacCreate(TAC_ARG, argCode->res, nullptr, nullptr);
        argTacs = tacJoin(tacJoin(argTacs, argCode), argTac);
      }
      break;
    }
  }

  SymbolNode* result = makeTemp();
  TAC* tacCall = tacCreate(TAC_CALL, result, funcSymbol, nullptr);

  return tacJoin(argTacs, tacCall);
}

// Gerar código para atribuição
TAC* generateAssignment(ASTNode* node) {
  if (!node) return nullptr;

  // Estrutura: child[0] = IDENTIFIER, child[1] = expression
  SymbolNode* target = node->child[0] ? node->child[0]->symbol : nullptr;
  if (!target) return nullptr;

  TAC* codeExpr = generateTAC(node->child[1]);  // Expressão do lado direito

  TAC* tacMove = tacCreate(TAC_MOVE, target,
                           codeExpr ? codeExpr->res : nullptr, nullptr);

  return tacJoin(codeExpr, tacMove);
}

// Gerar código para atribuição em vetor
TAC* generateVectorAssignment(ASTNode* node) {
  if (!node) return nullptr;

  // Estrutura: child[0] = IDENTIFIER, child[1] = index_expression, child[2] = value_expression
  SymbolNode* vecSymbol = node->child[0] ? node->child[0]->symbol : nullptr;
  if (!vecSymbol) return nullptr;

  TAC* codeIndex = generateTAC(node->child[1]);  // Índice
  TAC* codeExpr = generateTAC(node->child[2]);   // Expressão

  TAC* tacVecWrite = tacCreate(TAC_VEC_WRITE, vecSymbol,
                               codeIndex ? codeIndex->res : nullptr,
                               codeExpr ? codeExpr->res : nullptr);

  return tacJoin(tacJoin(codeIndex, codeExpr), tacVecWrite);
}

// Gerar código para acesso a vetor
TAC* generateVectorAccess(ASTNode* node) {
  if (!node) return nullptr;

  // Estrutura: child[0] = IDENTIFIER, child[1] = index_expression
  SymbolNode* vecSymbol = node->child[0] ? node->child[0]->symbol : nullptr;
  if (!vecSymbol) return nullptr;

  TAC* codeIndex = generateTAC(node->child[1]);  // Índice

  SymbolNode* result = makeTemp();
  TAC* tacVecAccess = tacCreate(TAC_VEC_ACCESS, result, vecSymbol,
                                codeIndex ? codeIndex->res : nullptr);

  return tacJoin(codeIndex, tacVecAccess);
}

// Função auxiliar para coletar todos os itens de uma PRINT_LIST recursivamente
void collectPrintItems(ASTNode* node, std::vector<ASTNode*>& items) {
  if (!node) return;

  if (node->type == AST_PRINT_LIST) {
    // Processar recursivamente: primeiro child[0], depois child[1]
    collectPrintItems(node->child[0], items);
    collectPrintItems(node->child[1], items);
  } else {
    // É um item a ser impresso (não é uma lista)
    items.push_back(node);
  }
}

// Gerar código para PRINT
TAC* generatePrint(ASTNode* node) {
  if (!node) return nullptr;

  // Coletar todos os itens a serem impressos
  std::vector<ASTNode*> items;
  collectPrintItems(node->child[0], items);

  // Gerar código para cada item
  TAC* result = nullptr;
  for (ASTNode* item : items) {
    TAC* codeExpr = generateTAC(item);
    if (codeExpr) {
      TAC* tacPrint = tacCreate(TAC_PRINT, nullptr, codeExpr->res, nullptr);
      result = tacJoin(tacJoin(result, codeExpr), tacPrint);
    }
  }

  return result;
}

// Gerar código para READ
TAC* generateRead(ASTNode* node) {
  if (!node) return nullptr;

  // Estrutura: child[0] = IDENTIFIER
  SymbolNode* target = node->child[0] ? node->child[0]->symbol : nullptr;
  if (!target) return nullptr;

  TAC* tacRead = tacCreate(TAC_READ, target, nullptr, nullptr);
  return tacRead;
}

// Gerar código para RETURN
TAC* generateReturn(ASTNode* node) {
  if (!node) return nullptr;

  TAC* codeExpr = generateTAC(node->child[0]);  // Expressão de retorno

  TAC* tacRet = tacCreate(TAC_RET, nullptr,
                          codeExpr ? codeExpr->res : nullptr, nullptr);

  return tacJoin(codeExpr, tacRet);
}

// Função principal de geração de código TAC
TAC* generateTAC(ASTNode* node) {
  if (!node) return nullptr;

  TAC* code0 = nullptr;
  TAC* code1 = nullptr;
  TAC* code2 = nullptr;
  TAC* code3 = nullptr;
  TAC* result = nullptr;

  switch(node->type) {
    case AST_IDENTIFIER:
    case AST_LITERAL:
      // Criar TAC_SYMBOL para facilitar a recursão
      result = tacCreate(TAC_SYMBOL, node->symbol, nullptr, nullptr);
      break;

    case AST_EXPRESSION_BINOP:
      result = generateBinOp(node);
      break;

    case AST_EXPRESSION_UNOP:
      result = generateUnOp(node);
      break;

    case AST_ASSIGNMENT:
      result = generateAssignment(node);
      break;

    case AST_VECTOR_ASSIGNMENT:
      result = generateVectorAssignment(node);
      break;

    case AST_VECTOR_ACCESS:
      result = generateVectorAccess(node);
      break;

    case AST_FUNCTION_CALL:
      result = generateFunctionCall(node);
      break;

    case AST_IF:
      result = generateIf(node);
      break;

    case AST_IF_ELSE:
      result = generateIfElse(node);
      break;

    case AST_WHILE:
      result = generateWhile(node);
      break;

    case AST_RETURN:
      result = generateReturn(node);
      break;

    case AST_PRINT:
      result = generatePrint(node);
      break;

    case AST_READ:
      result = generateRead(node);
      break;

    case AST_FUNCTION_DECLARATION:
      result = generateFunction(node);
      break;

    case AST_PROGRAM:
    case AST_DECLARATION_LIST:
    case AST_COMMAND_LIST:
    case AST_BLOCK:
    case AST_PARAMETER_LIST:
    case AST_ARGUMENT_LIST:
    case AST_PRINT_LIST:
    case AST_LITERAL_LIST:
      // Processar filhos sequencialmente
      code0 = generateTAC(node->child[0]);
      code1 = generateTAC(node->child[1]);
      code2 = generateTAC(node->child[2]);
      code3 = generateTAC(node->child[3]);
      result = tacJoin(tacJoin(tacJoin(code0, code1), code2), code3);
      break;

    case AST_VAR_DECLARATION:
    case AST_VECTOR_DECLARATION:
    case AST_PARAMETER:
      // Declarações não geram código TAC (só ocupam espaço na memória)
      result = nullptr;
      break;

    default:
      result = nullptr;
      break;
  }

  return result;
}