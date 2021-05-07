#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

void PrintToken(PTOKEN pToken, void *pUserData) {
	switch(pToken->tokenType) {
	case TOKEN_TYPE_COMMA:
		printf("comma: %c\n", pToken->tokenValue.cCharacter);
		break;
	case TOKEN_TYPE_IDENTIFIER:
		printf("identifier: %s\n", pToken->tokenValue.pszString);
		break;
	default:
		printf("unknown: %d\n", pToken->tokenType);
	}
}

int main(int argc, char **argv) {
	LEXER lexer;
	char const *pszBuffer = "mov rax, rbx";

	LexerInitialize(&lexer, pszBuffer, strlen(pszBuffer));
	PTOKEN pTokens = LexerRun(&lexer);
	TokenReverse(&pTokens);
	TokenMapFn(pTokens, PrintToken, NULL);

	return EXIT_SUCCESS;
}
