/*
 * Compiladores - etapa2 - main.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 */

#include "symbols.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

// Declarações das funções externas
extern int yyparse();
extern FILE *yyin;
extern void initMe();

int main(int argc, char *argv[]) {
  // Verifica se foi passado um arquivo como parâmetro
  if (argc != 2) {
    cerr << "Uso: " << argv[0] << " <arquivo>" << endl;
    return 1;
  }

  // Tenta abrir o arquivo
  yyin = fopen(argv[1], "r");
  if (!yyin) {
    cerr << "Erro: não foi possível abrir o arquivo " << argv[1] << endl;
    return 2;
  }

  cout << "Iniciando análise sintática do arquivo: " << argv[1] << endl;

  // Inicializa o analisador léxico e a tabela de símbolos
  initMe();

  // Executa a análise sintática
  int result = yyparse();

  // Fecha o arquivo
  fclose(yyin);

  // Se a análise foi bem-sucedida
  if (result == 0) {
    cout << "Análise sintática concluída com SUCESSO!" << endl;

    // Imprime a tabela de símbolos
    symbolTable->printTable();

    // Finaliza a tabela de símbolos
    finalizeSymbolTable();

    exit(0);
  } else {
    // Em caso de erro, o yyerror já chamou exit(3)
    // Mas por segurança, finalizamos aqui também
    finalizeSymbolTable();
    exit(3);
  }

  return 0;
}