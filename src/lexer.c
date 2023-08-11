#include "lexer.h"

void TokenArray_Initalize(TokenArray* tokens)
{
    tokens->size = 0;
    tokens->heap = 2;
    tokens->content = malloc(tokens->heap*sizeof(Token));
}

void TokenArray_Push(TokenArray* tokens, Token token)
{
    if(tokens->heap <= tokens->size)
    {
        tokens->heap*=2;
        tokens->content = realloc(tokens->content, tokens->heap*sizeof(Token));
    }
    tokens->content[tokens->size++] = token;
}

Token TokenArray_Shift(TokenArray* tokens)
{
    assert(tokens->size > 0);
    tokens->size--;
    return *(tokens->content++);
}

Token Token_New(unsigned int line, TokenType type, const char* value)
{
    Token token;
    token.line = line;
    token.type = type;
    token.value = _strdup(value);
    return token;
}

TokenType TokenType_From_Word(const char* word)
{
    if(!strcmp(word, "var"))
    {
        return TOK_VAR;
    }
    else if(!strcmp(word, "set"))
    {
        return TOK_SET;
    }
    else if(!strcmp(word, "do"))
    {
        return TOK_DO;
    }
    else if(!strcmp(word, "end"))
    {
        return TOK_END;
    }
    else if(!strcmp(word, "if"))
    {
        return TOK_IF;
    }
    else if(!strcmp(word, "fun"))
    {
        return TOK_FUNCTION;
    }
    return TOK_ID;
}

TokenArray Tokenize(const char* source)
{
    TokenArray tokens;
    TokenArray_Initalize(&tokens);

    char* lex = calloc(256, sizeof(char));
    unsigned char li = 0;

    unsigned int line = 1;
    for(unsigned int i = 0; i < strlen(source); ++i)
    {
        for(;IsWS(source[i]);++i)
        {
            if(source[i] == '\n')
                ++line;
        }

        for(unsigned int j = 0; j < 256; ++j) lex[j] = 0;
        li = 0;

        switch(source[i])
        {
            case ';':
                for(;source[i] != '\n';++i);
            break;
            case '(':
                TokenArray_Push(&tokens, Token_New(line, TOK_OPEN_PARENTH, NULL));
            break;
            case ')':
                TokenArray_Push(&tokens, Token_New(line, TOK_CLOSE_PARENTH, NULL));
            break;
            case '-':
                lex[li++] = source[i++];
                while(source[i] == '_' || IsNum(source[i]) || source[i] == '.')
                {
                    if(source[i] == '_')
                    {
                        ++i;
                        continue;
                    }
                    lex[li++] = source[i++];
                }
                i--;
                lex[li] = 0;
                TokenArray_Push(&tokens, Token_New(line, TOK_NUM, lex));
            break;
            default:
                if(IsNum(source[i]))
                {
                    while(source[i] == '_' || IsNum(source[i]) || source[i] == '.')
                    {
                        if(source[i] == '_')
                        {
                            ++i;
                            continue;
                        }
                        lex[li++] = source[i++];
                    }
                    i--;
                    lex[li] = 0;
                    TokenArray_Push(&tokens, Token_New(line, TOK_NUM, lex));
                }
                else if(IsAlpha(source[i]))
                {
                    while(IsAlpha(source[i]) || source[i] == '_' || IsNum(source[i]))
                    {
                        lex[li++] = source[i++];
                    }
                    --i;
                    lex[li] = 0;
                    TokenArray_Push(&tokens, Token_New(line, TokenType_From_Word(lex), lex));
                }
                else
                {
                    printf("ERROR: Unknown character found in line %d\ncharacter: %c\n", line, source[i]);
                    exit(1);
                }
            break;
        }
    }
    
    return tokens;
}