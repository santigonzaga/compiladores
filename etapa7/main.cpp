/*
 * Compiladores - etapa7 - main.cpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 * Recuperacao de erros sintaticos
 */

#include "symbols.hpp"
#include "ast.hpp"
#include "semantic.hpp"
#include "tac.hpp"
#include "asm.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;

// Declaracoes das funcoes externas
extern int yyparse();
extern FILE *yyin;
extern void initMe();
extern ASTNode* programRoot;

// Contador de erros sintaticos (definido no parser.y)
extern int syntaxErrorCount;

int main(int argc, char *argv[]) {
  // Verifica se foram passados dois arquivos como parametros
  if (argc != 3) {
    cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << endl;
    return 1;
  }

  // Tenta abrir o arquivo de entrada
  yyin = fopen(argv[1], "r");
  if (!yyin) {
    cerr << "Erro: nao foi possivel abrir o arquivo de entrada " << argv[1] << endl;
    return 2;
  }

  cout << "Iniciando analise sintatica do arquivo: " << argv[1] << endl;

  // Inicializa o analisador lexico e a tabela de simbolos
  initMe();

  // Executa a analise sintatica (com recuperacao de erros)
  int result = yyparse();

  // Fecha o arquivo
  fclose(yyin);

  // Verifica se houve erros sintaticos (recuperados ou nao)
  if (syntaxErrorCount > 0 || result != 0) {
    cerr << "\nAnalise sintatica concluida com " << syntaxErrorCount << " erro(s) sintatico(s)" << endl;
    cerr << "Geracao de codigo DESABILITADA devido a erros sintaticos" << endl;

    // Ainda executa analise semantica para mostrar todos os erros
    if (programRoot) {
      SemanticAnalyzer semantic;
      semantic.analyze(programRoot);

      // Imprime a tabela de simbolos
      symbolTable->printTable();

      // Imprime a AST (parcial)
      printAST(programRoot);

      freeAST(programRoot);
    }

    finalizeSymbolTable();
    exit(3);
  }

  // Se a analise foi bem-sucedida (sem erros sintaticos)
  cout << "Analise sintatica concluida com SUCESSO!" << endl;

  // Executa analise semantica
  SemanticAnalyzer semantic;
  bool semanticSuccess = semantic.analyze(programRoot);

  // Imprime a tabela de simbolos
  symbolTable->printTable();

  // Imprime a AST
  printAST(programRoot);

  // Se houve erros semanticos, termina com exit(4)
  if (!semanticSuccess) {
    cerr << "Compilacao FALHOU devido a erros semanticos" << endl;
    if (programRoot) freeAST(programRoot);
    finalizeSymbolTable();
    exit(4);
  }

  // Gera codigo TAC (Three Address Code)
  cout << "\nGerando codigo intermediario (TAC)..." << endl;
  TAC* tacCode = generateTAC(programRoot);

  if (tacCode) {
    // A lista esta invertida, usar tacPrintBackwards para reverter e imprimir
    tacPrintBackwards(tacCode);
  }

  // Abre arquivo de saida para geracao de codigo assembly
  FILE* outputFile = fopen(argv[2], "w");
  if (!outputFile) {
    cerr << "Erro: nao foi possivel abrir o arquivo de saida " << argv[2] << endl;
    finalizeSymbolTable();
    if (programRoot) freeAST(programRoot);
    return 4;
  }

  // Gera o codigo assembly
  cout << "Gerando codigo assembly em: " << argv[2] << endl;
  generateAsm(tacCode, outputFile);
  fclose(outputFile);

  cout << "\nCompilacao concluida com SUCESSO!" << endl;
  cout << "\nPara montar e executar o codigo gerado:" << endl;
  cout << "  gcc -o programa " << argv[2] << endl;
  cout << "  ./programa" << endl;

  // Libera memoria do TAC
  if (tacCode) {
    TAC* current = tacCode;
    while (current) {
      TAC* temp = current->prev;
      delete current;
      current = temp;
    }
  }

  // Libera memoria
  if (programRoot) freeAST(programRoot);
  finalizeSymbolTable();

  exit(0);
}
