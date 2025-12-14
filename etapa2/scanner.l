/*
 * Compiladores - etapa2 - scanner.l - semestre 2025/2 
 * Autor: Santiago Gonzaga
 */

%{
#include <iostream>
#include <string>
#include "symbols.hpp"
#include "parser.tab.h"

using namespace std;

int lineNumber = 1;

int getLineNumber(void) {
    return lineNumber;
}

void initMe(void) {
    lineNumber = 1;
    initSymbolTable();
}
%}

%option noyywrap

DIGIT           [0-9]
LETTER          [a-zA-Z]
IDENTIFIER      ({LETTER}|_|-)({LETTER}|{DIGIT}|_|-)*
INTEGER         {DIGIT}+
FLOAT           {DIGIT}+\.{DIGIT}+
CHAR_LITERAL    '([^'\\]|\\.)'
STRING_LITERAL  \"([^\"\\]|\\.)*\"
WHITESPACE      [ \t]
NEWLINE         \n
SINGLE_COMMENT  \/\/.*
MULTI_COMMENT   \/\*([^*]|\*+[^*/])*\*+\/

%%

{WHITESPACE}    { /* ignora espaços e tabs */ }
{NEWLINE}       { lineNumber++; }
{SINGLE_COMMENT} { /* ignora comentários de linha única */ }
{MULTI_COMMENT} { 
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
"while"         { return KW_WHILE; }
"read"          { return KW_READ; }
"print"         { return KW_PRINT; }
"return"        { return KW_RETURN; }

"true"          { 
    yylval.symbol = symbolTable->insert(yytext, SYMBOL_LIT_BOOL); 
    return LIT_TRUE; 
}
"false"         { 
    yylval.symbol = symbolTable->insert(yytext, SYMBOL_LIT_BOOL); 
    return LIT_FLASE; 
}

"<="            { return OPERATOR_LE; }
">="            { return OPERATOR_GE; }
"=="            { return OPERATOR_EQ; }
"!="            { return OPERATOR_DIF; }

[,;:()[\]{}=+\-*/%<>&|~] { return yytext[0]; }

{IDENTIFIER}    { 
    yylval.symbol = symbolTable->insert(yytext, SYMBOL_IDENTIFIER); 
    return TK_IDENTIFIER; 
}
{INTEGER}       { 
    yylval.symbol = symbolTable->insert(yytext, SYMBOL_LIT_INT); 
    return LIT_INT; 
}
{FLOAT}         { 
    yylval.symbol = symbolTable->insert(yytext, SYMBOL_LIT_FLOAT); 
    return LIT_FLOAT; 
}
{CHAR_LITERAL}  { 
    yylval.symbol = symbolTable->insert(yytext, SYMBOL_LIT_CHAR); 
    return LIT_CHAR; 
}
{STRING_LITERAL} { 
    yylval.symbol = symbolTable->insert(yytext, SYMBOL_LIT_STRING); 
    return LIT_STRING; 
}

<<EOF>>         { return 0; }
.               { return TOKEN_ERROR; }

%%