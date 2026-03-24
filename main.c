/*
* Name    : Mathews Roy
* Date    : 8-02-2026
* Project : Implementing a MINISHELL 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"

char prompt[20] = "minishell:~$";           // shell prompt string
char input_string[100];                     // Buffer for storing input 
char* external_commands[153];               // Array for storing external commands
int status;               
pid_t pid;
job_t* head = NULL;                         // Head of linked list of stopped jobs

int main()
{
    system("clear");
    scan_input(prompt);         // Input function calling
    return 0;
}