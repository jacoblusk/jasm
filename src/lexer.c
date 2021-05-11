#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <stdint.h>
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

void Lexer_Initialize(PLEXER pLexer, char const *pszBuffer,
					 size_t cbBufferSize) {
	pLexer->pszBuffer = pszBuffer;
	pLexer->cbBufferSize = cbBufferSize;
	pLexer->cbPosition = 0;

	HashMap_Initialize(&pLexer->hmCharacterMap, 10, NULL, NULL);

	HashMap_Put(&pLexer->hmCharacterMap, ",", (void *)TOKEN_TYPE_COMMA);
	HashMap_Put(&pLexer->hmCharacterMap, "[", (void *)TOKEN_TYPE_LBRACKET);
	HashMap_Put(&pLexer->hmCharacterMap, "]", (void *)TOKEN_TYPE_RBRACKET);
	HashMap_Put(&pLexer->hmCharacterMap, "*", (void *)TOKEN_TYPE_ASTERISK);
	HashMap_Put(&pLexer->hmCharacterMap, "+", (void *)TOKEN_TYPE_PLUS);
	HashMap_Put(&pLexer->hmCharacterMap, "-", (void *)TOKEN_TYPE_MINUS);
	HashMap_Put(&pLexer->hmCharacterMap, "/", (void *)TOKEN_TYPE_FORWARD_SLASH);
}

void Lexer_Advance(PLEXER pLexer, size_t cBytes) {
	pLexer->cbPosition += cBytes;
}

void Lexer_Destroy(PLEXER pLexer) {
	HashMap_Destroy(&pLexer->hmCharacterMap);
}

void Token_Prepend(PPTOKEN ppToken, PTOKEN pToken) {
	if(!*ppToken) {
		pToken->pNext = NULL;
		*ppToken = pToken;
		return;
	}

	pToken->pNext = *ppToken;
	*ppToken = pToken;
}

void Token_Reverse(PPTOKEN ppToken) {
	PTOKEN pReversed = NULL;

	for(PTOKEN pCurrent = *ppToken; pCurrent;) {
		PTOKEN pTemp = pCurrent->pNext;
		Token_Prepend(&pReversed, pCurrent);
		pCurrent = pTemp;
	}

	*ppToken = pReversed;
}

void __Token_Destroy_DestroyToken(PTOKEN pToken, void *pUserData) {
	UNUSED(pUserData);

	switch(pToken->tokenType) {
		case TOKEN_TYPE_IDENTIFIER:
			free((char *)pToken->tokenValue.pszString);
			break;
		default:
			break;
	}

	free(pToken);
}

void Token_Destroy(PTOKEN pToken) {
	Token_MapFn(pToken, __Token_Destroy_DestroyToken, NULL);
}

void Token_MapFn(PTOKEN pToken, TOKEN_MAP_FN_ROUTINE pMapFn,
		void *pUserData) {
	for(PTOKEN pCurrent = pToken; pCurrent; ) {
		PTOKEN pTemp = pCurrent->pNext;
		pMapFn(pCurrent, pUserData);
		pCurrent = pTemp;
	}
}

PTOKEN Lexer_Identifier(PLEXER pLexer) {
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

	Lexer_Advance(pLexer, cLexemeLength);
	return pToken;
}

PTOKEN Lexer_Number(PLEXER pLexer) {
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

	int iResult = atoi(pszLexeme);
	free(pszLexeme);

	PTOKEN pToken = CreateToken(
		TOKEN_TYPE_INTEGER,
		(UTOKEN_VALUE) { .iInteger = iResult }
	);

	Lexer_Advance(pLexer, cLexemeLength);
	return pToken;
}

PTOKEN Lexer_Run(PLEXER pLexer) {
	PTOKEN pTokens = NULL;

	do {
		char pszC[4];
		char c = pLexer->pszBuffer[pLexer->cbPosition];
		sprintf(pszC, "%c", c);

		TOKEN_TYPE tokenType = (uintptr_t)HashMap_Get(
			&pLexer->hmCharacterMap,
			pszC
		);

		if(tokenType) {
			PTOKEN pToken = CreateToken(
				tokenType,
				(UTOKEN_VALUE) { .cCharacter = c }
			);

			Token_Prepend(&pTokens, pToken);
			Lexer_Advance(pLexer, 1);
		}
		else if(isspace(c)) {
			Lexer_Advance(pLexer, 1);
		} else if(isalpha(c)) {
			PTOKEN pToken = Lexer_Identifier(pLexer);
			Token_Prepend(&pTokens, pToken);
		} else if(isdigit(c)) {
			PTOKEN pToken = Lexer_Number(pLexer);
			Token_Prepend(&pTokens, pToken);
		}
	} while(pLexer->cbPosition < pLexer->cbBufferSize);

	Token_Reverse(&pTokens);
	return pTokens;
}

