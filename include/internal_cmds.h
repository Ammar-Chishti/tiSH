#ifndef INTERNAL_CMDS_H
#define INTERNAL_CMDS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "var_linked_list.h"

#define PATH_MAX 4096

int internal_cd(char* dest_path);
int internal_pwd();
int internal_echo();
int internal_var_assignment(char* token);

#endif