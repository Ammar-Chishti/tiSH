#include "tish.h"

int wstatus;

// These are global because they need to be freed on exit
char** commands;
char** tokens;
char* cur_line;

// These are global because they need to be accessed within signal handlers
int d_flag_global;
int num_tokens_global;

#ifdef EXTRA_CREDIT
int t_flag_global;
struct rusage usage;
struct timeval user_start, user_end;
struct timeval system_start, system_end;

long wall_time_start, wall_time_end;
struct timeval wall_time;
#endif

bool file_exists(char* file_path) {
    struct stat sb;
    return (stat(file_path, &sb) == 0);
}

void validate_CLArgs(int argc, char* argv[], struct program_args* CLArgs) {
    int option;

#ifdef EXTRA_CREDIT
    while ((option = getopt(argc, argv, "dt")) != -1) {
#else
    while ((option = getopt(argc, argv, "d")) != -1) {
#endif
        switch (option) {
            case 'd':
                printf("-d flag\n");
                CLArgs->d_flag = true;
                d_flag_global = true;
                break;
#ifdef EXTRA_CREDIT
            case 't':
                printf("-t flag\n");
                t_flag_global = true;
                break;
#endif
            case '?':
                fprintf(stderr, "Unknown option %c given. Exiting... \n", optopt);
                exit(EXIT_FAILURE);
                break;
        }
    }

    if ((argc - optind) > 1) {
        fprintf(stderr, "More than one argument given when max one is allowed for file name. Exiting...\n");
        exit(EXIT_FAILURE);
    } else if ((argc - optind) == 1) {
        if (!file_exists(argv[optind])) {
            fprintf(stderr, "File given to read cmds does not exist. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        char* full_infile_path = realpath(argv[optind], NULL);
        CLArgs->infile = full_infile_path;
    }
}

int get_next_line(char** cur_line, size_t* cur_line_size, FILE* infileptr) {
    if (infileptr) {    // Non-Interactive Mode
        if (getline(cur_line, cur_line_size, infileptr) < 0) {
            return -1;
        }
    } else {    // Interactive Mode
        printf("tiSH> ");
        if (fgets(*cur_line, getpagesize(), stdin) == NULL) {
            return -1;
        }
        *cur_line_size = strlen(*cur_line)-1;
        (*cur_line)[*cur_line_size] = '\0';
    }
    return 0;
}

void tokenize(char* cur_line_or_command, char** commands_or_tokens, int* num, char* strtok_str) {
    char* token = strtok(cur_line_or_command, strtok_str);
    while (token != NULL) {
        commands_or_tokens[*num] = token;
        *num += 1;
        token = strtok(NULL, strtok_str);
    }
}

void redirect_streams(char** tokens, int* num_tokens, struct redirect_info* redirect_flags) {
    
    for (int i = 0; i < *num_tokens; i++) {
        char* cur_token = tokens[i];

        if (!strcmp(cur_token, ">") || !(strcmp(cur_token, "1>"))) {
            if (i == *num_tokens-1 || i == 0) {
                fprintf(stderr, "Incorrect redirection args. Exiting...");
                exit(EXIT_FAILURE);
            }
            redirect_flags->outfile = malloc(strlen(tokens[i+1]));
            strcpy(redirect_flags->outfile, tokens[i+1]);

        } else if (!strcmp(cur_token, "2>")) {
            if (i == *num_tokens-1 || i == 0) {
                fprintf(stderr, "Incorrect redirection args. Exiting...");
                exit(EXIT_FAILURE);
            }
            redirect_flags->errfile = malloc(strlen(tokens[i+1]));
            strcpy(redirect_flags->errfile, tokens[i+1]);
        } else if (!strcmp(cur_token, "<")) {
            if (i == *num_tokens-1 || i == 0) {
                fprintf(stderr, "Incorrect redirection args. Exiting...");
                exit(EXIT_FAILURE);
            }
            redirect_flags->infile = malloc(strlen(tokens[i+1]));
            strcpy(redirect_flags->infile, tokens[i+1]);
        }
    }

    if (redirect_flags->infile) {
        int fd = open(redirect_flags->infile, O_RDWR);
        if (fd < 0) {
            perror("Unable to open file redirected from stdin");
            exit(EXIT_FAILURE);
        }

        redirect_flags->saved_stdin = dup(STDIN_FILENO);
        if (redirect_flags->saved_stdin < 0) {
            perror("Unable to dup to save stdin fd");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("Unable to dup2 with file we are redirecting from stdin");
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
    if (redirect_flags->outfile) {
        int fd = open(redirect_flags->outfile, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
        if (fd < 0) {
            perror("Unable to open file we are redirecting stdout to");
            exit(EXIT_FAILURE);
        }

        redirect_flags->saved_stdout = dup(STDOUT_FILENO);
        if (redirect_flags->saved_stdout < 0) {
            perror("Unable to dup to save stdout fd");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("Unable to dup2 with file we are redirecting stdout to");
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
    if (redirect_flags->errfile) {
        int fd = open(redirect_flags->errfile, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
        if (fd < 0) {
            perror("Unable to open file we are redirecting stderr to");
            exit(EXIT_FAILURE);
        }

        redirect_flags->saved_stderr = dup(STDERR_FILENO);
        if (redirect_flags->saved_stderr < 0) {
            perror("Unable to dup to save stderr fd");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDERR_FILENO) < 0) {
            perror("Unable to dup2 with file we are redirecting stderr to");
            exit(EXIT_FAILURE);
        }
        close(fd);
    }

    // Removing all redirection tokens
    for (int i = 0; i < *num_tokens; i++) {
        char* cur_token = tokens[i];
        if (!strcmp(cur_token, "<") || !strcmp(cur_token, ">") || !strcmp(cur_token, "1>") || !strcmp(cur_token, "2>")) {
            *num_tokens = i;
            tokens[i] = '\0';
        }
    }
}

void set_ret_val(int ret_val) {
    if (!ret_val) {
        var_list_set("?", "0");
    } else {
        var_list_set("?", "1");
    }
}

void sigchld_handler(int sig) {
    if (d_flag_global) {
        fprintf(stderr, "Exiting: \"");
        for (int i = 0; i < num_tokens_global; i++) {
            if (i < num_tokens_global-1) {
                fprintf(stderr, "%s ", tokens[i]);
            } else {
                fprintf(stderr, "%s", tokens[i]);
            }
        }
        fprintf(stderr, "\" (ret=%d)\n", WEXITSTATUS(wstatus));
    }
#ifdef EXTRA_CREDIT
    if (t_flag_global) {
        if (getrusage(RUSAGE_SELF, &usage) < 0) {
            fprintf(stderr, "getrusage failed in getting end times. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        user_end = usage.ru_utime;
        system_end = usage.ru_stime;

        if (gettimeofday(&wall_time, NULL) < 0) {
            fprintf(stderr, "gettimeofday failed in setting end times. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        wall_time_end = (long)wall_time.tv_sec * 1000 + (long)wall_time.tv_usec / 1000;

        printf("TIMES: real=%ld.%03lds user=%ld.%06lds sys=%ld.%06lds\n", (wall_time_end - wall_time_start) / 1000, (wall_time_end - wall_time_start) % 1000, user_end.tv_sec - user_start.tv_sec, user_end.tv_usec - user_start.tv_usec, system_end.tv_sec - system_start.tv_sec, system_end.tv_usec - system_start.tv_usec);
    }
#endif
    set_ret_val(WEXITSTATUS(wstatus));
}

void sigint_handler(int sig) {
    free(commands);
    free(tokens);
    free(cur_line);
    free_var_list();
    exit(EXIT_SUCCESS);
}

char** expand_glob(char** tokens, int* num_tokens, int cur_index) {
    bool found_flag = false;
    char** temp_buf = malloc(MAX_INPUT_SIZE);
    if (temp_buf == NULL) {
        fprintf(stderr, "Malloc failed. Exiting...\n");
        exit(EXIT_FAILURE);
    }

    int temp_buf_size = 0;

    for (int i = 0; i < cur_index; i++) {
        temp_buf[i] = tokens[i];
    }
    temp_buf_size = cur_index;
    
    DIR *folder;
    struct dirent *entry;
    int files = 0;

    folder = opendir(".");
    if (folder == NULL) {
        perror("Unable to read cur_dir when trying to parse file glob");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(folder))) {
        files++;
        if (strstr(entry->d_name, tokens[cur_index]+1)) {
            found_flag = true;
            char* dir_buf = malloc(strlen(entry->d_name));
            strcpy(dir_buf, entry->d_name);
            temp_buf[temp_buf_size] = dir_buf;
            temp_buf_size += 1;
        }
    }

    for (int i = cur_index+1; i < *num_tokens; i++) {
        temp_buf[temp_buf_size] = tokens[i];
        temp_buf_size += 1;
    }

    temp_buf[temp_buf_size] = '\0';
    closedir(folder);

    *num_tokens = temp_buf_size;
    if (!found_flag) {
        free(temp_buf);
        return tokens;
    }
    
    free(tokens);
    return temp_buf;
}

void external_cmd_exec(char** tokens, int num_tokens, int d_flag) {
    if (d_flag) {
        fprintf(stderr, "RUNNING: \"");
        for (int i = 0; i < num_tokens; i++) {
            if (i < num_tokens-1) {
                fprintf(stderr, "%s ", tokens[i]);
            } else {
                fprintf(stderr, "%s", tokens[i]);
            }
        }
        fprintf(stderr, "\"\n");
        num_tokens_global = num_tokens;
    }

#ifdef EXTRA_CREDIT
    if (t_flag_global) {
        if (getrusage(RUSAGE_SELF, &usage) < 0) {
            fprintf(stderr, "getrusage failed in setting start times. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        user_start = usage.ru_utime;
        system_start = usage.ru_stime;

        if (gettimeofday(&wall_time, NULL) < 0) {
            fprintf(stderr, "gettimeofday failed in setting start time. Exiting...\n");
            exit(EXIT_FAILURE);
        }
        wall_time_start = (long)wall_time.tv_sec * 1000 + (long)wall_time.tv_usec / 1000;
    }

    for (int i = 0; i < num_tokens; i++) {
        if (strchr(tokens[i], '*')) {
            tokens = expand_glob(tokens, &num_tokens, i);
            break;
        }
    }
#endif

    int child = fork();
    if (child < 0) {
        fprintf(stderr, "Fork failed. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    if (child == 0) {
        if (execvp(tokens[0], tokens) < 0) {
            exit(EXIT_FAILURE);
        }
    }
    wait(&wstatus);
}

void exec_cmd(char** tokens, int num_tokens, struct redirect_info* redirect_flags, bool d_flag) {

    int ret_val = 1;
    if (!strcmp(tokens[0], "cd")) {
        if (d_flag) fprintf(stderr, "RUNNING: \"cd\"\n");
        ret_val = internal_cd(tokens[1]);
        set_ret_val(ret_val);
        if (d_flag) fprintf(stderr, "ENDED: \"cd\" (ret=%d)\n", ret_val);
    } else if (!strcmp(tokens[0], "pwd")) {
        if (d_flag) fprintf(stderr, "RUNNING: \"pwd\"\n");
        ret_val = internal_pwd();
        set_ret_val(ret_val);
        if (d_flag) fprintf(stderr, "ENDED: \"pwd\" (ret=%d)\n", ret_val);
    } else if (!strcmp(tokens[0], "echo")) {
        if (d_flag) fprintf(stderr, "RUNNING: \"echo\"\n");
        ret_val = internal_echo(tokens, num_tokens);
        set_ret_val(ret_val);
        if (d_flag) fprintf(stderr, "ENDED: \"echo\" (ret=%d)\n", ret_val);
    } else if (!strcmp(tokens[0], "exit")) {
        free(commands);
        free(tokens);
        free(cur_line);
        free_var_list();
        if (d_flag) fprintf(stderr, "RUNNING: \"exit\"");
        exit(EXIT_SUCCESS);
    } else if (strchr(tokens[0], '=')) {
        if (d_flag) fprintf(stderr, "RUNNING: assigning var\n");
        ret_val = internal_var_assignment(tokens[0]);
        set_ret_val(ret_val);
        if (d_flag) fprintf(stderr, "ENDED: assigning var (ret=%d)\n", ret_val);
    } else {
        external_cmd_exec(tokens, num_tokens, d_flag);
    }

    fflush(stdout);
    fflush(stderr);
}

int main(int argc, char** argv, char** envp) {
    struct program_args CLArgs = {
        .t_flag = false,
        .d_flag = false,
        .infile = NULL  // The file we are reading when in non-interactive mode
    };
    validate_CLArgs(argc, argv, &CLArgs);

    signal(SIGCHLD, sigchld_handler);
    signal(SIGINT, sigint_handler);

    var_list_init(envp);

    FILE* exec_file_ptr = NULL;
    if (CLArgs.infile) {
        if ((exec_file_ptr = fopen(CLArgs.infile, "r")) < 0) {
            fprintf(stderr, "Unable to open infile. Exiting...");
            free_var_list();
            exit(EXIT_FAILURE);
        }
    }

    size_t cur_line_size = 0;
    cur_line = malloc(sizeof(char) * MAX_INPUT_SIZE);
    if (cur_line == NULL) {
        fprintf(stderr, "Malloc Failed. Exiting...");
        exit(EXIT_FAILURE);
    }

    struct redirect_info redirect_flags = {
        .saved_stdin = -1,
        .saved_stdout = -1,
        .saved_stderr = -1,
        .infile = NULL,
        .outfile = NULL,
        .errfile = NULL
    };
    while ((get_next_line(&cur_line, &cur_line_size, exec_file_ptr) != -1)) {
        
        // We have to do some hacky stuff here because cur_line_size is not exactly
        // strlen(cur_line) due to how getline operates. We also need to remove the newline
        if (CLArgs.infile) {
            int i = 0;
            while (i < cur_line_size) {
                if (cur_line[i] == '\n') {
                    cur_line[i] = '\0';
                    cur_line_size = i;
                    break;
                }
                i += 1;
            }
        }

        for (int i = 0; i < cur_line_size; i++) {   // Removing everything past #
            if (cur_line[i] == '#') {
                cur_line[i] = '\0';
                cur_line_size = i;
                break;
            }
        }

        if (cur_line_size == 0) {
            continue;
        }

        commands = malloc(sizeof(char) * MAX_INPUT_SIZE);
        if (commands == NULL) {
            fprintf(stderr, "Malloc failed. Exiting...");
            exit(EXIT_FAILURE);
        }
        
        // This is kind of ugly
        int num_commands = 1;
        if (strchr(cur_line, '|')) {
            int temp = 1;
            for (int i = 0; i < strlen(cur_line); i++) {
                if (cur_line[i] == '|') {
                    temp += 1;
                }
            }
            tokenize(cur_line, commands, &num_commands, "| ");
            num_commands = temp;
        } else {
            commands[0] = cur_line;
        }
        
        // printf("Num commands is %d\n", num_commands);
        // for (int i = 0; i < num_commands; i++) {
        //     printf("%d\n", num_commands);
        // }

        for (int i = 0; i < num_commands; i++) {
            char* cur_command = commands[i];

            int num_tokens = 0;
            tokens = malloc(sizeof(char) * MAX_INPUT_SIZE);
            if (tokens == NULL) {
                fprintf(stderr, "Malloc failed. Exiting...");
                exit(EXIT_FAILURE);
            }

            tokenize(cur_command, tokens, &num_tokens, " ");

            if (num_tokens > 1) redirect_streams(tokens, &num_tokens, &redirect_flags);

            exec_cmd(tokens, num_tokens, &redirect_flags, CLArgs.d_flag);

            if (redirect_flags.saved_stdin != -1) {
                if (dup2(redirect_flags.saved_stdin, STDIN_FILENO) < 0) {
                    fprintf(stderr, "dup2 failed when restoring saved stdin fd. Exiting...\n");
                    exit(EXIT_FAILURE);
                }
            }
            if (redirect_flags.saved_stdout != -1) {
                if (dup2(redirect_flags.saved_stdout, STDOUT_FILENO) < 0) {
                    fprintf(stderr, "dup2 failed when restoring saved stdout fd. Exiting...\n");
                    exit(EXIT_FAILURE);
                }
            }
            if (redirect_flags.saved_stderr != -1) {
                if (dup2(redirect_flags.saved_stderr, STDERR_FILENO) < 0) {
                    fprintf(stderr, "dup2 failed when restoring saved stderr fd. Exiting...\n");
                    exit(EXIT_FAILURE);
                }
            }
            redirect_flags.saved_stdin = -1;
            redirect_flags.saved_stdout = -1;
            redirect_flags.saved_stderr = -1;
            redirect_flags.infile = NULL;
            redirect_flags.outfile = NULL;
            redirect_flags.errfile = NULL;
        }
    }

    free(commands);
    free(tokens);
    free(cur_line);
    free_var_list();
    if (CLArgs.infile) {
        fclose(exec_file_ptr);
    }
    return EXIT_SUCCESS;
}
