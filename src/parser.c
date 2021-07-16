#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"
#include "lexer.h"
#include "parser.h"
#include "utility.h"

void Parser_Initialize(PPARSER pParser, PTOKEN pTokens) {
    pParser->pTokens = pTokens;
    pParser->cInstructions = 0;

    HashMap_Initialize(&pParser->hmLabels, 64, NULL, NULL);
}

PTOKEN Parser_Peek(PPARSER pParser) { return pParser->pTokens->pNext; }

PTOKEN Parser_MatchIdentifier(PPARSER pParser, char const *pszMatch) {
    if (pParser->pTokens->tokenType == TOKEN_TYPE_IDENTIFIER &&
        strcmp(pParser->pTokens->tokenValue.pszString, pszMatch) == 0) {
        PTOKEN pCurrent = pParser->pTokens;
        pParser->pTokens = pCurrent->pNext;
        return pCurrent;
    }

    return NULL;
}

PTOKEN Parser_MatchCharacter(PPARSER pParser, char c) {
    if (pParser->pTokens->tokenValue.cCharacter == c) {
        PTOKEN pCurrent = pParser->pTokens;
        pParser->pTokens = pCurrent->pNext;
        return pCurrent;
    }

    return NULL;
}

_Noreturn void Parser_Error(char const *pszErrorMessage, ...) {
    va_list vArgs;
    va_start(vArgs, pszErrorMessage);
    vfprintf(stderr, pszErrorMessage, vArgs);
    exit(EXIT_FAILURE);
}

PTOKEN Parser_MatchTokenType(PPARSER pParser, TOKEN_TYPE tokenType) {
    if (pParser->pTokens->tokenType == tokenType) {
        PTOKEN pCurrent = pParser->pTokens;
        pParser->pTokens = pCurrent->pNext;
        return pCurrent;
    }

    return NULL;
}

PTOKEN Parser_MatchV(PPARSER pParser, size_t cCount, size_t *pcMatchCount,
                     ...) {
    PTOKEN pMatchedTokens = NULL;
    PTOKEN pToken, pCurrent;
    va_list vArgs;
    size_t i;

    PARSER parserCopy;
    memcpy(&parserCopy, pParser, sizeof(PARSER));

    va_start(vArgs, pcMatchCount);

    for (pCurrent = parserCopy.pTokens, i = 0; pCurrent && (i <= cCount);
         i++, pCurrent = pCurrent->pNext) {
        TOKEN_TYPE tokenType = va_arg(vArgs, TOKEN_TYPE);
        pToken = Parser_MatchTokenType(&parserCopy, tokenType);
        if (!pToken) {
            break;
        }

        PTOKEN pTokenCopy = CopyToken(pToken);
        pTokenCopy->pNext = NULL;
        TokenList_Prepend(&pMatchedTokens, pTokenCopy);
    }

    *pcMatchCount = i;
    if (i == cCount) {
        memcpy(pParser, &parserCopy, sizeof(PARSER));
    }

    TokenList_Reverse(&pMatchedTokens);
    va_end(vArgs);
    return pMatchedTokens;
}

PLABEL Parser_ParseLabel(PPARSER pParser) {
    size_t cMatches;
    PTOKEN pMatchTokens = Parser_MatchV(
        pParser, 2, &cMatches, TOKEN_TYPE_IDENTIFIER, TOKEN_TYPE_COLON);

    if (cMatches != 2) {
        TokenList_Destroy(pMatchTokens);
        return NULL;
    }

    PLABEL pLabel = malloc(sizeof(LABEL));
    pLabel->pszLabelName = strdup(pMatchTokens->tokenValue.pszString);
    pLabel->uLocation = pParser->cInstructions;

    HashMap_Put(&pParser->hmLabels, pLabel->pszLabelName, pLabel);
    TokenList_Destroy(pMatchTokens);
    return pLabel;
}
_Bool Parser_ParseInstruction(PPARSER pParser) { return FALSE; }

_Bool Parser_ParseProgram(PPARSER pParser) { return FALSE; }

_Bool Parser_ParseExpression(PPARSER pParser) { return FALSE; }