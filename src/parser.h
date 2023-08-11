#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"

typedef enum ExpressionType
{
    EXPR_DefVar,
    EXPR_SetVar,
    EXPR_Conditional,
    EXPR_FunctionCall,
    EXPR_DefFunction,
    EXPR_Literal,
    EXPR_Program
}ExpressionType;

typedef enum LiteralType
{
    LITERAL_ID,
    LITERAL_Num
}LiteralType;

typedef enum ConditionalType
{
    CONDITIONAL_If
    //TODO: add CONDITIONAL_Else and CONDITIONAL_ElseIf
}ConditionalType;

struct Expression;

typedef struct ExpressionArray
{
    unsigned int size;
    unsigned int heap;
    struct Expression* content;
}ExpressionArray;

typedef struct Argument
{
    const char* id;
}Argument;

typedef struct ArgumentArray
{
    unsigned int size;
    unsigned int heap;
    Argument* content;
}ArgumentArray;

typedef struct Expression
{
    ExpressionType type;
    union
    {
        struct
        {
            const char* id;
            struct Expression* expr;
        }DefVar;
        struct
        {
            const char* id;
            struct Expression* expr;
        }SetVar;
        struct
        {
            ConditionalType type;
            struct Expression* expr;
            struct Expression* program;
        }Conditional;
        struct
        {
            const char* id;
            ExpressionArray arguments;
        }FunctionCall;
        struct
        {
            const char* id;
            ArgumentArray arguments;
            struct Expression* program;
        }DefFunction;
        struct
        {
            LiteralType type;
            union
            {
                double num;
                const char* id;
            }value;
        }Literal;
        struct
        {
            ExpressionArray expressions;
        }Program;
    }e;
}Expression;

void ArgumentArray_Initialize(ArgumentArray* array);
void ArgumentArray_Push(ArgumentArray* array, Argument element);

void ExpressionArray_Initalize(ExpressionArray* array);
void ExpressionArray_Push(ExpressionArray* array, Expression element);

Expression Expr_Program();
Expression Expr_DefVar(const char* id, Expression expr);
Expression Expr_DefFunction(const char* id, ArgumentArray arguments, Expression program);

Expression Parse_Literal(Token tok);
Expression Parse_DefVar(TokenArray* tokens);
Expression Parse_FunctionCall(const char* id, TokenArray* tokens);
Expression Parse_CodeBlock(TokenArray* tokens);
Expression Parse_DefFunction(TokenArray* tokens);
Expression Parse_Tokens(TokenArray* tokens);
Expression Parse_AST(TokenArray tokens);

#endif // PARSER_H_