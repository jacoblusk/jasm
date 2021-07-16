#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#else
#include <stdlib.h>
#endif

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "utility.h"

PTOKEN CreateToken(TOKEN_TYPE tokenType, UTOKEN_VALUE tokenValue,
                   CURSOR cursor) {
    PTOKEN pToken;

    pToken = malloc(sizeof(TOKEN));
    if (!pToken)
        return NULL;

    pToken->tokenType = tokenType;
    pToken->tokenValue = tokenValue;
    pToken->pNext = NULL;
    pToken->cursor = cursor;

    return pToken;
}

PTOKEN CopyToken(PTOKEN pToken) {
    PTOKEN pCopy = malloc(sizeof(TOKEN));
    memcpy(pCopy, pToken, sizeof(TOKEN));
    if (pToken->tokenType == TOKEN_TYPE_IDENTIFIER) {
        pCopy->tokenValue.pszString = strdup(pToken->tokenValue.pszString);
    }
    return pCopy;
}

void Lexer_Initialize(PLEXER pLexer, char const *pszBuffer,
                      size_t cbBufferSize) {
    pLexer->pszBuffer = pszBuffer;
    pLexer->cbBufferSize = cbBufferSize;
    pLexer->cbPosition = 0;

    HashMap_Initialize(&pLexer->hmCharacterMap, 10, NULL, NULL);

    TOKEN_TYPE *tokenTypes = malloc(sizeof(TOKEN_TYPE) * 15);
    tokenTypes[0] = TOKEN_TYPE_COMMA;
    tokenTypes[1] = TOKEN_TYPE_LBRACKET;
    tokenTypes[2] = TOKEN_TYPE_RBRACKET;
    tokenTypes[3] = TOKEN_TYPE_ASTERISK;
    tokenTypes[4] = TOKEN_TYPE_PLUS;
    tokenTypes[5] = TOKEN_TYPE_MINUS;
    tokenTypes[6] = TOKEN_TYPE_FORWARD_SLASH;
    tokenTypes[7] = TOKEN_TYPE_CARROT;
    tokenTypes[8] = TOKEN_TYPE_VERTICAL_BAR;
    tokenTypes[9] = TOKEN_TYPE_AND;
    tokenTypes[10] = TOKEN_TYPE_LESS_THAN;
    tokenTypes[11] = TOKEN_TYPE_GREATER_THAN;
    tokenTypes[12] = TOKEN_TYPE_LPAREN;
    tokenTypes[13] = TOKEN_TYPE_RPAREN;
    tokenTypes[14] = TOKEN_TYPE_COLON;

    HashMap_Put(&pLexer->hmCharacterMap, ",", &tokenTypes[0]);
    HashMap_Put(&pLexer->hmCharacterMap, "[", &tokenTypes[1]);
    HashMap_Put(&pLexer->hmCharacterMap, "]", &tokenTypes[2]);
    HashMap_Put(&pLexer->hmCharacterMap, "*", &tokenTypes[3]);
    HashMap_Put(&pLexer->hmCharacterMap, "+", &tokenTypes[4]);
    HashMap_Put(&pLexer->hmCharacterMap, "-", &tokenTypes[5]);
    HashMap_Put(&pLexer->hmCharacterMap, "/", &tokenTypes[6]);
    HashMap_Put(&pLexer->hmCharacterMap, "^", &tokenTypes[7]);
    HashMap_Put(&pLexer->hmCharacterMap, "|", &tokenTypes[8]);
    HashMap_Put(&pLexer->hmCharacterMap, "&", &tokenTypes[9]);
    HashMap_Put(&pLexer->hmCharacterMap, "<", &tokenTypes[10]);
    HashMap_Put(&pLexer->hmCharacterMap, ">", &tokenTypes[11]);
    HashMap_Put(&pLexer->hmCharacterMap, "(", &tokenTypes[12]);
    HashMap_Put(&pLexer->hmCharacterMap, ")", &tokenTypes[13]);
    HashMap_Put(&pLexer->hmCharacterMap, ":", &tokenTypes[14]);

    pLexer->cursor = (CURSOR){1, 1};
}

void Lexer_Advance(PLEXER pLexer, size_t cBytes) {
    pLexer->cbPosition += cBytes;
    pLexer->cursor.cColumnNumber += cBytes;
}

void Lexer_Destroy(PLEXER pLexer) {
    void *pTokenType;

    // We allocated space for the character map, need to free it here.
    HashMap_Get(&pLexer->hmCharacterMap, ",", &pTokenType);
    free(pTokenType);
    HashMap_Destroy(&pLexer->hmCharacterMap);
}

void TokenList_Prepend(PPTOKEN ppToken, PTOKEN pToken) {
    if (!*ppToken) {
        pToken->pNext = NULL;
        *ppToken = pToken;
        return;
    }

    pToken->pNext = *ppToken;
    *ppToken = pToken;
}

void TokenList_Reverse(PPTOKEN ppToken) {
    PTOKEN pReversed = NULL;

    for (PTOKEN pCurrent = *ppToken; pCurrent;) {
        PTOKEN pTemp = pCurrent->pNext;
        TokenList_Prepend(&pReversed, pCurrent);
        pCurrent = pTemp;
    }

    *ppToken = pReversed;
}

void __TokenList_Destroy_DestroyToken(PTOKEN pToken, void *pUserData) {
    UNUSED(pUserData);

    switch (pToken->tokenType) {
    case TOKEN_TYPE_IDENTIFIER:
        free((char *)pToken->tokenValue.pszString);
        break;
    default:
        break;
    }

    free(pToken);
}

void TokenList_Destroy(PTOKEN pToken) {
    TokenList_MapFn(pToken, __TokenList_Destroy_DestroyToken, NULL);
}

void TokenList_MapFn(PTOKEN pToken, TOKENLIST_MAP_FN_ROUTINE pMapFn,
                     void *pUserData) {
    for (PTOKEN pCurrent = pToken; pCurrent;) {
        PTOKEN pTemp = pCurrent->pNext;
        pMapFn(pCurrent, pUserData);
        pCurrent = pTemp;
    }
}

PTOKEN Lexer_LexIdentifier(PLEXER pLexer) {
    size_t cLexemeLength = 0;
    char c;

    do {
        cLexemeLength++;
        c = pLexer->pszBuffer[pLexer->cbPosition + cLexemeLength];
    } while (pLexer->cbBufferSize > (pLexer->cbPosition + cLexemeLength) &&
             isalpha(c));

    char *pszLexeme = malloc(sizeof(char) * cLexemeLength + 1);
    pszLexeme[cLexemeLength] = '\0';

    memcpy(pszLexeme, pLexer->pszBuffer + pLexer->cbPosition, cLexemeLength);

    PTOKEN pToken =
        CreateToken(TOKEN_TYPE_IDENTIFIER,
                    (UTOKEN_VALUE){.pszString = pszLexeme}, pLexer->cursor);

    Lexer_Advance(pLexer, cLexemeLength);
    return pToken;
}

PTOKEN Lexer_LexNumber(PLEXER pLexer) {
    size_t cLexemeLength = 0;
    char c;

    do {
        cLexemeLength++;
        c = pLexer->pszBuffer[pLexer->cbPosition + cLexemeLength];
    } while (pLexer->cbBufferSize > (pLexer->cbPosition + cLexemeLength) &&
             isdigit(c));

    char *pszLexeme = malloc(sizeof(char) * cLexemeLength + 1);
    pszLexeme[cLexemeLength] = '\0';

    memcpy(pszLexeme, pLexer->pszBuffer + pLexer->cbPosition, cLexemeLength);

    int iResult = atoi(pszLexeme);
    free(pszLexeme);

    PTOKEN pToken =
        CreateToken(TOKEN_TYPE_INTEGER, (UTOKEN_VALUE){.iInteger = iResult},
                    pLexer->cursor);

    Lexer_Advance(pLexer, cLexemeLength);
    return pToken;
}

PTOKEN Lexer_Run(PLEXER pLexer) {
    PTOKEN pTokens = NULL;

    do {
        char pszC[2];
        char c = pLexer->pszBuffer[pLexer->cbPosition];
        sprintf(pszC, "%c", c);
        void *pTokenType;

        HASHMAP_STATUS hmStatus =
            HashMap_Get(&pLexer->hmCharacterMap, pszC, &pTokenType);

        if (hmStatus == HASHMAP_STATUS_OK) {
            PTOKEN pToken =
                CreateToken(*(TOKEN_TYPE *)pTokenType,
                            (UTOKEN_VALUE){.cCharacter = c}, pLexer->cursor);

            TokenList_Prepend(&pTokens, pToken);
            Lexer_Advance(pLexer, 1);
        } else if (isspace(c)) {
            Lexer_Advance(pLexer, 1);

            if (c == '\n') {
                pLexer->cursor.cLineNumber++;
                pLexer->cursor.cColumnNumber = 1;
            }
        } else if (isalpha(c)) {
            PTOKEN pToken = Lexer_LexIdentifier(pLexer);
            TokenList_Prepend(&pTokens, pToken);
        } else if (isdigit(c)) {
            PTOKEN pToken = Lexer_LexNumber(pLexer);
            TokenList_Prepend(&pTokens, pToken);
        }

    } while (pLexer->cbPosition < pLexer->cbBufferSize);

    TokenList_Reverse(&pTokens);
    return pTokens;
}
