#ifndef LEXER_H_
#define LEXER_H_

#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef enum TokenType
{
    TOK_VAR,
    TOK_SET,
    TOK_ID,
    TOK_NUM,
    TOK_DO,
    TOK_END,
    TOK_IF,
    TOK_FUNCTION,
    TOK_OPEN_PARENTH,
    TOK_CLOSE_PARENTH
}TokenType;

typedef struct Token
{
    TokenType type;
    const char* value;
    unsigned int line;
}Token;

typedef struct TokenArray
{
    unsigned int size;
    unsigned int heap;
    Token* content;
}TokenArray;

void TokenArray_Initalize(TokenArray* tokens);
void TokenArray_Push(TokenArray* tokens, Token token);
Token TokenArray_Shift(TokenArray* tokens);

Token Token_New(unsigned int line, TokenType type, const char* value);

TokenType TokenType_From_Word(const char* word);

TokenArray Tokenize(const char* source);

#endif // UTILS_H_