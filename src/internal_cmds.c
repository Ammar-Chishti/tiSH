#include "internal_cmds.h"


int internal_cd(char* dest_path) {
    if (chdir(dest_path) < 0) {
        fprintf(stderr, "Internal cd failed");
        return 1;
    }
    return 0;
}

int internal_pwd() {
    char* cwd = malloc(sizeof(char) * PATH_MAX);
    if (getcwd(cwd, PATH_MAX) == NULL) {
        free(cwd);
        fprintf(stderr, "Internal pwd cmd failed");
        return 1;
    }
    printf("%s\n", cwd);
    free(cwd);
    return 0;
}

int internal_echo(char** tokens, int num_tokens) {
    char* echo_str = malloc(sizeof(char) * PATH_MAX);
    if (echo_str == NULL) {
        fprintf(stderr, "Malloc failed. Exiting...");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < num_tokens; i++) {
        if (tokens[i][0] == '$') {
            char* var = var_list_get(tokens[i]+1);
            if (var == NULL) {
                fprintf(stderr, "Trying to echo a variable that doesn't exist\n");
                return 1;
            } else {
                strcat(echo_str, var);
                strcat(echo_str, " ");
                continue;
            }
        }

        for (int j = 0; j < strlen(tokens[i]); j++) {
            if (tokens[i][j] ==  '\"') {
                continue;
            }
            sprintf(echo_str + strlen(echo_str), "%c", tokens[i][j]);
        }
        strcat(echo_str, " ");
    }
    strcat(echo_str, "\0");
    printf("%s\n", echo_str);
    free(echo_str);
    return 0;
}

int internal_var_assignment(char* token) {
    // printf("%s\n", token);
    char* name_val = strtok(token, "=");
    char* name_actual = malloc(strlen(name_val));
    if (name_actual == NULL) {
        fprintf(stderr, "Malloc failed. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    strcpy(name_actual, name_val);
    
    name_val = strtok(NULL, "=");
    char* val_actual = malloc(strlen(name_val));
    if (val_actual == NULL) {
        fprintf(stderr, "Malloc failed. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    int size = 0;
    for (int i = 0; i < strlen(name_val); i++) {
        if (name_val[i] == '\"') continue;
        val_actual[size] = name_val[i];
        size += 1;
    }
    val_actual[size]= '\0';

    var_list_append(name_actual, val_actual);
    return 0;
}