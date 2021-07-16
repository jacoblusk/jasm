#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdarg.h>

typedef enum tagX86_INSTRUCTION_TYPE {
    ADD_Eb_Gb = 0x00,
    ADD_Ev_Gv = 0x10
} X86_INSTRUCTION_TYPE;

typedef enum tagOPERAND_TYPE {
    OPERAND_TYPE_MEMORY,
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_LITERAL
} OPERAND_TYPE;

typedef struct tagOPERAND_BASE {
    OPERAND_TYPE operandType;
} OPERAND_BASE;

typedef struct tagOPERAND_MEMORY {
    OPERAND_BASE operandBase;
} OPERAND_MEMORY;

typedef struct tagSIB {

} SIB;

typedef struct tagMODRM {

} MODRM;

typedef struct tagX86_INSTRUCTION {
    char *pszMnemonic;

} X86_INSTRUCTION;

typedef struct tagLABEL {
    char *pszLabelName;
    size_t uLocation;
} LABEL, *PLABEL;

typedef struct tagPARSER {
    PTOKEN pTokens;
    HASHMAP hmLabels;
    size_t cInstructions;
} PARSER, *PPARSER;

void Parser_Initialize(PPARSER pParser, PTOKEN pTokens);
PTOKEN Parser_Peek(PPARSER pParser);
PTOKEN Parser_MatchIdentifier(PPARSER pParser, char const *pszMatch);
PTOKEN Parser_MatchCharacter(PPARSER pParser, char c);
_Noreturn void Parser_Error(char const *pszErrorMessage, ...);
PTOKEN Parser_MatchV(PPARSER pParser, size_t cCount, size_t *pcMatches, ...);
_Bool Parser_ParseExpression(PPARSER pParser);
PLABEL Parser_ParseLabel(PPARSER pParser);

#endif