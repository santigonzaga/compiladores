/*
 * Compiladores - etapa2 - symbols.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Implementação da tabela de símbolos
 */

#include "symbols.hpp"
#include <iomanip>
#include <iostream>

// Instância global da tabela de símbolos
SymbolTable *symbolTable = nullptr;

SymbolTable::SymbolTable() { table.clear(); }

SymbolTable::~SymbolTable() { clear(); }

SymbolNode *SymbolTable::insert(const std::string &lexeme, int type) {
  // Se o símbolo já existe, retorna o nodo existente
  auto it = table.find(lexeme);
  if (it != table.end()) {
    return it->second;
  }

  // Criar novo nodo
  SymbolNode *node = new SymbolNode(type, lexeme);
  table[lexeme] = node;

  return node;
}

SymbolNode *SymbolTable::lookup(const std::string &lexeme) {
  auto it = table.find(lexeme);
  if (it != table.end()) {
    return it->second;
  }
  return nullptr;
}

void SymbolTable::clear() {
  for (auto &pair : table) {
    delete pair.second;
  }
  table.clear();
}

bool SymbolTable::exists(const std::string &lexeme) {
  return table.find(lexeme) != table.end();
}

std::string SymbolTable::getTypeName(int type) {
  switch (type) {
  case SYMBOL_LIT_INT:
    return "LIT_INT";
  case SYMBOL_LIT_CHAR:
    return "LIT_CHAR";
  case SYMBOL_LIT_FLOAT:
    return "LIT_FLOAT";
  case SYMBOL_LIT_BOOL:
    return "LIT_BOOL";
  case SYMBOL_LIT_STRING:
    return "LIT_STRING";
  case SYMBOL_IDENTIFIER:
    return "IDENTIFIER";
  default:
    return "UNKNOWN";
  }
}

std::string SymbolTable::getNatureName(int nature) {
  switch (nature) {
  case NATURE_SCALAR:
    return "SCALAR";
  case NATURE_VECTOR:
    return "VECTOR";
  case NATURE_FUNCTION:
    return "FUNCTION";
  default:
    return "-";
  }
}

std::string SymbolTable::getDataTypeName(int dataType) {
  switch (dataType) {
  case DATATYPE_INT:
    return "int";
  case DATATYPE_CHAR:
    return "char";
  case DATATYPE_FLOAT:
    return "float";
  case DATATYPE_BOOL:
    return "bool";
  default:
    return "-";
  }
}

void SymbolTable::printTable() {
  std::cout << "\n========== TABELA DE SÍMBOLOS ==========" << std::endl;
  std::cout << std::left << std::setw(20) << "LEXEMA" << std::setw(15) << "TIPO"
            << std::setw(12) << "NATUREZA" << std::setw(10) << "DATA_TYPE" << std::endl;
  std::cout << "--------------------------------------------------------" << std::endl;

  if (table.empty()) {
    std::cout << "Tabela vazia" << std::endl;
  } else {
    for (const auto &pair : table) {
      std::cout << std::left << std::setw(20) << pair.first << std::setw(15)
                << getTypeName(pair.second->type) << std::setw(12)
                << getNatureName(pair.second->nature) << std::setw(10)
                << getDataTypeName(pair.second->dataType) << std::endl;
    }
  }
  std::cout << "========================================================" << std::endl;
  std::cout << "Total de símbolos: " << table.size() << std::endl << std::endl;
}

void initSymbolTable() {
  if (symbolTable == nullptr) {
    symbolTable = new SymbolTable();
  }
}

void finalizeSymbolTable() {
  if (symbolTable != nullptr) {
    delete symbolTable;
    symbolTable = nullptr;
  }
}