#include "parser.h"
#include "common.h"
#include "lexer.h"
#include "x86.h"

#include <string.h>

enum operand_type { OPERAND_REGISTER, OPERAND_MEMORY, OPERAND_IMMEDIATE };

enum immediate_size {
    IMMEDIATE_SIZE_8BIT,
    IMMEDIATE_SIZE_16BIT,
    IMMEDIATE_SIZE_32BIT,
    IMMEIDATE_SIZE_64BIT
};

struct memory {
    struct register_entry register_entry;
    uint32_t displacement;
};

struct operand {
    enum operand_type type;
    union {
        struct register_entry register_entry;
        struct memory memory;
        uint64_t immediate;
    } value;
};

enum immediate_size find_immediate_size(uint64_t immediate) {
    if (immediate < 0x10000) {
        if (immediate < 0x100)
            return IMMEDIATE_SIZE_8BIT;
        else
            return IMMEDIATE_SIZE_16BIT;
    } else {
        if (immediate < 0x100000000L)
            return IMMEDIATE_SIZE_32BIT;
        else
            return IMMEIDATE_SIZE_64BIT;
    }
}

[[nodiscard]] _Bool
find_binary_generic_opcode_entry(char const *mnemonic,
                                 struct opcode_entry *opcode) {
    for (size_t i = 0; i < ARRAY_SIZE(OPCODE_BINARY_GENERIC_TABLE); i++) {
        struct opcode_entry entry = OPCODE_BINARY_GENERIC_TABLE[i];

        if (entry.mnemonic == NULL)
            return 0;

        if (strcmp(entry.mnemonic, mnemonic) == 0) {
            *opcode = entry;
            return 1;
        }
    }

    return 0;
}

_Bool find_register_entry(char const *register_name,
                          struct register_entry *register_entry) {
    for (size_t i = 0; i < ARRAY_SIZE(_32BIT_REGISTERS); i++) {
        struct register_entry entry = _32BIT_REGISTERS[i];

        if (entry.register_name == NULL)
            return 0;

        if (strcmp(entry.register_name, register_name) == 0) {
            *register_entry = entry;
            return 1;
        }
    }

    return 0;
}

_Bool parse_token(struct jasm_token **tokens, enum jasm_token_type type) {
    if ((*tokens)->type == type) {
        (void)jasm_tokens_pop(tokens);
        return 1;
    }

    return 0;
}

_Bool parse_operand(struct jasm_token **tokens, struct operand *operand) {
    if ((*tokens)->type == JASM_TOKEN_IDENTIFIER) {
        struct jasm_token *identifier_token = jasm_tokens_pop(tokens);
        struct register_entry entry;

        if (!find_register_entry(identifier_token->value.identifier, &entry))
            return 0;

        operand->type                 = OPERAND_REGISTER;
        operand->value.register_entry = entry;

        return 1;
    } else if ((*tokens)->type == JASM_TOKEN_INTEGER) {
        struct jasm_token *integer_token = jasm_tokens_pop(tokens);

        operand->type            = OPERAND_IMMEDIATE;
        operand->value.immediate = integer_token->value.integer;

        return 1;
    }

    return 0;
}

_Bool parse_binary_instruction(struct jasm_token **tokens,
                               struct x86_instruction *instruction) {
    if (*tokens == NULL)
        return 0;

    if ((*tokens)->type != JASM_TOKEN_IDENTIFIER)
        return 0;

    struct jasm_token *identifier_token = jasm_tokens_pop(tokens);
    struct opcode_entry opcode_entry;

    if (!find_binary_generic_opcode_entry(identifier_token->value.identifier,
                                          &opcode_entry))
        return 0;

    instruction->has_modrm   = 1;
    instruction->opcode_size = 1;

    struct operand first_operand  = {0};
    struct operand second_operand = {0};

    if (!parse_operand(tokens, &first_operand))
        return 0;

    if (first_operand.type == OPERAND_IMMEDIATE) {
        fatalf("error: first operand cannot be an immediate value\n");
    }

    if (!parse_token(tokens, JASM_TOKEN_COMMA))
        return 0;

    if (!parse_operand(tokens, &second_operand))
        return 0;

    if (second_operand.type == OPERAND_IMMEDIATE) {
        instruction->opcode[0] = opcode_entry.opcode_ev_iz;
        uint8_t mod            = 0;

        if (first_operand.type == OPERAND_REGISTER) {
            mod = MOD_REGISTER_ADDRESSING;
        }

        instruction->modrm = modrm_tobyte(
            &(struct modrm){mod, opcode_entry.modrm_opcode,
                            first_operand.value.register_entry.register_code});

        instruction->immediate_size = 4;
        memcpy(instruction->immediate,
               (uint8_t *)&second_operand.value.immediate, 4);
    } else if (second_operand.type == OPERAND_REGISTER) {
        instruction->opcode[0] = encode_generic_opcode(
            opcode_entry.opcode_eb_gb, DIRECTION_REGISTER_DESTINATION,
            SIZE_16BIT_32BIT_REG_MEM);

        instruction->modrm = modrm_tobyte(
            &(struct modrm){MOD_REGISTER_ADDRESSING,
                            first_operand.value.register_entry.register_code,
                            second_operand.value.register_entry.register_code});
    }

    return 1;
}