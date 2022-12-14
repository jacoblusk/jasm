#include <string.h>

#include "common.h"
#include "x86.h"

uint8_t encode_generic_opcode(uint8_t opcode, enum direction_flag d,
                              enum size_flag s) {
    return opcode | s | d << 1;
}

uint8_t sib_tobyte(struct sib const *sib) {
    return sib->base | sib->index << 3 | sib->scale << 6;
}

uint8_t modrm_tobyte(struct modrm const *modrm) {
    return modrm->r_m | (modrm->reg_or_opcode << 3) | (modrm->mod << 6);
}

uint8_t rex_prefix_tobyte(struct rex_prefix const *rex_prefix) {
    return REX_PREFIX_CODE << 4 | rex_prefix->W << 3 | rex_prefix->R << 2 |
           rex_prefix->X << 1 | rex_prefix->B;
}

void encode_instruction(struct x86_instruction const *in,
                        uint8_t *instruction_bytes,
                        size_t *instruction_length) {
    size_t current_instruction_byte = 0;

    *instruction_length = in->prefix_size + in->opcode_size + in->has_modrm +
                          in->has_sib + in->displacement_size +
                          in->immediate_size;

    if (in->prefix_size > 0) {
        memcpy(instruction_bytes, in->prefix, in->prefix_size);
        current_instruction_byte += in->prefix_size;
    }

    if (in->opcode_size > 0) {
        memcpy(instruction_bytes + current_instruction_byte, in->opcode,
               in->opcode_size);
        current_instruction_byte += in->opcode_size;
    }

    if (in->has_modrm) {
        instruction_bytes[current_instruction_byte++] = in->modrm;
    }

    if (in->has_sib) {
        instruction_bytes[current_instruction_byte++] = in->sib;
    }

    if (in->displacement_size > 0) {
        memcpy(instruction_bytes + current_instruction_byte, in->displacement,
               in->displacement_size);
        current_instruction_byte += in->displacement_size;
    }

    if (in->immediate_size > 0) {
        memcpy(instruction_bytes + current_instruction_byte, in->immediate,
               in->immediate_size);
        current_instruction_byte += in->immediate_size;
    }
}