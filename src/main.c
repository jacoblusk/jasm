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

void PrintToken(PTOKEN pToken, void *pUserData) {
	UNUSED(pUserData);

	switch(pToken->tokenType) {
		case TOKEN_TYPE_COMMA:
		case TOKEN_TYPE_LBRACKET:
		case TOKEN_TYPE_RBRACKET:
		case TOKEN_TYPE_ASTERISK:
		case TOKEN_TYPE_PLUS:
		case TOKEN_TYPE_MINUS:
		case TOKEN_TYPE_FORWARD_SLASH:
			printf("syntax: %c\n", pToken->tokenValue.cCharacter);
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

int main(int argc, char **argv) {
	UNUSED(argc), UNUSED(argv);

	LEXER lexer;
	char const *pszBuffer = "imul [rax], 2 * 4 + 1 - 4 / 2";

	Lexer_Initialize(&lexer, pszBuffer, strlen(pszBuffer));
	{
		PTOKEN pTokens = Lexer_Run(&lexer);
		Token_MapFn(pTokens, PrintToken, NULL);
		Token_Destroy(pTokens);
	}
	Lexer_Destroy(&lexer);

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif
	return EXIT_SUCCESS;
}
