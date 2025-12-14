/*
 * Compiladores - etapa4 - semantic.hpp - semestre 2025/2
 * Autor: Santiago Gonzaga
 *
 * Definições para análise semântica
 */

#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include "ast.hpp"
#include "symbols.hpp"

// Classe para análise semântica
class SemanticAnalyzer {
private:
    int errorCount;

    // Funções auxiliares
    void reportError(const std::string& message);
    int getDataTypeFromString(const std::string& typeStr);
    int getDataTypeFromLiteral(int literalType);
    int inferExpressionType(ASTNode* node);
    bool isArithmeticOperator(int op);
    bool isRelationalOperator(int op);
    bool isLogicalOperator(int op);
    bool areTypesCompatible(int type1, int type2);
    int countParameters(ASTNode* paramList);
    int countArguments(ASTNode* argList);
    bool checkParameterArgumentCompatibility(ASTNode* params, ASTNode* args);
    void checkVectorInitialization(ASTNode* node);

public:
    SemanticAnalyzer();

    // Função principal de análise
    bool analyze(ASTNode* root);

    // Primeira passada: registrar declarações
    void firstPass(ASTNode* node);

    // Segunda passada: verificar usos e tipos
    void secondPass(ASTNode* node, int functionReturnType = 0);

    // Verificações específicas
    void checkDeclaration(ASTNode* node);
    void checkUsage(ASTNode* node);
    void checkExpression(ASTNode* node);
    void checkFunctionCall(ASTNode* node);
    void checkReturn(ASTNode* node, int expectedType);
    void checkVectorAccess(ASTNode* node);
    void checkAssignment(ASTNode* node);

    // Obter número de erros
    int getErrorCount() const { return errorCount; }
};

#endif // SEMANTIC_HPP
