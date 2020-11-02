#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "svec.h"
#include "tokens.h"
#include "ast.h"

void execute_ast(nush_ast* tree);

void
eval(nush_ast* tree) {
    if (tree->arg0 && tree->arg1) {
        execute_ast(tree);
    }
    else {
        svec* tokens = tree->value;
        
        char* lines[tokens->size + 1];
        
        lines[tokens->size] = 0;
        for (int i = 0; i < tokens->size; i++) {
            lines[i] = svec_get(tokens, i);
        }
        if (strcmp(lines[0], "exit") == 0) {
            exit(0);
        }
        else {
            execvp(lines[0], lines);
        }
    }
    
}

void
replace_echo(svec* cmd, svec* variables) {
    for (int i = 1; i < cmd->size; i++) {
        char* line = calloc(1, 4);
        memcpy(line, svec_get(cmd, i) + 1, 3);

        if (svec_contains(variables, line)) {
            int j = 0;
            while (j < variables->size) {
                if (strcmp(line, svec_get(variables, j)) == 0) {
                    break;
                }
                j++;
            }
            free(svec_get(cmd, i));
            svec_put(cmd, i, svec_get(variables, j+1));
        }
        free(line);
    }
}

void
execute(char* cmd, char** lines)
{
    if (strcmp(lines[0], "exit") == 0) {
        exit(0);
    }
    if (strcmp(lines[0], "cd") == 0) {
        chdir(lines[1]);
        return;
    }

    int cpid;

    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
    }
    else {
        for (int ii = 0; ii < strlen(cmd); ++ii) {
            if (cmd[ii] == ' ') {
                cmd[ii] = 0;
                break;
            }
        }

        execvp(cmd, lines);
        printf("Can't get here, exec only returns on error.");
    }
}

void
redir_in(nush_ast* tree) {
    int cpid;

    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
    } 
    else {
        int in = open(svec_get(tree->arg1->value, 0), O_RDONLY);

        close(0);
        dup(in);
        close(in);

        execute_ast(tree->arg0);
    }
}

// Redirect to out function based on cs3650 Fall 2020 video lecture 6
// Author: Nat Tuck
// URL: https://github.com/NatTuck/scratch-2020-09/blob/master/3650/v06/redir.c
void
redir_out(nush_ast* tree) {
    int cpid;

    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
    }
    else {
        int out = open(svec_get(tree->arg1->value, 0), O_CREAT | O_APPEND | O_WRONLY, 0644);

        close(1);
        dup(out);  
        close(out);

        execute_ast(tree->arg0);
    }
}

// Execute pipe function based on cs3650 Fall 2020 video lecture 6
// Author: Nat Tuck
// URL: https://github.com/NatTuck/scratch-2020-09/blob/master/3650/v06/pipe1.c
void
exec_pipe(nush_ast* tree) {
    int pipe_fds[2];
    pipe(pipe_fds);

    int p_read = pipe_fds[0];
    int p_write = pipe_fds[1];
    int cpid;

    if ((cpid = fork())) {
        close(1);
        dup(p_write);
        close(p_read);

        execute_ast(tree->arg0);

    } 
    else {
        close(0);
        dup(p_read);
        close(p_write);
        
        execute_ast(tree->arg1);
    }
}

void
exec_background(nush_ast* tree) {
    int cpid;

    if ((cpid = fork())) { 

    } 
    else {
        eval(tree->arg0);        
    }
}

void
exec_and(nush_ast* tree) {
    int cpid;

    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);

        if (status == 0) {
            eval(tree->arg1);
        }
    }
    else {
        eval(tree->arg0);
    }
}

void
exec_or(nush_ast* tree) {
    int cpid;

    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);

        if (status != 0) {
            eval(tree->arg1);
        }
    }
    else {
        eval(tree->arg0);
    }
}

void
exec_semicolon(nush_ast* tree) {
    execute_ast(tree->arg0);
    execute_ast(tree->arg1); 
}

void
exec_equals(nush_ast* tree, svec* variables) {
    char* var = svec_get(tree->arg0->value, 0);
    char* val = svec_get(tree->arg1->value, 0);

    if (svec_contains(variables, var)) {
        for (int i = 0; i < variables->size; i++) {
            if (strcmp(svec_get(variables, i), var) == 0) {
                svec_put(variables, i+1, val);
                break;
            }
        }
    }
    else {
        svec_push_back(variables, var);
        svec_push_back(variables, val);
    }

}

void
execute_ast(nush_ast* tree) {
    if (tree) {
        char* cmd = svec_get(tree->value, 0);

        if (strcmp(cmd, "<") == 0) {
            redir_in(tree);
        }
        else if (strcmp(cmd, ">") == 0) {
            redir_out(tree);
        }
        else if (strcmp(cmd, "|") == 0) {
            exec_pipe(tree);
        }
        else if (strcmp(cmd, "&") == 0) {
            exec_background(tree);
        }
        else if (strcmp(cmd, "&&") == 0) {
            exec_and(tree);
        }
        else if (strcmp(cmd, "||") == 0) {
            exec_or(tree);
        }
        else if (strcmp(cmd, ";") == 0) {
            exec_semicolon(tree);
        }
        else {
            svec* tokens = tree->value;
        
            char* lines[tokens->size + 1];
        
            lines[tokens->size] = 0;
            for (int i = 0; i < tokens->size; i++) {
                lines[i] = svec_get(tokens, i);
            } 
            
            execute(lines[0], lines);
        }
    }
}


int
main(int argc, char* argv[])
{
    char cmd[256];
    FILE* f = stdin;
    int isFile = 1;
    if (argc != 1) {
        f = fopen(argv[1], "r");
        isFile = 0;
    }

    svec* variables = make_svec();

    while (1) {
        if (isFile) {
            printf("nush$ ");
            fflush(stdout);
        }

        char* rv = fgets(cmd, 256, f);

        if (! rv) {
            exit(0);
        }
         
        svec* tokens = tokenize(cmd);
        nush_ast* tree = parse_tokens(tokens);

        if (strcmp(svec_get(tree->value, 0), "=") == 0) {
            exec_equals(tree, variables);
        }
        else if (strcmp(svec_get(tree->value, 0), "echo") == 0) {
            replace_echo(tree->value, variables);
            execute_ast(tree);
        }
        else {
            execute_ast(tree);
        }
        free_ast(tree);
    }

    free_svec(variables);
    
    return 0;
}
