/*
 * Compiladores - etapa4 - main.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 */

#include "symbols.hpp"
#include "ast.hpp"
#include "semantic.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

// Declarações das funções externas
extern int yyparse();
extern FILE *yyin;
extern void initMe();
extern ASTNode* programRoot;

int main(int argc, char *argv[]) {
  // Verifica se foram passados dois arquivos como parâmetros
  if (argc != 3) {
    cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << endl;
    return 1;
  }

  // Tenta abrir o arquivo de entrada
  yyin = fopen(argv[1], "r");
  if (!yyin) {
    cerr << "Erro: não foi possível abrir o arquivo de entrada " << argv[1] << endl;
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

    // Executa análise semântica
    SemanticAnalyzer semantic;
    bool semanticSuccess = semantic.analyze(programRoot);

    // Imprime a tabela de símbolos
    symbolTable->printTable();

    // Imprime a AST
    printAST(programRoot);

    // Se houve erros semânticos, termina com exit(4)
    if (!semanticSuccess) {
      cerr << "Compilação FALHOU devido a erros semânticos" << endl;
      if (programRoot) freeAST(programRoot);
      finalizeSymbolTable();
      exit(4);
    }

    // Abre arquivo de saída para geração de código
    FILE* outputFile = fopen(argv[2], "w");
    if (!outputFile) {
      cerr << "Erro: não foi possível abrir o arquivo de saída " << argv[2] << endl;
      finalizeSymbolTable();
      if (programRoot) freeAST(programRoot);
      return 4;
    }

    // Gera o código fonte equivalente
    cout << "Gerando código fonte equivalente em: " << argv[2] << endl;
    generateCode(programRoot, outputFile);
    fclose(outputFile);

    cout << "\nCompilação concluída com SUCESSO!" << endl;

    // Libera memória
    if (programRoot) freeAST(programRoot);
    finalizeSymbolTable();

    exit(0);
  } else {
    // Em caso de erro, o yyerror já chamou exit(3)
    // Mas por segurança, finalizamos aqui também
    if (programRoot) freeAST(programRoot);
    finalizeSymbolTable();
    exit(3);
  }

  return 0;
}