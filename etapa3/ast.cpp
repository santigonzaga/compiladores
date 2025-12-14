/*
 * Compiladores - etapa3 - ast.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Implementação da Árvore Sintática Abstrata (AST)
 */

#include "ast.hpp"
#include <iostream>

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