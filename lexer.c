#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "lexer.h"

struct jasm_token *jasm_tokens_pop(struct jasm_token **tokens) {
    struct jasm_token *head = *tokens;
    *tokens                 = head->next;

    return head;
}

void jasm_tokens_reverse(struct jasm_token **tokens) {
    struct jasm_token *curr = *tokens;
    struct jasm_token *prev = nullptr;
    struct jasm_token *next = nullptr;

    while (curr != nullptr) {
        next       = curr->next;
        curr->next = prev;

        prev = curr;
        curr = next;
    }

    *tokens = prev;
}

void jasm_token_print(struct jasm_token *token) {
    switch (token->type) {
    case JASM_TOKEN_PLUS:
    case JASM_TOKEN_MULTIPLY:
    case JASM_TOKEN_LPAREN:
    case JASM_TOKEN_RPAREN:
    case JASM_TOKEN_LBRACKET:
    case JASM_TOKEN_RBRACKET:
    case JASM_TOKEN_COMMA:
        printf("%c ", (char)token->type);
        break;
    case JASM_TOKEN_INTEGER:
        printf("i:%llu ", token->value.integer);
        break;
    case JASM_TOKEN_IDENTIFIER:
        printf("s:%s ", token->value.identifier);
    default:
        break;
    }
}

void jasm_tokens_print(struct jasm_token *tokens) {
    for (struct jasm_token *curr = tokens; curr != nullptr; curr = curr->next) {
        jasm_token_print(curr);
    }
    printf("\n");
}

void jasm_pushtoken(struct jasm_token **tokens, struct jasm_token *token) {
    token->next = *tokens;
    *tokens     = token;
}

[[nodiscard]] struct jasm_token *jasm_mktoken(enum jasm_token_type type) {
    struct jasm_token *token = malloc(sizeof(struct jasm_token));
    if (token == nullptr) {
        fatalf("fatal: malloc failed.\n");
    }

    token->next = nullptr;
    token->type = type;
    return token;
}

[[nodiscard]] int ipow(int base, int exp) {
    int result = 1;
    while (exp) {
        if (exp % 2)
            result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}

void jasm_scanint(struct jasm_token **tokens, char const **input) {
    int integer = 0;
    int length  = 0;

    while (isdigit(*(*input + length))) {
        length++;
    }

    for (int i = length - 1; i >= 0; i--) {
        int digit = (*(*input + i) - '0');
        integer += digit * ipow(10, length - i - 1);
    }

    struct jasm_token *token = jasm_mktoken(JASM_TOKEN_INTEGER);
    token->value.integer     = integer;

    jasm_pushtoken(tokens, token);
    *input += length;
}

void jasm_scanident(struct jasm_token **tokens, char const **input) {
    size_t length = 0;

    char c;
    while (isalpha(c = *(*input + length)) || isdigit(c)) {
        length++;
    }

    char *identifier = malloc(sizeof(char) * (length + 1));
    if (identifier == nullptr) {
        fatalf("fatal: malloc failed.\n");
    }

    identifier[length] = '\0';
    memcpy(identifier, *input, length);

    struct jasm_token *token = jasm_mktoken(JASM_TOKEN_IDENTIFIER);
    token->value.identifier  = identifier;

    jasm_pushtoken(tokens, token);
    *input += length;
}

[[nodiscard]] struct jasm_token *jasm_tokenize(char const *input) {
    char c;
    struct jasm_token *tokens = nullptr;

    while (*input != '\0') {
        c = *input;

        switch (c) {
        case '+':
        case '*':
        case ',':
        case '(':
        case ')':
        case ']':
        case '[': {
            jasm_pushtoken(&tokens, jasm_mktoken((enum jasm_token_type)c));
            input++;
            continue;
        }
        }

        if (isspace(c)) {
            input++;
            continue;
        }

        else if (isdigit(c)) {
            jasm_scanint(&tokens, &input);
        } else if (isalpha(c)) {
            jasm_scanident(&tokens, &input);
        }
    }

    jasm_tokens_reverse(&tokens);
    return tokens;
}