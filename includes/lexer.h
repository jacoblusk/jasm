#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "hashmap.h"

typedef struct tagCURSOR {
    size_t cLineNumber;
    size_t cColumnNumber;
} CURSOR;

typedef struct tagLEXER {
    char const *pszBuffer;
    size_t cbBufferSize;
    size_t cbPosition;
    CURSOR cursor;

    HASHMAP hmCharacterMap;
} LEXER, *PLEXER;

typedef enum tagTOKEN_TYPE {
    TOKEN_TYPE_IDENTIFIER = 1,
    TOKEN_TYPE_INTEGER,
    TOKEN_TYPE_COMMA,
    TOKEN_TYPE_PLUS,
    TOKEN_TYPE_MINUS,
    TOKEN_TYPE_ASTERISK,
    TOKEN_TYPE_LBRACKET,
    TOKEN_TYPE_RBRACKET,
    TOKEN_TYPE_FORWARD_SLASH,
    TOKEN_TYPE_CARROT,
    TOKEN_TYPE_VERTICAL_BAR,
    TOKEN_TYPE_COLON,
    TOKEN_TYPE_AND,
    TOKEN_TYPE_LESS_THAN,
    TOKEN_TYPE_GREATER_THAN,
    TOKEN_TYPE_LPAREN,
    TOKEN_TYPE_RPAREN
} TOKEN_TYPE;

typedef union tagUTOKEN_VALUE {
    char const *pszString;
    int iInteger;
    char cCharacter;
} UTOKEN_VALUE;

typedef struct tagTOKEN TOKEN, *PTOKEN, **PPTOKEN;
struct tagTOKEN {
    TOKEN_TYPE tokenType;
    UTOKEN_VALUE tokenValue;
    CURSOR cursor;
    PTOKEN pNext;
};

PTOKEN CreateToken(TOKEN_TYPE tokenType, UTOKEN_VALUE tokenValue,
                   CURSOR cursor);
PTOKEN CopyToken(PTOKEN pToken);

void Lexer_Initialize(PLEXER pLexer, char const *pszBuffer,
                      size_t cbBufferSize);

void Lexer_Advance(PLEXER pLexer, size_t cBytes);
PTOKEN Lexer_Run(PLEXER pLexer);
PTOKEN Lexer_LexIdentifier(PLEXER pLexer);
PTOKEN Lexer_LexNumber(PLEXER pLexer);
void Lexer_Destroy(PLEXER pLexer);

void TokenList_Prepend(PPTOKEN ppToken, PTOKEN pToken);
void TokenList_Reverse(PPTOKEN ppToken);
void TokenList_Destroy(PTOKEN pToken);

typedef void (*TOKENLIST_MAP_FN_ROUTINE)(PTOKEN, void *);
void TokenList_MapFn(PTOKEN pToken, TOKENLIST_MAP_FN_ROUTINE pMapFn,
                     void *pUserData);

#endif
