#include <stdio.h>
#include <time.h>
#include <utils.h>
#include <lexer.h>
#include <parser.h>
#include <optimizer.h>
#include <interpreter.h>

#define DEBUG_PRINT 1

long ARGS[10] = {0};

void debug_print(const char* msg, unsigned int argc)
{
    #if DEBUG_PRINT
    printf("DEBUG: %s ", msg);
    for(unsigned int i = 0; i < argc; ++i)
    {
        printf("%lu ", ARGS[i]);
    }
    putchar('\n');
    #else
    UNUSED(msg);
    UNUSED(argc);
    #endif
}

int main(int argc, char** argv)
{
    const char* program = *argv++;
    UNUSED(program);
    UNUSED(argc);

    if(!*argv)
    {
        printf("ERROR: No input file provided!\n");
        return 1;
    }
    
    TokenArray tokens = Tokenize(ReadFile(*argv++));

    debug_print("File tokenized", 0);

    //Preprocessor for tokens
    debug_print("Preprocessor pass", 0);
    
    for(unsigned int i = 0; i < tokens.size; ++i)
    {
        Token tok = tokens.content[i];
        switch(tok.type)
        {
            case TOK_LOAD:
            {
                if(tokens.content[i+1].type != TOK_STR)
                {
                    printf("ERROR: Load preprocessor takes an string as input to search file!\n");
                    exit(1);
                }
                const char* file = ReadFile(tokens.content[i+1].value);

                TokenArray_Drop(&tokens, i);
                TokenArray_Drop(&tokens, i);
                
                tokens = TokenArray_Glue(tokens, Tokenize(file), i);
            }
            break;
            default:
            
            break;
        }
    }

    #if DEBUG_PRINT
    for(unsigned int i = 0; i < tokens.size; ++i)
    {
        Token tok = tokens.content[i];
        switch(tok.type)
        {
            case TOK_NUM:
                printf("%s ", tok.value);
            break;
            case TOK_ID:
                printf("%s ", tok.value);
            break;
            case TOK_VAR:
                printf("var ");
            break;
            case TOK_DO:
                printf("do\n");
            break;
            case TOK_END:
                printf("\nend\n");
            break;
            case TOK_IF:
                printf("if ");
            break;
            case TOK_ELSE:
                printf("if ");
            break;
            case TOK_SET:
                printf("set ");
            break;
            case TOK_OPEN_PARENTH:
                printf("(");
            break;
            case TOK_CLOSE_PARENTH:
                printf(")");
            break;
            case TOK_FUNCTION:
                printf("fun ");
            break;
            case TOK_ERROR:
                printf("error\n");
            break;
            case TOK_LOAD:
                printf("load ");
            break;
            case TOK_STR:
                printf("%s ", tok.value);
            break;
        }
    }
    #endif

    Expression ast = Parse_AST(tokens);
    debug_print("AST Generated", 0);
    ARGS[0] = ast.e.Program.expressions.size;
    debug_print("AST Size: ", 1);

    TokenArray_Free(&tokens);

    debug_print("No longer used tokens released", 0);

    Optimize_AST(&ast);

    debug_print("AST Optimized", 0);
    ARGS[0] = ast.e.Program.expressions.size;
    debug_print("Optimized AST Size: ", 1);

    Initialize_Luna();
    debug_print("Initialized variables", 0);

    clock_t start = clock();
    
    Interpret_Program(ast);

    ARGS[0] = clock()-start;
    debug_print("Interpretation Time: ", 1);
    return 0;
}