#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <math.h>
#include "parser.h"

typedef struct Variable
{
    const char* id;
    double value;
    unsigned char isNull;
}Variable;

typedef struct VariableArray
{
    unsigned int size;
    unsigned int heap;
    struct Variable* content;
}VariableArray;

typedef struct Function
{
    const char* id;
    ArgumentArray arguments;
    Expression program;
}Function;

typedef struct FunctionArray
{
    unsigned int size;
    unsigned int heap;
    struct Function* content;
}FunctionArray;

void VariableArray_Initalize(VariableArray* array);
void VariableArray_Push(VariableArray* array, Variable element);
Variable* VariableArray_Find(VariableArray* array, const char* id);
void VariableArray_Free(VariableArray* array);

void FunctionArray_Initalize(FunctionArray* array);
void FunctionArray_Push(FunctionArray* array, Function element);
Function* FunctionArray_Find(FunctionArray* array, const char* id);

void Initialize_Luna();

Variable Interpret(Expression e, ExpressionArray* array, unsigned int i);
Variable Interpret_DefVar(Expression defvar);
Variable Interpret_SetVar(Expression defvar);
Variable Interpret_Literal(Expression literal);
Variable Interpret_FunctionCall(Expression fcall);
Variable Interpret_DefFunction(Expression deffunc);
Variable Interpret_Conditional(Expression expr, ExpressionArray* array, unsigned int i);
Variable Interpret_Program(Expression program);

#endif // INTERPRETER_H_