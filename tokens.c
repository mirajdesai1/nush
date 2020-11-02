#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "tokens.h"
#include "svec.h"

int
isOperator(const char text) {
    switch (text) {
        case '>':
        case '<':
        case '|':
        case '&':
        case ';':
        case '=':
            return 1;
        default:
            return 0;
    }
}

char*
readCommand(const char* text, int ii) {
    int nn = 0;
    while (!isOperator(text[ii + nn]) && !isspace(text[ii + nn])) {
        nn++;
    }

    char* command = calloc(1, nn + 1);
    memcpy(command, text + ii, nn);
    return command;
}

svec*
tokenize(const char* text) {
    svec* toks = make_svec();

    int nn = strlen(text);
    int ii = 0;

    while (ii < nn) {
        if (isspace(text[ii])) {
            ii++;
            continue;
        }
        
        if (! isOperator(text[ii])) {
            char* command = readCommand(text, ii);
            svec_push_back(toks, command);
            ii += strlen(command);
            free(command);
            continue;
        }
        
        char op[3];
        op[0] = text[ii];
        op[1] = 0;
        if (ii < nn - 1) {
            if (isOperator(text[ii + 1])) {
                ii++;
                op[1] = text[ii];
            }
        }
        svec_push_back(toks, op);
        ++ii;
    }
    return toks;
}