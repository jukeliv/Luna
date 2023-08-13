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
    TOK_STR,
    TOK_DO,
    TOK_END,
    TOK_IF,
    TOK_ELSE,
    TOK_FUNCTION,
    TOK_OPEN_PARENTH,
    TOK_CLOSE_PARENTH,
    TOK_ERROR,
    // Preprocessor
    TOK_LOAD,
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
TokenArray TokenArray_Glue(TokenArray a, TokenArray b, unsigned int i);
void TokenArray_Drop(TokenArray* array, unsigned int index);
Token TokenArray_Shift(TokenArray* tokens);
void TokenArray_Free(TokenArray* tokens);

Token Token_New(unsigned int line, TokenType type, const char* value);

TokenType TokenType_From_Word(const char* word);

TokenArray Tokenize(const char* source);

#endif // UTILS_H_