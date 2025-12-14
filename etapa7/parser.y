/*
 * Compiladores - etapa7 - parser.y - semestre 2025/2
 * Autor: Santiago Gonzaga
 * Recuperação de erros sintáticos (modo pânico)
 */

%{
#include <iostream>
#include <string>
#include <cstdlib>
#include "symbols.hpp"
#include "ast.hpp"

using namespace std;

extern int yylex();
extern int getLineNumber();
extern char* yytext;

void yyerror(const char* msg);

ASTNode* programRoot = nullptr;

// Contador de erros sintáticos para recuperação
int syntaxErrorCount = 0;
%}

%token KW_CHAR KW_INT KW_FLOAT KW_BOOL
%token KW_IF KW_ELSE KW_WHILE
%token KW_READ KW_PRINT KW_RETURN
%token OPERATOR_LE OPERATOR_GE OPERATOR_EQ OPERATOR_DIF
%token TK_IDENTIFIER
%token LIT_INT LIT_CHAR LIT_FLOAT LIT_TRUE LIT_FALSE LIT_STRING
%token TOKEN_ERROR

%union {
    SymbolNode* symbol;
    ASTNode* ast;
}

%type <symbol> TK_IDENTIFIER LIT_INT LIT_CHAR LIT_FLOAT LIT_TRUE LIT_FALSE LIT_STRING
%type <ast> program global_declaration_list global_declaration variable_declaration vector_declaration function_declaration
%type <ast> parameter_list parameter_list_non_empty parameter local_variable_list type literal
%type <ast> block command_list command assignment print_list print_element flow_control expression
%type <ast> argument_list argument_list_non_empty literal_list

%left '|'
%left '&'
%left OPERATOR_EQ OPERATOR_DIF
%left '<' '>' OPERATOR_LE OPERATOR_GE
%left '+' '-'
%left '*' '/' '%'
%right '~'

%%

program:
    global_declaration_list { $$ = createNode(AST_PROGRAM, nullptr, $1, nullptr, nullptr, nullptr); programRoot = $$; }
    ;

global_declaration_list:
    /* empty */ { $$ = nullptr; }
    | global_declaration_list global_declaration { $$ = createNode(AST_DECLARATION_LIST, nullptr, $1, $2, nullptr, nullptr); }
    | global_declaration_list error {
        cerr << "Erro sintático na linha " << getLineNumber() << ": declaração global inválida" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = $1;
    }
    ;

global_declaration:
    variable_declaration { $$ = $1; }
    | vector_declaration { $$ = $1; }
    | function_declaration { $$ = $1; }
    ;

variable_declaration:
    type TK_IDENTIFIER '=' literal ';' { $$ = createNode(AST_VAR_DECLARATION, nullptr, $1, createLeafNode(AST_IDENTIFIER, $2), $4, nullptr); }
    | type TK_IDENTIFIER '=' error ';' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": literal inválido na declaração de variável" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    ;

vector_declaration:
    type TK_IDENTIFIER '[' LIT_INT ']' ';' { $$ = createNode(AST_VECTOR_DECLARATION, nullptr, $1, createLeafNode(AST_IDENTIFIER, $2), createLeafNode(AST_LITERAL, $4), nullptr); }
    | type TK_IDENTIFIER '[' LIT_INT ']' '=' literal_list ';' { $$ = createNode(AST_VECTOR_DECLARATION, nullptr, $1, createLeafNode(AST_IDENTIFIER, $2), createLeafNode(AST_LITERAL, $4), $7); }
    | type TK_IDENTIFIER '[' error ']' ';' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": tamanho de vetor inválido" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    | type TK_IDENTIFIER '[' error ']' '=' literal_list ';' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": tamanho de vetor inválido" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    ;

literal_list:
    literal { $$ = createNode(AST_LITERAL_LIST, nullptr, $1, nullptr, nullptr, nullptr); }
    | literal_list literal { $$ = createNode(AST_LITERAL_LIST, nullptr, $1, $2, nullptr, nullptr); }
    ;

function_declaration:
    type TK_IDENTIFIER '(' parameter_list ')' local_variable_list block { $$ = createNode(AST_FUNCTION_DECLARATION, nullptr, $1, createLeafNode(AST_IDENTIFIER, $2), $4, createNode(AST_DECLARATION_LIST, nullptr, $6, $7, nullptr, nullptr)); }
    | type TK_IDENTIFIER '(' error ')' local_variable_list block {
        cerr << "Erro sintático na linha " << getLineNumber() << ": lista de parâmetros inválida" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    ;

parameter_list:
    /* empty */ { $$ = nullptr; }
    | parameter_list_non_empty { $$ = $1; }
    ;

parameter_list_non_empty:
    parameter { $$ = createNode(AST_PARAMETER_LIST, nullptr, $1, nullptr, nullptr, nullptr); }
    | parameter_list_non_empty ',' parameter { $$ = createNode(AST_PARAMETER_LIST, nullptr, $1, $3, nullptr, nullptr); }
    ;

parameter:
    type TK_IDENTIFIER { $$ = createNode(AST_PARAMETER, nullptr, $1, createLeafNode(AST_IDENTIFIER, $2), nullptr, nullptr); }
    ;

local_variable_list:
    /* empty */ { $$ = nullptr; }
    | local_variable_list variable_declaration { $$ = createNode(AST_DECLARATION_LIST, nullptr, $1, $2, nullptr, nullptr); }
    ;

type:
    KW_CHAR { $$ = createLeafNode(AST_LITERAL, symbolTable->insert("char", SYMBOL_IDENTIFIER)); }
    | KW_INT { $$ = createLeafNode(AST_LITERAL, symbolTable->insert("int", SYMBOL_IDENTIFIER)); }
    | KW_FLOAT { $$ = createLeafNode(AST_LITERAL, symbolTable->insert("float", SYMBOL_IDENTIFIER)); }
    | KW_BOOL { $$ = createLeafNode(AST_LITERAL, symbolTable->insert("bool", SYMBOL_IDENTIFIER)); }
    ;

literal:
    LIT_INT { $$ = createLeafNode(AST_LITERAL, $1); }
    | LIT_CHAR { $$ = createLeafNode(AST_LITERAL, $1); }
    | LIT_FLOAT { $$ = createLeafNode(AST_LITERAL, $1); }
    | LIT_TRUE { $$ = createLeafNode(AST_LITERAL, $1); }
    | LIT_FALSE { $$ = createLeafNode(AST_LITERAL, $1); }
    ;

block:
    '{' command_list '}' { $$ = createNode(AST_BLOCK, nullptr, $2, nullptr, nullptr, nullptr); }
    | '{' error '}' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": bloco de comandos inválido" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    ;

command_list:
    /* empty */ { $$ = nullptr; }
    | command_list command { $$ = createNode(AST_COMMAND_LIST, nullptr, $1, $2, nullptr, nullptr); }
    ;

command:
    ';' { $$ = nullptr; }
    | assignment ';' { $$ = $1; }
    | flow_control { $$ = $1; }
    | KW_READ TK_IDENTIFIER ';' { $$ = createNode(AST_READ, nullptr, createLeafNode(AST_IDENTIFIER, $2), nullptr, nullptr, nullptr); }
    | KW_PRINT print_list ';' { $$ = createNode(AST_PRINT, nullptr, $2, nullptr, nullptr, nullptr); }
    | KW_RETURN expression ';' { $$ = createNode(AST_RETURN, nullptr, $2, nullptr, nullptr, nullptr); }
    | block { $$ = $1; }
    | error ';' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": comando inválido" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    | KW_READ error ';' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": comando read inválido" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    | KW_PRINT error ';' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": comando print inválido" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    | KW_RETURN error ';' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": comando return inválido" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    ;

assignment:
    TK_IDENTIFIER '=' expression { $$ = createNode(AST_ASSIGNMENT, nullptr, createLeafNode(AST_IDENTIFIER, $1), $3, nullptr, nullptr); }
    | TK_IDENTIFIER '[' expression ']' '=' expression { $$ = createNode(AST_VECTOR_ASSIGNMENT, nullptr, createLeafNode(AST_IDENTIFIER, $1), $3, $6, nullptr); }
    ;

print_list:
    print_element { $$ = createNode(AST_PRINT_LIST, nullptr, $1, nullptr, nullptr, nullptr); }
    | print_list print_element { $$ = createNode(AST_PRINT_LIST, nullptr, $1, $2, nullptr, nullptr); }
    ;

print_element:
    LIT_STRING { $$ = createLeafNode(AST_LITERAL, $1); }
    | expression { $$ = $1; }
    ;

flow_control:
    KW_IF '(' expression ')' command %prec KW_IF { $$ = createNode(AST_IF, nullptr, $3, $5, nullptr, nullptr); }
    | KW_IF '(' expression ')' command KW_ELSE command { $$ = createNode(AST_IF_ELSE, nullptr, $3, $5, $7, nullptr); }
    | KW_WHILE '(' expression ')' command { $$ = createNode(AST_WHILE, nullptr, $3, $5, nullptr, nullptr); }
    | KW_IF '(' error ')' command %prec KW_IF {
        cerr << "Erro sintático na linha " << getLineNumber() << ": expressão inválida no if" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    | KW_IF '(' error ')' command KW_ELSE command {
        cerr << "Erro sintático na linha " << getLineNumber() << ": expressão inválida no if-else" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    | KW_WHILE '(' error ')' command {
        cerr << "Erro sintático na linha " << getLineNumber() << ": expressão inválida no while" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    ;

expression:
    literal { $$ = $1; }
    | TK_IDENTIFIER { $$ = createLeafNode(AST_IDENTIFIER, $1); }
    | TK_IDENTIFIER '[' expression ']' { $$ = createNode(AST_VECTOR_ACCESS, nullptr, createLeafNode(AST_IDENTIFIER, $1), $3, nullptr, nullptr); }
    | TK_IDENTIFIER '(' argument_list ')' { $$ = createNode(AST_FUNCTION_CALL, nullptr, createLeafNode(AST_IDENTIFIER, $1), $3, nullptr, nullptr); }
    | '(' expression ')' { $$ = $2; }
    | expression '+' expression { $$ = createOperatorNode(AST_ADD, $1, $3); }
    | expression '-' expression { $$ = createOperatorNode(AST_SUB, $1, $3); }
    | expression '*' expression { $$ = createOperatorNode(AST_MUL, $1, $3); }
    | expression '/' expression { $$ = createOperatorNode(AST_DIV, $1, $3); }
    | expression '%' expression { $$ = createOperatorNode(AST_MOD, $1, $3); }
    | expression '<' expression { $$ = createOperatorNode(AST_LT, $1, $3); }
    | expression '>' expression { $$ = createOperatorNode(AST_GT, $1, $3); }
    | expression OPERATOR_LE expression { $$ = createOperatorNode(AST_LE, $1, $3); }
    | expression OPERATOR_GE expression { $$ = createOperatorNode(AST_GE, $1, $3); }
    | expression OPERATOR_EQ expression { $$ = createOperatorNode(AST_EQ, $1, $3); }
    | expression OPERATOR_DIF expression { $$ = createOperatorNode(AST_DIF, $1, $3); }
    | expression '&' expression { $$ = createOperatorNode(AST_AND, $1, $3); }
    | expression '|' expression { $$ = createOperatorNode(AST_OR, $1, $3); }
    | '~' expression { $$ = createNode(AST_EXPRESSION_UNOP, nullptr, $2, nullptr, nullptr, nullptr); ((ASTNode*)$$)->operator_type = AST_NOT; }
    | '(' error ')' {
        cerr << "Erro sintático na linha " << getLineNumber() << ": expressão entre parênteses inválida" << endl;
        syntaxErrorCount++;
        yyerrok;
        $$ = nullptr;
    }
    ;

argument_list:
    /* empty */ { $$ = nullptr; }
    | argument_list_non_empty { $$ = $1; }
    ;

argument_list_non_empty:
    expression { $$ = createNode(AST_ARGUMENT_LIST, nullptr, $1, nullptr, nullptr, nullptr); }
    | argument_list_non_empty ',' expression { $$ = createNode(AST_ARGUMENT_LIST, nullptr, $1, $3, nullptr, nullptr); }
    ;

%%

void yyerror(const char* msg) {
    // Não incrementa syntaxErrorCount aqui porque as regras de error já fazem isso
    // Esta função é chamada pelo Bison antes de entrar na regra de error
}
