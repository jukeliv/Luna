#include "parser.h"

void ArgumentArray_Initialize(ArgumentArray* array)
{
    array->size = 0;
    array->heap = 2;
    array->content = malloc(array->heap*sizeof(Argument));
}

void ArgumentArray_Push(ArgumentArray* array, Argument element)
{
    if(array->heap <= array->size)
    {
        array->heap*=2;
        array->content = realloc(array->content, array->heap*sizeof(Argument));
    }
    array->content[array->size++] = element;
}

void ExpressionArray_Initalize(ExpressionArray* array)
{
    array->size = 0;
    array->heap = 2;
    array->content = malloc(array->heap*sizeof(Expression));
}

void ExpressionArray_Push(ExpressionArray* array, Expression element)
{
    if(array->heap <= array->size)
    {
        array->heap*=2;
        array->content = realloc(array->content, array->heap*sizeof(Expression));
    }
    array->content[array->size++] = element;
}

Expression Expr_Program()
{
    Expression expr;
    expr.type = EXPR_Program;
    ExpressionArray_Initalize(&expr.e.Program.expressions);
    return expr;
}

Expression Expr_DefVar(const char* id, Expression expr)
{
    Expression e;
    e.type = EXPR_DefVar;
    e.e.DefVar.id = _strdup(id);
    e.e.DefVar.expr = malloc(sizeof(Expression));
    *e.e.DefVar.expr = expr;
    return e;
}

Expression Expr_SetVar(const char* id, Expression expr)
{
    Expression e;
    e.type = EXPR_SetVar;
    e.e.SetVar.id = _strdup(id);
    e.e.SetVar.expr = malloc(sizeof(Expression));
    *e.e.SetVar.expr = expr;
    return e;
}

Expression Parse_Literal(Token tok)
{
    Expression expr;
    expr.type = EXPR_Literal;

    switch(tok.type)
    {
        case TOK_ID:
            expr.e.Literal.type = LITERAL_ID;
            expr.e.Literal.value.id = _strdup(tok.value);
        break;
        case TOK_NUM:
            expr.e.Literal.type = LITERAL_Num;
            expr.e.Literal.value.num = strtod(tok.value, NULL);
        break;
        default:
            //handle error
            exit(1);
        break;
    }

    return expr;
}

Expression Parse_DefVar(TokenArray* tokens)
{
    Token id = TokenArray_Shift(tokens);
    if(id.type != TOK_ID)
    {
        //handle error
        exit(1);
    }
    return Expr_DefVar(id.value, Parse_Tokens(tokens));
}

Expression Parse_SetVar(TokenArray* tokens)
{
    Token id = TokenArray_Shift(tokens);
    if(id.type != TOK_ID)
    {
        //handle error
        exit(1);
    }
    return Expr_SetVar(id.value, Parse_Tokens(tokens));
}

void print_token(Token tok)
{
    printf("DEBUG: %d : %s at line %d\n", tok.type, tok.value, tok.line);
}

void print_first_token(TokenArray tokens)
{
    print_token(tokens.content[0]);
}

Expression Expr_DefFunction(const char* id, ArgumentArray arguments, Expression program)
{
    Expression expr;
    expr.type = EXPR_DefFunction;
    expr.e.DefFunction.id = _strdup(id);
    expr.e.DefFunction.arguments = arguments;
    expr.e.DefFunction.program = malloc(sizeof(Expression));
    *expr.e.DefFunction.program = program;
    return expr;
}

Expression Parse_DefFunction(TokenArray* tokens)
{
    const char* id = TokenArray_Shift(tokens).value;
    ArgumentArray arguments;
    ArgumentArray_Initialize(&arguments);

    TokenArray_Shift(tokens);

    while(1)
    {
        Token tok = TokenArray_Shift(tokens);
        if(tok.type == TOK_CLOSE_PARENTH)
        {
            break;
        }
        if(tok.type != TOK_ID)
        {
            printf("ERROR: A function definition should only the fine the name of the variables the function takes! ( function: %s )\n", id);
            print_token(tok);
            exit(1);
        }
        ArgumentArray_Push(&arguments, (Argument){.id = _strdup(tok.value)});
    }

    TokenArray_Shift(tokens);

    return Expr_DefFunction(id, arguments, Parse_CodeBlock(tokens));
}

Expression Parse_FunctionCall(const char* id, TokenArray* tokens)
{
    Expression expr;
    expr.type = EXPR_FunctionCall;
    expr.e.FunctionCall.id = _strdup(id);

    ExpressionArray_Initalize(&expr.e.FunctionCall.arguments);

    unsigned int indentationLevel = 0;
    unsigned int indent = 1;
    for(unsigned int i = 0; tokens->content[i].type != TOK_CLOSE_PARENTH && indentationLevel; ++i)
    {
        Token tok = tokens->content[i];
        if(tok.type == TOK_OPEN_PARENTH)
        {
            indentationLevel++;
            indent++;
        }
        else if(tok.type == TOK_CLOSE_PARENTH)
        {
            indentationLevel--;
        }
    }

    while (indent > 0)
    {
        if(tokens->content[0].type == TOK_CLOSE_PARENTH)
        {
            indent--;
            continue;
        }
        ExpressionArray_Push(&expr.e.FunctionCall.arguments, Parse_Tokens(tokens));
    }

    TokenArray_Shift(tokens);

    return expr;
}

Expression Parse_CodeBlock(TokenArray* tokens)
{
    Expression expr;
    expr.type = EXPR_Program;

    ExpressionArray_Initalize(&expr.e.Program.expressions);

    unsigned int indentationLevel = 0;
    unsigned int indent = 1;
    for(unsigned int i = 0; tokens->content[i].type != TOK_END && indentationLevel; ++i)
    {
        Token tok = tokens->content[i];
        if(tok.type == TOK_DO)
        {
            indentationLevel++;
            indent++;
        }
        else if(tok.type == TOK_END)
        {
            indentationLevel--;
        }
    }

    while (indent > 0)
    {
        if(tokens->content[0].type == TOK_END)
        {
            indent--;
            continue;
        }
        ExpressionArray_Push(&expr.e.Program.expressions, Parse_Tokens(tokens));
    }

    TokenArray_Shift(tokens);

    return expr;
}

Expression Parse_Conditional(TokenArray* tokens, ConditionalType type)
{
    Expression expr;
    expr.type = EXPR_Conditional;
    expr.e.Conditional.type = type;
    switch(type)
    {
        case CONDITIONAL_If:
            expr.e.Conditional.expr = malloc(sizeof(Expression));
            *expr.e.Conditional.expr = Parse_Tokens(tokens);
        break;
    }
    expr.e.Conditional.program = malloc(sizeof(Expression));
    
    TokenArray_Shift(tokens);

    *expr.e.Conditional.program = Parse_CodeBlock(tokens);
    return expr;
}

Expression Parse_Tokens(TokenArray* tokens)
{
    Token tok = TokenArray_Shift(tokens);
    
    switch(tok.type)
    {
        case TOK_VAR:
            return Parse_DefVar(tokens);
        break;
        case TOK_SET:
            return Parse_SetVar(tokens);
        break;
        case TOK_IF:
            return Parse_Conditional(tokens, CONDITIONAL_If);
        break;
        case TOK_ID:
            switch(tokens->content[0].type)
            {
                case TOK_OPEN_PARENTH:
                    TokenArray_Shift(tokens);
                    return Parse_FunctionCall(tok.value, tokens);
                break;
                default:
                    return Parse_Literal(tok);
                break;
            }
        break;
        case TOK_NUM:
            return Parse_Literal(tok);
        break;
        case TOK_DO:
            return Parse_CodeBlock(tokens);
        break;
        case TOK_FUNCTION:
            //fun ID(arg) do arg end
            return Parse_DefFunction(tokens);
        break;
        default:
            printf("TODO: Not implemented (%d : %s at line %d)!\n", tok.type, tok.value, tok.line);
            exit(1);
        break;
    }
    return (Expression){};
}

Expression Parse_AST(TokenArray tokens)
{
    Expression ast = Expr_Program();
    while(tokens.size > 0)
    {
        ExpressionArray_Push(&ast.e.Program.expressions, Parse_Tokens(&tokens));
    }
    return ast;
}