#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>

#include "lexer.h"

#define UNUSED(X) ((void)X)

void PrintToken(PTOKEN pToken, void *pUserData) {
	UNUSED(pUserData);

	switch(pToken->tokenType) {
		case TOKEN_TYPE_COMMA:
			printf("comma: %c\n", pToken->tokenValue.cCharacter);
			break;
		case TOKEN_TYPE_IDENTIFIER:
			printf("identifier: %s\n", pToken->tokenValue.pszString);
			break;
		case TOKEN_TYPE_INTEGER:
			printf("integer: %d\n", pToken->tokenValue.iInteger);
			break;
		default:
			printf("unknown: %d\n", pToken->tokenType);
	}
}

void DestroyTokens(PTOKEN pToken, void *pUserData) {
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

int main(int argc, char **argv) {
	UNUSED(argc), UNUSED(argv);

	LEXER lexer;
	char const *pszBuffer = "imul rax, 2";

	LexerInitialize(&lexer, pszBuffer, strlen(pszBuffer));
	PTOKEN pTokens = LexerRun(&lexer);
	TokenReverse(&pTokens);
	TokenMapFn(pTokens, PrintToken, NULL);

	TokenMapFn(pTokens, DestroyTokens, NULL);

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
	return EXIT_SUCCESS;
}
