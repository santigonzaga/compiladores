/*
 * Compiladores - etapa1 - main.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 */

#include "tokens.h"
#include <iostream>

using namespace std;

// Declarações das funções externas
extern int yylex();
extern char *yytext;
extern void initMe();
extern int getLineNumber();
extern int isRunning();

int main() {
  initMe();

  int token;

  cout << "Iniciando análise léxica..." << endl;

  while (isRunning()) {
    token = yylex();

    if (token == 0)
      break;

    cout << "Token: " << token;
    cout << " | Lexema: \"" << yytext << "\"";
    cout << " | Linha: " << getLineNumber() << endl;

    if (token == TOKEN_ERROR) {
      cout << "ERRO LÉXICO: caractere inválido '" << yytext << "' na linha "
           << getLineNumber() << endl;
    }
  }

  cout << "Análise léxica concluída." << endl;

  return 0;
}