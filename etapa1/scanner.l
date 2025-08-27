/*
 * Compiladores - etapa1 - scanner.l - semestre 2025/2 
 * Autor: Santiago Gonzaga
 */

%{
#include <iostream>
#include <map>
#include <string>
#include "tokens.h"

using namespace std;

// Tabela de símbolos global
map<string, int> symbolTable;

// Variáveis de controle
int lineNumber = 1;
int running = 1;

// Função para obter o número da linha
int getLineNumber(void) {
    return lineNumber;
}

// Função para verificar se está rodando
int isRunning(void) {
    return running;
}

// Função de inicialização
void initMe(void) {
    lineNumber = 1;
    running = 1;
    symbolTable.clear();
}

// Função para inserir na tabela de símbolos
void insertSymbol(string lexeme, int token) {
    symbolTable[lexeme] = token;
}

%}

%option noyywrap

/* Definições de expressões regulares */
DIGIT           [0-9]
LETTER          [a-zA-Z]
IDENTIFIER      ({LETTER}|_|-)({LETTER}|{DIGIT}|_|-)*
INTEGER         {DIGIT}+
FLOAT           {DIGIT}+\.{DIGIT}+
CHAR_LITERAL    '.'
STRING_LITERAL  \"([^\"\\]|\\.)*\"
WHITESPACE      [ \t]
NEWLINE         \n

/* Comentários */
SINGLE_COMMENT  \/\/.*
MULTI_COMMENT   \/\*([^*]|\*+[^*/])*\*+\/

%%

{WHITESPACE}    { /* ignora espaços e tabs */ }

{NEWLINE}       { lineNumber++; }

{SINGLE_COMMENT} { /* ignora comentários de linha única */ }

{MULTI_COMMENT} { 
    /* conta quebras de linha em comentários multilinha */
    for (int i = 0; yytext[i]; i++) {
        if (yytext[i] == '\n') lineNumber++;
    }
}

"char"          { return KW_CHAR; }
"int"           { return KW_INT; }
"float"         { return KW_FLOAT; }
"bool"          { return KW_BOOL; }
"if"            { return KW_IF; }
"else"          { return KW_ELSE; }
"do"            { return KW_DO; }
"while"         { return KW_WHILE; }
"read"          { return KW_READ; }
"print"         { return KW_PRINT; }
"return"        { return KW_RETURN; }
"true"          { insertSymbol(yytext, LIT_TRUE); return LIT_TRUE; }
"false"         { insertSymbol(yytext, LIT_FLASE); return LIT_FLASE; }

"<="            { return OPERATOR_LE; }
">="            { return OPERATOR_GE; }
"=="            { return OPERATOR_EQ; }
"!="            { return OPERATOR_DIF; }

[,;:()[\]{}=+\-*/%<>&|~] { return yytext[0]; }

{IDENTIFIER}    { 
    insertSymbol(yytext, TK_IDENTIFIER); 
    return TK_IDENTIFIER; 
}

{INTEGER}       { 
    insertSymbol(yytext, LIT_INT); 
    return LIT_INT; 
}

{FLOAT}         { 
    insertSymbol(yytext, LIT_FLOAT); 
    return LIT_FLOAT; 
}

{CHAR_LITERAL}  { 
    insertSymbol(yytext, LIT_CHAR); 
    return LIT_CHAR; 
}

{STRING_LITERAL} { 
    insertSymbol(yytext, LIT_STRING); 
    return LIT_STRING; 
}

<<EOF>>         { 
    running = 0; 
    return 0; 
}

.               { 
    return TOKEN_ERROR; 
}

%%