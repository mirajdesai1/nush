#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "svec.h"

svec*
make_svec() {
    svec* sv = malloc(sizeof(svec));
    sv->data = malloc(4 * sizeof(char*));
    sv->size = 0;
    sv->capacity = 4; 
    return sv;
}

void
free_svec(svec* sv) {
    for (int i = 0; i < sv->size; i++) {
        free(sv->data[i]);
    }
    free(sv->data);
    free(sv); 
}

char*
svec_get(svec* sv, int ii) {
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}

void
svec_put(svec* sv, int ii, char* item) {
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] = strdup(item);
}

void
svec_push_back(svec* sv, char* item) {
    int ii = sv->size;
    
    if (ii >= sv->capacity) {
        sv->capacity *= 2;
        sv->data = realloc(sv->data, sizeof(char*) * sv->capacity);
    }

    sv->size = ii + 1;
    svec_put(sv, ii, item);
}

void
svec_swap(svec* sv, int ii, int jj) {
    assert(ii >= 0 && ii < sv->size);
    assert(jj >= 0 && jj < sv->size);

    if (ii != jj) {
        char* temp = sv->data[ii];
        sv->data[ii] = sv->data[jj];
        sv->data[jj] = temp;
    }
}

int
svec_contains(svec* sv, char* item) {
    if (sv) {
        for (int i = 0; i < sv->size; i++) {
            char* item1 = svec_get(sv, i);
            if (strcmp(item, item1) == 0) {
                return 1;
            }
        }
    }
    return 0;
}
