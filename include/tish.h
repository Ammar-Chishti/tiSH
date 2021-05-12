#ifndef TISH_H
#define TISH_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <dirent.h>
#include "var_linked_list.h"
#include "internal_cmds.h"

#define MAX_INPUT_SIZE getpagesize()

struct program_args {
    bool t_flag;
    bool d_flag;
    char* infile;
};

struct redirect_info {
    int saved_stdin;
    int saved_stdout;
    int saved_stderr;
    char* infile;
    char* outfile;
    char* errfile;
};

// -d
// -t
// file_name
// format: exec [-dt] [file_name]

#endif