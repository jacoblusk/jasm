#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    struct jasm_token *tokens = nullptr;

    tokens = jasm_tokenize("sub ebx, eax\n mov eax, 2\nadd edx, eax");
    jasm_tokens_print(tokens);

    /*
    struct x86_instruction instruction = {
        .opcode_size = 1,
        .opcode      = encode_generic_opcode(0, DIRECTION_REGISTER_DESTINATION,
                                             SIZE_16BIT_32BIT_REG_MEM),
        .has_modrm   = 1,
        .modrm =
            modrm_tobyte(&(struct
    modrm){MOD_REG_INDIRECT_ONE_BYTE_SIGNED_DISPLACEMENT, REGISTER_CODE_EAX,
    REGISTER_CODE_ESI}), .displacement_size = 1, .displacement      = {0xFF}};

    uint8_t instruction_bytes[16] = {0};
    size_t instruction_size;
    encode_instruction(&instruction, instruction_bytes, &instruction_size);
    */

    struct x86_instruction instruction = {0};
    uint8_t instruction_bytes[16]      = {0};
    size_t instruction_size            = 0;

    while (parse_binary_instruction(&tokens, &instruction)) {
        encode_instruction(&instruction, instruction_bytes, &instruction_size);

        for (size_t i = 0; i < instruction_size; i++) {
            printf("%02x ", instruction_bytes[i]);
        }

        memset(&instruction, 0, sizeof(instruction));
    }

    printf("\n");

    return EXIT_SUCCESS;
}
