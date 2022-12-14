#ifndef PARSER_INCLUDE_GUARD
#define PARSER_INCLUDE_GUARD

#include <stdint.h>

#include "lexer.h"
#include "x86.h"

_Bool parse_binary_instruction(struct jasm_token **tokens,
                               struct x86_instruction *instruction);
[[nodiscard]] _Bool find_binary_generic_opcode(char const *mnemonic,
                                               uint8_t *opcode);

#endif