#include "interpreter.h"

VariableArray* currentScope;
VariableArray mainScope;
FunctionArray functions;

void Initialize_Luna()
{
    VariableArray_Initalize(&mainScope);
    currentScope = &mainScope;
    FunctionArray_Initalize(&functions);
}

void VariableArray_Initalize(VariableArray* array)
{
    array->size = 0;
    array->heap = 1;
    array->content = malloc(array->heap*sizeof(Variable));
}

void VariableArray_Push(VariableArray* array, Variable element)
{
    if(array->heap <= array->size)
    {
        array->heap*=2;
        array->content = realloc(array->content, array->heap*sizeof(Variable));
    }
    array->content[array->size++] = element;
}

Variable* VariableArray_Find(VariableArray* array, const char* id)
{
    for(unsigned int i = 0; i < array->size; ++i)
    {
        if(!strcmp(array->content[i].id, id))
            return &array->content[i];
    }
    return NULL;
}

void VariableArray_Drop(VariableArray* array, const char* id)
{
    for (unsigned int i = 0; i < array->size; ++i)
    {
        if (!strcmp(array->content[i].id, id))
        {
            for (unsigned int j = i; j < array->size - 1; j++) {
                array->content[j] = array->content[j + 1];
            }
            array->size--;
        }
    }
}

void FunctionArray_Initalize(FunctionArray* array)
{
    array->size = 0;
    array->heap = 1;
    array->content = malloc(array->heap*sizeof(Function));
}

void FunctionArray_Push(FunctionArray* array, Function element)
{
    if(array->heap <= array->size)
    {
        array->heap*=2;
        array->content = realloc(array->content, array->heap*sizeof(Function));
    }
    array->content[array->size++] = element;
}

Function* FunctionArray_Find(FunctionArray* array, const char* id)
{
    for(unsigned int i = 0; i < array->size; ++i)
    {
        if(!strcmp(array->content[i].id, id))
            return &array->content[i];
    }
    return NULL;
}

void VariableArray_Free(VariableArray* array)
{
    array->size = 0;
    array->heap = 0;
    free(array->content);
    array->content = NULL;
}

Variable Interpret(Expression e, ExpressionArray* array, unsigned int i)
{
    switch(e.type)
    {
        case EXPR_DefVar:
            return Interpret_DefVar(e);
        break;
        case EXPR_SetVar:
            return Interpret_SetVar(e);
        break;
        case EXPR_FunctionCall:
            return Interpret_FunctionCall(e);
        break;
        case EXPR_Literal:
            return Interpret_Literal(e);
        break;
        case EXPR_Program:
            return Interpret_Program(e);
        break;
        case EXPR_DefFunction:
            return Interpret_DefFunction(e);
        break;
        case EXPR_Conditional:
            return Interpret_Conditional(e, array, i);
        break;
        case EXPR_Error:
            printf("ERROR at line %d: %s\n", e.e.Error.line, e.e.Error.msg);
            exit(1);
        break;
        default:
            printf("TODO: Implement (%d)!\n", e.type);
            exit(1);
        break;
    }

    return (Variable){.id = NULL, .value = 0, .isNull = 1};
}

Variable Interpret_Literal(Expression literal)
{
    switch(literal.e.Literal.type)
    {
        case LITERAL_Num:
            return (Variable){.id = NULL, .value = literal.e.Literal.value.num, .isNull = 0};
        break;
        case LITERAL_ID:
        {
            Variable* var = VariableArray_Find(currentScope, literal.e.Literal.value.id);
            if(var == NULL)
            {
                printf("ERROR: variable \"%s\" is not defined!\n", literal.e.Literal.value.id);
                exit(1);
            }
            return *var;
        }
        break;
    }
    
    return (Variable){.id = NULL, .value = 0, .isNull = 1};
}

Variable Interpret_DefVar(Expression defvar)
{
    if(VariableArray_Find(currentScope, defvar.e.DefVar.id))
    {
        printf("ERROR: Can't re-define variable \"%s\"\n", defvar.e.DefVar.id);
        exit(1);
    }
    
    Variable eval = Interpret(*defvar.e.DefVar.expr, NULL, 0);
    if(eval.isNull)
    {
        printf("ERROR: Can't set variable \"%s\" to a null variable\n", defvar.e.DefVar.id);
        exit(1);
    }

    Variable var = (Variable){.id = _strdup(defvar.e.DefVar.id), .value = eval.value};
    VariableArray_Push(currentScope, var);
    
    return *VariableArray_Find(currentScope, var.id);
}

Variable Interpret_SetVar(Expression setvar)
{
    Variable eval = Interpret(*setvar.e.SetVar.expr, NULL, 0);
    if(eval.isNull)
    {
        printf("ERROR: Can't set variable \"%s\" to a null variable\n", setvar.e.SetVar.id);
        exit(1);
    }
    Variable var = (Variable){.id = _strdup(setvar.e.SetVar.id), .value = eval.value};
    Variable* eVar = VariableArray_Find(currentScope, var.id);
    if(!eVar)
    {
        printf("ERROR: Can't set variable \"%s\", it is not defined\n", var.id);
        exit(1);
    }
    *eVar = var;
    return *eVar;
}

Variable Interpret_Function(Function function, VariableArray arguments)
{
    // if the function is empty, we just don't even try to run it
    if(function.program.type == EXPR_Program && !function.program.e.Program.expressions.size)
        return (Variable){.id=NULL, .value=0, .isNull=1};

    VariableArray newScope;
    VariableArray_Initalize(&newScope);
    currentScope = &newScope;

    for(unsigned int i = 0; i < arguments.size; ++i)
    {
        Variable var = arguments.content[i];
        var.id = _strdup(function.arguments.content[i].id);

        if(VariableArray_Find(currentScope, var.id))
        {
            printf("ERROR: Can't re-define variable \"%s\"\n", var.id);
            exit(1);
        }

        VariableArray_Push(currentScope, var);
    }
    
    Variable v = Interpret(function.program, NULL, 0);

    VariableArray_Free(&newScope);

    return v;
}

Variable Interpret_FunctionCall(Expression fcall)
{
    VariableArray variables = (VariableArray){};
    VariableArray_Initalize(&variables);

    for(unsigned int i = 0; i < fcall.e.FunctionCall.arguments.size; ++i)
    {
        VariableArray_Push(&variables, Interpret(fcall.e.FunctionCall.arguments.content[i], NULL, 0));
    }

    if(fcall.e.FunctionCall.arguments.size != variables.size)
    {
        printf("BUG: Something went wrong while Interpreting argument to function \"%s\"\n", fcall.e.FunctionCall.id);
        exit(1);
    }

    for(unsigned int i = 0; i < variables.size; ++i)
    {
        if(variables.content[i].isNull)
        {
            printf("ERROR: Can't pass null variables to functions!!! ( function: \"%s\" )\n", fcall.e.FunctionCall.id);
            exit(1);
        }
    }

    if(!strcmp(fcall.e.FunctionCall.id, "print"))
    {
        if(variables.size > 1)
        {
            printf("ERROR: To many arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }
        if(variables.size == 0)
        {
            printf("ERROR: To few arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }

        printf("%g\n", variables.content[0].value);
        return variables.content[0];
    }
    else if(!strcmp(fcall.e.FunctionCall.id, "add"))
    {
        if(variables.size > 2)
        {
            printf("ERROR: To many arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }
        if(variables.size < 2)
        {
            printf("ERROR: To few arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }

        Variable var = (Variable){.id = NULL, .value = variables.content[0].value + variables.content[1].value, .isNull = 0};
        return var;
    }
    else if(!strcmp(fcall.e.FunctionCall.id, "sub"))
    {
        if(variables.size > 2)
        {
            printf("ERROR: To many arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }
        if(variables.size < 2)
        {
            printf("ERROR: To few arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }

        Variable var = (Variable){.id = NULL, .value = variables.content[0].value - variables.content[1].value, .isNull = 0};
        return var;
    }
    else if(!strcmp(fcall.e.FunctionCall.id, "mul"))
    {
        if(variables.size > 2)
        {
            printf("ERROR: To many arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }
        if(variables.size < 2)
        {
            printf("ERROR: To few arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }

        Variable var = (Variable){.id = NULL, .value = variables.content[0].value * variables.content[1].value, .isNull = 0};
        return var;
    }
    else if(!strcmp(fcall.e.FunctionCall.id, "div"))
    {
        if(variables.size > 2)
        {
            printf("ERROR: To many arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }
        if(variables.size < 2)
        {
            printf("ERROR: To few arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }

        Variable var = (Variable){.id = NULL, .value = variables.content[0].value / variables.content[1].value, .isNull = 0};
        return var;
    }
    else if(!strcmp(fcall.e.FunctionCall.id, "round"))
    {
        if(variables.size > 1)
        {
            printf("ERROR: To many arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }
        if(variables.size < 1)
        {
            printf("ERROR: To few arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }

        Variable var = (Variable){.id = NULL, .value = round(variables.content[0].value), .isNull = 0};
        return var;
    }
    else if(!strcmp(fcall.e.FunctionCall.id, "sign"))
    {
        if(variables.size > 1)
        {
            printf("ERROR: To many arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }
        if(variables.size < 1)
        {
            printf("ERROR: To few arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }

        double v = 0;
        if(variables.content[0].value < 0)
        {
            v = -1;
        }
        else if(variables.content[0].value > 0)
        {
            v = 1;
        }
        Variable var = (Variable){.id = NULL, .value = v, .isNull = 0};
        return var;
    }
    else
    {
        Function* function = FunctionArray_Find(&functions, fcall.e.FunctionCall.id);
        if(!function)
        {
            printf("ERROR: Can't call function \"%s\" cuz it is not defined\n", fcall.e.FunctionCall.id);
            exit(1);
        } 

        if(variables.size > function->arguments.size)
        {
            printf("ERROR: To many arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }
        if(variables.size < function->arguments.size)
        {
            printf("ERROR: To few arguments passed to function \"%s\"", fcall.e.FunctionCall.id);
            exit(1);
        }

        VariableArray* lastScope = currentScope;
        Variable r = Interpret_Function(*function, variables);
        currentScope = lastScope;

        return r;
    }
    return (Variable){.id = NULL, .value = 0, .isNull = 1};
}

Variable Interpret_DefFunction(Expression deffunc)
{
    if(FunctionArray_Find(&functions, deffunc.e.DefFunction.id))
    {
        printf("ERROR: Can't re-define function \"%s\"\n", deffunc.e.DefFunction.id);
        exit(1);
    }
    Function function;
    function.arguments = deffunc.e.DefFunction.arguments;
    function.id = _strdup(deffunc.e.DefFunction.id);
    function.program = *deffunc.e.DefFunction.program;
    FunctionArray_Push(&functions, function);
    return (Variable){.id = NULL, .value = 0, .isNull = 1};
}

Variable Interpret_Conditional(Expression expr, ExpressionArray* array, unsigned int i)
{
    switch(expr.e.Conditional.type)
    {
        case CONDITIONAL_If:
        {
            if(Interpret(*expr.e.Conditional.expr, array, i).value)
            {
                return Interpret(*expr.e.Conditional.program, array, i);
            }
        } 
        break;
        case CONDITIONAL_Else:
            if(array->content[i-1].type != EXPR_Conditional)
            {
                printf("ERROR: \"else\" is meant to be used after an control flow expression!");
                exit(1);
            }
            if(array->content[i-1].e.Conditional.type != CONDITIONAL_If)
            {
                printf("ERROR: \"else\" is meant to be used after an \"if\"!");
                exit(1);
            }
            if(!Interpret(*array->content[i-1].e.Conditional.expr, array, i).value)
            {
                return Interpret(*expr.e.Conditional.program, array, i);
            }
        break;
    }
    return (Variable){.id=NULL, .value=0, .isNull=1};
}

Variable Interpret_Program(Expression program)
{
    Variable r = (Variable){.id = NULL, .value = 0, .isNull = 1};

    for(unsigned int i = 0; i < program.e.Program.expressions.size; ++i)
    {
        Expression e = program.e.Program.expressions.content[i];
        Variable var = Interpret(e, &program.e.Program.expressions, i);
        if(!var.isNull)
            r = var;
    }
    return r;
}