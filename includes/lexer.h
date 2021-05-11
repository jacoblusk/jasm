#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

#include "hashmap.h"

#define UNUSED(X) ((void)X)

typedef struct tagLEXER {
	char const *pszBuffer;
	size_t cbBufferSize;
	size_t cbPosition;

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
	TOKEN_TYPE_FORWARD_SLASH
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
	PTOKEN pNext;
};

PTOKEN CreateToken(TOKEN_TYPE tokenType, UTOKEN_VALUE tokenValue);

void Lexer_Initialize(
	PLEXER pLexer,
	char const *pszBuffer,
	size_t cbBufferSize
);

void Lexer_Advance(PLEXER pLexer, size_t cBytes); 
PTOKEN Lexer_Run(PLEXER pLexer); 
PTOKEN Lexer_Identifier(PLEXER pLexer);
PTOKEN Lexer_Number(PLEXER pLexer); 
void Lexer_Destroy(PLEXER pLexer);

void Token_Prepend(PPTOKEN ppToken, PTOKEN pToken); 
void Token_Reverse(PPTOKEN ppToken);
void Token_Destroy(PTOKEN pToken);

typedef void (*TOKEN_MAP_FN_ROUTINE)(PTOKEN, void *);
void Token_MapFn(
	PTOKEN pToken,
	TOKEN_MAP_FN_ROUTINE pMapFn,
	void *pUserData
);

#endif
