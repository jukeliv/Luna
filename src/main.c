#include <stdio.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"
#include "optimizer.h"
#include "interpreter.h"

#define DEBUG_PRINT 0

const char* read_file(const char* path)
{
    FILE* fp = fopen(path, "rb");

    fseek(fp, 0, SEEK_END);
    size_t fs = ftell(fp);
    rewind(fp);

    char* buf = malloc(fs+1);
    if(fread(buf, sizeof(char), fs, fp) != fs)
    {
        printf("ERROR: Coudln't read file %s\n", path);
        return NULL;
    }

    buf[fs] = 0;
    fclose(fp);
    
    return buf;
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
    
    TokenArray tokens = Tokenize(read_file(*argv++));

    //Preprocessor for tokens
    #if DEBUG_PRINT
    printf("Preprocessor pass\n");
    #endif

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
                const char* file = read_file(tokens.content[i+1].value);

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

    printf("File tokenized\n");

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
    #if DEBUG_PRINT
    printf("AST Generated\n");
    #endif

    TokenArray_Free(&tokens);

    #if DEBUG_PRINT
    printf("No longer used tokens released\n");
    #endif

    Optimize_AST(&ast);

    #if DEBUG_PRINT
    printf("AST Optimized\n");
    #endif

    #if DEBUG_PRINT
    printf("AST Size: %d\n", ast.e.Program.expressions.size);
    #endif

    Initialize_Luna();
    #if DEBUG_PRINT
    printf("Initialized variables\n");
    #endif

    #if DEBUG_PRINT
    clock_t start = clock();
    #endif
    
    Interpret_Program(ast);

    #if DEBUG_PRINT
    printf("Interpreted successfully\n");
    printf("Time it took: %lu ms", clock()-start);
    #endif
    return 0;
}