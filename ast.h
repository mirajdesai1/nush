#ifndef NUSH_AST_H
#define NUSH_AST_H

#include "svec.h"

typedef struct nush_ast {
    struct nush_ast* arg0;
    struct nush_ast* arg1;

    svec* value;
} nush_ast;

nush_ast* make_ast_value(svec* cmd);
nush_ast* make_ast_op(svec* op, nush_ast* a0, nush_ast* a1);
nush_ast* parse_tokens(svec* tokens);
void free_ast(nush_ast* ast);
void print_ast(nush_ast* ast);

#endif
