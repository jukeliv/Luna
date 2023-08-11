#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

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

    if(!(*argv))
    {
        printf("ERROR: No input file provided!\n");
        return 1;
    }
    TokenArray tokens = Tokenize(read_file(*argv));
    
    #if 0
    for(unsigned int i = 0; i < tokens.size; ++i)
    {
        Token tok = tokens.content[i];
        printf("%d : %s at line %d\n", tok.type, tok.value, tok.line);
    }
    #endif

    Expression ast = Parse_AST(tokens);
    
    Initialize_Luna();
    Interpret_Program(ast);

    return 0;
}