#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef struct tagLEXER {
	char const *pszBuffer;
	size_t cbBufferSize;
	size_t cbPosition;
} LEXER, *PLEXER;

typedef enum tagTOKEN_TYPE {
	TOKEN_TYPE_IDENTIFIER,
	TOKEN_TYPE_COMMA
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

void LexerInitialize(
	PLEXER pLexer,
	char const *pszBuffer,
	size_t cbBufferSize
);

void LexerAdvance(PLEXER pLexer, size_t cBytes); 
PTOKEN LexerRun(PLEXER pLexer); 
void TokenPrepend(PPTOKEN ppToken, PTOKEN pToken); 
void TokenReverse(PPTOKEN ppToken);

typedef void (*TOKEN_MAP_FN_ROUTINE)(PTOKEN, void *);
void TokenMapFn(
	PTOKEN pToken,
	TOKEN_MAP_FN_ROUTINE pMapFn,
	void *pUserData
);

#endif
