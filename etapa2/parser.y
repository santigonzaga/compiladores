/*
 * Compiladores - etapa2 - parser.y - semestre 2025/2
 * Autor: Santiago Gonzaga
 */

%{
#include <iostream>
#include <string>
#include <cstdlib>
#include "symbols.hpp"

using namespace std;

extern int yylex();
extern int getLineNumber();
extern char* yytext;

void yyerror(const char* msg);
%}

%token KW_CHAR KW_INT KW_FLOAT KW_BOOL
%token KW_IF KW_ELSE KW_WHILE
%token KW_READ KW_PRINT KW_RETURN
%token OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_DIF
%token TK_IDENTIFIER
%token LIT_INT LIT_CHAR LIT_FLOAT LIT_TRUE LIT_FLASE LIT_STRING
%token TOKEN_ERROR

%union {
    SymbolNode* symbol;
}

%type <symbol> TK_IDENTIFIER LIT_INT LIT_CHAR LIT_FLOAT LIT_TRUE LIT_FLASE LIT_STRING

%left '|'
%left '&' 
%left OPERATOR_EQ OPERATOR_DIF
%left '<' '>' OPERATOR_LE OPERATOR_GE
%left '+' '-'
%left '*' '/' '%'
%right '~'

%%

program: 
    global_declaration_list
    ;

global_declaration_list:
    /* empty */
    | global_declaration_list global_declaration
    ;

global_declaration:
    variable_declaration
    | vector_declaration
    | function_declaration
    ;

variable_declaration:
    type TK_IDENTIFIER '=' literal ';'
    ;

vector_declaration:
    type TK_IDENTIFIER '[' LIT_INT ']' ';'
    | type TK_IDENTIFIER '[' LIT_INT ']' '=' literal_list ';'
    ;

literal_list:
    literal
    | literal_list literal
    ;

function_declaration:
    type TK_IDENTIFIER '(' parameter_list ')' local_variable_list block
    ;

parameter_list:
    /* empty */
    | parameter_list_non_empty
    ;

parameter_list_non_empty:
    parameter
    | parameter_list_non_empty ',' parameter
    ;

parameter:
    type TK_IDENTIFIER
    ;

local_variable_list:
    /* empty */
    | local_variable_list variable_declaration
    ;

type:
    KW_CHAR
    | KW_INT
    | KW_FLOAT
    | KW_BOOL
    ;

literal:
    LIT_INT
    | LIT_CHAR
    | LIT_FLOAT
    | LIT_TRUE
    | LIT_FLASE
    ;

block:
    '{' command_list '}'
    ;

command_list:
    /* empty */
    | command_list command
    ;

command:
    ';'
    | assignment ';'
    | flow_control
    | KW_READ TK_IDENTIFIER ';'
    | KW_PRINT print_list ';'
    | KW_RETURN expression ';'
    | block
    ;

assignment:
    TK_IDENTIFIER '=' expression
    | TK_IDENTIFIER '[' expression ']' '=' expression
    ;

print_list:
    print_element
    | print_list print_element
    ;

print_element:
    LIT_STRING
    | expression
    ;

flow_control:
    KW_IF '(' expression ')' command %prec KW_IF
    | KW_IF '(' expression ')' command KW_ELSE command
    | KW_WHILE '(' expression ')' command
    ;

expression:
    literal
    | TK_IDENTIFIER
    | TK_IDENTIFIER '[' expression ']'
    | TK_IDENTIFIER '(' argument_list ')'
    | '(' expression ')'
    | expression '+' expression
    | expression '-' expression
    | expression '*' expression
    | expression '/' expression
    | expression '%' expression
    | expression '<' expression
    | expression '>' expression
    | expression OPERATOR_LE expression
    | expression OPERATOR_GE expression
    | expression OPERATOR_EQ expression
    | expression OPERATOR_DIF expression
    | expression '&' expression
    | expression '|' expression
    | '~' expression
    ;

argument_list:
    /* empty */
    | argument_list_non_empty
    ;

argument_list_non_empty:
    expression
    | argument_list_non_empty ',' expression
    ;

%%

void yyerror(const char* msg) {
    cerr << "Erro sintático na linha " << getLineNumber() << endl;
    exit(3);
}