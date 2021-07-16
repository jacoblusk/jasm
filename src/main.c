#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "utility.h"

void PrintToken(PTOKEN pToken, void *pUserData) {
    UNUSED(pUserData);

    switch (pToken->tokenType) {
    case TOKEN_TYPE_COMMA:
    case TOKEN_TYPE_LBRACKET:
    case TOKEN_TYPE_RBRACKET:
    case TOKEN_TYPE_ASTERISK:
    case TOKEN_TYPE_PLUS:
    case TOKEN_TYPE_MINUS:
    case TOKEN_TYPE_FORWARD_SLASH:
    case TOKEN_TYPE_CARROT:
    case TOKEN_TYPE_VERTICAL_BAR:
    case TOKEN_TYPE_AND:
    case TOKEN_TYPE_LESS_THAN:
    case TOKEN_TYPE_GREATER_THAN:
    case TOKEN_TYPE_LPAREN:
    case TOKEN_TYPE_RPAREN:
    case TOKEN_TYPE_COLON:
        printf("syntax (%zu, %zu): %c\n", pToken->cursor.cColumnNumber,
               pToken->cursor.cLineNumber, pToken->tokenValue.cCharacter);
        break;
    case TOKEN_TYPE_IDENTIFIER:
        printf("identifier (%zu, %zu): %s\n", pToken->cursor.cColumnNumber,
               pToken->cursor.cLineNumber, pToken->tokenValue.pszString);
        break;
    case TOKEN_TYPE_INTEGER:
        printf("integer (%zu, %zu): %d\n", pToken->cursor.cColumnNumber,
               pToken->cursor.cLineNumber, pToken->tokenValue.iInteger);
        break;
    default:
        printf("unknown (%zu, %zu): %d\n", pToken->cursor.cColumnNumber,
               pToken->cursor.cLineNumber, pToken->tokenType);
    }
}

int main(int argc, char **argv) {
    UNUSED(argc), UNUSED(argv);

    LEXER lexer;
    char const *pszBuffer = "start:\n mov rax, rbx\n";

    Lexer_Initialize(&lexer, pszBuffer, strlen(pszBuffer));
    {
        PTOKEN pTokens = Lexer_Run(&lexer);
        TokenList_MapFn(pTokens, PrintToken, NULL);

        PARSER parser;
        Parser_Initialize(&parser, pTokens);

        PLABEL pResult = Parser_ParseLabel(&parser);
        printf("%s %d\n", pResult->pszLabelName, pResult->uLocation);
        TokenList_Destroy(pTokens);
    }
    Lexer_Destroy(&lexer);

#ifdef _DEBUG
    if (_CrtDumpMemoryLeaks()) {
        printf("_CrtDumpMemoryLeaks reported a memory leak.\n");
    }
#endif
    return EXIT_SUCCESS;
}
