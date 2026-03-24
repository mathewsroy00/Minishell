#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
 

#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern char prompt[20];
extern char input_string[100];
extern char* external_commands[153];
extern int status;
extern pid_t pid;
extern struct job* head;

typedef struct job
{
    int pid;            // process id
    char cmd[100];      // command string
    struct job *link;
}job_t;


void scan_input(char *prompt);
void signal_handler(int sig_num);

void insert_at_first(pid_t pid, char* cmd);
void delete_at_first(job_t **head);
void print_jobs_list(job_t* head);

int check_command_type(char *command);
void execute_internal_commands(char *args[]);
void execute_external_commands(char *args[]);
void extract_external_commands(char **external_commands);


#endif