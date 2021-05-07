#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

PTOKEN CreateToken(TOKEN_TYPE tokenType, UTOKEN_VALUE tokenValue) {
	PTOKEN pToken;

	pToken = malloc(sizeof(TOKEN));
	if(!pToken)
		return NULL;

	pToken->tokenType = tokenType;
	pToken->tokenValue = tokenValue;
	pToken->pNext = NULL;

	return pToken;
}

void LexerInitialize(PLEXER pLexer, char const *pszBuffer,
					 size_t cbBufferSize) {
	pLexer->pszBuffer = pszBuffer;
	pLexer->cbBufferSize = cbBufferSize;
	pLexer->cbPosition = 0;
}

void LexerAdvance(PLEXER pLexer, size_t cBytes) {
	pLexer->cbPosition += cBytes;
}

void TokenPrepend(PPTOKEN ppToken, PTOKEN pToken) {
	if(!*ppToken) {
		pToken->pNext = NULL;
		*ppToken = pToken;
		return;
	}

	pToken->pNext = *ppToken;
	*ppToken = pToken;
}

void TokenReverse(PPTOKEN ppToken) {
	PTOKEN pReversed = NULL;

	for(PTOKEN pCurrent = *ppToken; pCurrent;) {
		PTOKEN pTemp = pCurrent->pNext;
		TokenPrepend(&pReversed, pCurrent);
		pCurrent = pTemp;
	}

	*ppToken = pReversed;
}

typedef void (*TOKEN_MAP_FN_ROUTINE)(PTOKEN, void *);
void TokenMapFn(PTOKEN pToken, TOKEN_MAP_FN_ROUTINE pMapFn,
		void *pUserData) {
	for(PTOKEN pCurrent = pToken; pCurrent; pCurrent = pCurrent->pNext) {
		pMapFn(pCurrent, pUserData);
	}
}

PTOKEN LexerIdentifier(PLEXER pLexer) {
	size_t cLexemeLength = 0;
	char c;

	do {
		cLexemeLength++;
		c = pLexer->pszBuffer[pLexer->cbPosition + cLexemeLength];
	} while(
		pLexer->cbBufferSize > (pLexer->cbPosition + cLexemeLength)
		&& isalpha(c)
	);

	char *pszLexeme = malloc(sizeof(char) * cLexemeLength + 1);
	pszLexeme[cLexemeLength] = '\0';

	memcpy(
		pszLexeme,
		pLexer->pszBuffer + pLexer->cbPosition,
		cLexemeLength
	);

	PTOKEN pToken = CreateToken(
		TOKEN_TYPE_IDENTIFIER,
		(UTOKEN_VALUE) { .pszString = pszLexeme }
	);

	LexerAdvance(pLexer, cLexemeLength);
	return pToken;
}

PTOKEN LexerNumber(PLEXER pLexer) {
	size_t cLexemeLength = 0;
	char c;

	do {
		cLexemeLength++;
		c = pLexer->pszBuffer[pLexer->cbPosition + cLexemeLength];
	} while(
		pLexer->cbBufferSize > (pLexer->cbPosition + cLexemeLength)
		&& isdigit(c)
	);

	char *pszLexeme = malloc(sizeof(char) * cLexemeLength + 1);
	pszLexeme[cLexemeLength] = '\0';

	memcpy(
		pszLexeme,
		pLexer->pszBuffer + pLexer->cbPosition,
		cLexemeLength
	);

	atoi(
	PTOKEN pToken = CreateToken(
		TOKEN_TYPE_IDENTIFIER,
		(UTOKEN_VALUE) { .pszString = pszLexeme }
	);

	LexerAdvance(pLexer, cLexemeLength);
	return pToken;
}

PTOKEN LexerRun(PLEXER pLexer) {
	PTOKEN pTokens = NULL;

	do {
		char c = pLexer->pszBuffer[pLexer->cbPosition];
		if(c == ',') {
			PTOKEN pToken = CreateToken(
				TOKEN_TYPE_COMMA,
				(UTOKEN_VALUE) { .cCharacter = c }
			);

			TokenPrepend(&pTokens, pToken);
			LexerAdvance(pLexer, 1);
		}
		else if(isspace(c)) {
			LexerAdvance(pLexer, 1);
		} else if(isalpha(c)) {
			PTOKEN pToken = LexerIdentifier(pLexer);
			TokenPrepend(&pTokens, pToken);
		}
	} while(pLexer->cbPosition < pLexer->cbBufferSize);

	return pTokens;
}

