#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

nush_ast*
make_ast_value(svec* cmd) {
    nush_ast* ast = malloc(sizeof(nush_ast));
    ast->value = cmd;
    ast->arg0 = 0;
    ast->arg1 = 0;
    return ast;
}

nush_ast*
make_ast_op(svec* op, nush_ast* a0, nush_ast* a1) {
    nush_ast* ast = malloc(sizeof(nush_ast));
    ast->value = op;
    ast->arg0 = a0;
    ast->arg1 = a1;
    return ast;
}

int
isOp(const char* text) {
    return strcmp(text, ">") == 0 || strcmp(text, "<") == 0 || 
        strcmp(text, "|") == 0 || strcmp(text, "||") == 0 || 
        strcmp(text, "&") == 0 || strcmp(text, "&&") == 0 ||
        strcmp(text, ";") == 0 || strcmp(text, "=") == 0;
}

nush_ast*
parse_tokens(svec* tokens) {
    if (!(svec_contains(tokens, "<") || svec_contains(tokens, ">") || svec_contains(tokens, "|") ||
                svec_contains(tokens, "&") || svec_contains(tokens, "||") ||
                svec_contains(tokens, "&&") || svec_contains(tokens, ";") || 
                svec_contains(tokens, "="))) {
        return make_ast_value(tokens);
    }

    
    svec* left = make_svec();
    svec* right = make_svec();
    svec* op = make_svec();

    int i = 0;

    if (svec_contains(tokens, ";")) {
        while (strcmp(svec_get(tokens, i), ";") != 0) {
            svec_push_back(left, svec_get(tokens, i));
            i++;
        }
        svec_push_back(op, svec_get(tokens, i));
        i++;
        while (i < tokens->size) {
            svec_push_back(right, svec_get(tokens, i));
            i++;
        }
        return make_ast_op(op, parse_tokens(left), parse_tokens(right));
    }

    while (!(isOp(svec_get(tokens, i)))) {
        svec_push_back(left, svec_get(tokens, i));
        i++;
    }

    svec_push_back(op, svec_get(tokens, i));
    i++;

    while (i < tokens->size) {
        svec_push_back(right, svec_get(tokens, i));
        i++;
    }

    return make_ast_op(op, parse_tokens(left), parse_tokens(right));
      
}

void
free_ast(nush_ast* ast) {
    if (ast) {
        free_svec(ast->value);
        free_ast(ast->arg0);
        free_ast(ast->arg1);
        free(ast);
    }
}

void
print_ast(nush_ast* ast) {
    if (ast) {
        for (int i = 0; i < ast->value->size; i++) {
            printf("%s ", svec_get(ast->value, i));
        }
        printf("\n");
        print_ast(ast->arg0);
        print_ast(ast->arg1);
    }
}

