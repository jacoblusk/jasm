#ifndef X86_INCLUDE_GUARD
#define X86_INCLUDE_GUARD

#include <stdint.h>

struct opcode_entry {
    char *mnemonic;
    uint8_t opcode_eb_gb;
    uint8_t opcode_ev_iz;
    uint8_t modrm_opcode;
};

enum mod : uint8_t {
    MOD_REG_INDIRECT_OR_SIB_NO_DISP_OR_DISP_ONLY   = 0b00,
    MOD_REG_INDIRECT_ONE_BYTE_SIGNED_DISPLACEMENT  = 0b01,
    MOD_REG_INDIRECT_FOUR_BYTE_SIGNED_DISPLACEMENT = 0b10,
    MOD_REGISTER_ADDRESSING                        = 0b11
};

enum sib_scale : uint8_t {
    SIB_SCALE_1 = 0b00,
    SIB_SCALE_2 = 0b01,
    SIB_SCALE_4 = 0b10,
    SIB_SCALE_8 = 0b11
};

enum register_code : uint8_t {
    REGISTER_CODE_EAX         = 0b000,
    REGISTER_CODE_ECX         = 0b001,
    REGISTER_CODE_EDX         = 0b010,
    REGISTER_CODE_EBX         = 0b011,
    REGISTER_CODE_ESP_OR_SIB  = 0b100,
    REGISTER_CODE_EBP_OR_DISP = 0b101,
    REGISTER_CODE_ESI         = 0b110,
    REGISTER_CODE_EDI         = 0b111
};

struct register_entry {
    char *register_name;
    enum register_code register_code;
    _Bool _64bit;
    _Bool extended;
};

#define REX_PREFIX_CODE 0b0100

struct rex_prefix {
    _Bool W;
    _Bool R;
    _Bool X;
    _Bool B;
};

struct modrm {
    enum mod mod;
    uint8_t reg_or_opcode;
    uint8_t r_m;
};

struct sib {
    enum sib_scale scale;
    enum register_code index;
    enum register_code base;
};

// 2.1 INSTRUCTION FORMAT FOR PROTECTED MODE, REAL-ADDRESS MODE, AND
// VIRTUAL-8086 MODE Intel® 64 and IA-32 Architectures Software Developer’s
// Manual

enum size_flag { SIZE_8BIT_REG_MEM = 0, SIZE_16BIT_32BIT_REG_MEM = 1 };

enum direction_flag {
    DIRECTION_REGISTER_SOURCE      = 0,
    DIRECTION_REGISTER_DESTINATION = 1,
};

struct x86_instruction {
    size_t prefix_size;
    uint8_t prefix[3];

    size_t opcode_size;
    uint8_t opcode[3];

    uint8_t modrm;
    _Bool has_modrm;

    uint8_t sib;
    _Bool has_sib;

    size_t displacement_size;
    uint8_t displacement[4];

    size_t immediate_size;
    uint8_t immediate[4];
};

[[maybe_unused]] static struct opcode_entry const
    OPCODE_BINARY_GENERIC_TABLE[] = {{.mnemonic     = "add",
                                      .opcode_eb_gb = 0x0,
                                      .opcode_ev_iz = 0x81,
                                      .modrm_opcode = 0b000},
                                     {.mnemonic     = "sub",
                                      .opcode_eb_gb = 0x28,
                                      .opcode_ev_iz = 0x81,
                                      .modrm_opcode = 0b101},
                                     {.mnemonic     = "mov",
                                      .opcode_eb_gb = 0x88,
                                      .opcode_ev_iz = 0xC7,
                                      .modrm_opcode = 0b000},
                                     {NULL}};

[[maybe_unused]] static struct register_entry const _64BIT_REGISTERS[] = {
    {"rax", REGISTER_CODE_EAX, 1},
    {"rbx", REGISTER_CODE_EBX, 1},
    {"rcx", REGISTER_CODE_ECX, 1},
    {"rdx", REGISTER_CODE_EDX, 1},
    {"rsp", REGISTER_CODE_ESP_OR_SIB, 1},
    {"rbp", REGISTER_CODE_EBP_OR_DISP, 1},
    {"rsi", REGISTER_CODE_ESI, 1},
    {"rdi", REGISTER_CODE_EDI, 1},

    {"r8", REGISTER_CODE_EAX, 1, 1},
    {"r9", REGISTER_CODE_EBX, 1, 1},
    {"r10", REGISTER_CODE_ECX, 1, 1},
    {"r12", REGISTER_CODE_EDX, 1, 1},
    {"r13", REGISTER_CODE_ESP_OR_SIB, 1, 1},
    {"r14", REGISTER_CODE_EBP_OR_DISP, 1, 1},
    {"r15", REGISTER_CODE_ESI, 1, 1},
    {"r16", REGISTER_CODE_EDI, 1, 1},
    {NULL},
};

[[maybe_unused]] static struct register_entry const _32BIT_REGISTERS[] = {
    {"eax", REGISTER_CODE_EAX, 1},
    {"ebx", REGISTER_CODE_EBX, 1},
    {"ecx", REGISTER_CODE_ECX, 1},
    {"edx", REGISTER_CODE_EDX, 1},
    {"esp", REGISTER_CODE_ESP_OR_SIB, 1},
    {"ebp", REGISTER_CODE_EBP_OR_DISP, 1},
    {"esi", REGISTER_CODE_ESI, 1},
    {"edi", REGISTER_CODE_EDI, 1},
    {NULL},
};

uint8_t sib_tobyte(struct sib const *sib);
uint8_t modrm_tobyte(struct modrm const *modrm);
uint8_t rex_prefix_tobyte(struct rex_prefix const *rex_prefix);
void encode_instruction(struct x86_instruction const *in,
                        uint8_t *instruction_bytes, size_t *instruction_length);
uint8_t encode_generic_opcode(uint8_t opcode, enum direction_flag d,
                              enum size_flag s);
#endif