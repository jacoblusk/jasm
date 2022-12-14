#ifndef LEXER_INCLUDE_GUARD
#define LEXER_INCLUDE_GUARD

#include <stdint.h>

enum jasm_token_type : uint_fast8_t {
    JASM_TOKEN_IDENTIFIER,
    JASM_TOKEN_INTEGER,
    JASM_TOKEN_LBRACKET = '[',
    JASM_TOKEN_RBRACKET = ']',
    JASM_TOKEN_LPAREN   = '(',
    JASM_TOKEN_RPAREN   = ')',
    JASM_TOKEN_COMMA    = ',',
    JASM_TOKEN_PLUS     = '+',
    JASM_TOKEN_MULTIPLY = '*'
};

struct jasm_token;
struct jasm_token {
    enum jasm_token_type type;
    struct jasm_token *next;
    union {
        char *identifier;
        uint64_t integer;
    } value;
};

struct jasm_token *jasm_tokens_pop(struct jasm_token **tokens);
void jasm_tokens_reverse(struct jasm_token **tokens);
void jasm_token_print(struct jasm_token *token);
void jasm_tokens_print(struct jasm_token *tokens);
void jasm_pushtoken(struct jasm_token **tokens, struct jasm_token *token);
[[nodiscard]] struct jasm_token *jasm_mktoken(enum jasm_token_type type);
void jasm_scanint(struct jasm_token **tokens, char const **input);
void jasm_scanident(struct jasm_token **tokens, char const **input);
[[nodiscard]] struct jasm_token *jasm_tokenize(char const *input);

#endif