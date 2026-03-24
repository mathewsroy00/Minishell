#include "shell.h"

void scan_input(char *prompt)
{
    // register SIGINIT, SIGTSTP, SIGCHLD
    signal(SIGINT, signal_handler);
    signal(SIGTSTP, signal_handler);
    signal(SIGCHLD, signal_handler);

    // Load list of external commands
    extract_external_commands(external_commands);

    while(1)
    {
        // Prints the prompt
        printf(ANSI_COLOR_GREEN "%s " ANSI_COLOR_RESET, prompt); 

        // Reading input from user
        memset(input_string, 0, 100);
        scanf("%[^\n]", input_string);
        getchar(); 

        // checking input string contains PS1
        if(strncmp(input_string, "PS1=", 4) == 0) 
        {
            // empty prompt
            if(input_string[4] == '\0')
            {
                printf("Command error: prompt is empty\n");
                continue;
            }

            // checking for space
            int i = 4;
            while(input_string[i] != '\0')
            {
                if(input_string[i] == ' ')
                {
                    printf("Command error: Spaces not allowed\n");
                    break;
                }
                i++;
            }

            if(input_string[i] == ' ')
                continue;

            strcpy(prompt, input_string + 4);
        }
        else
        {
            // Tokenize input into arguments
            char temp[100];
            strcpy(temp, input_string);

            char* args[50];
            int argc = 0;

            char* token = strtok(temp, " ");
            while(token != NULL)
            {
                args[argc++] = token;
                token = strtok(NULL, " ");
            }

            args[argc] = NULL;

            // Ignore empty input
            if(args[0] == NULL)
                continue;

            if(check_command_type(args[0]) == BUILTIN)
            {
                execute_internal_commands(args);
            }
            else if(check_command_type(args[0]) == EXTERNAL)
            {
                execute_external_commands(args);
            }
            else 
            {
                printf("Command not found\n");
                continue;
            }
        }
    }
}

/* Handles SIGINT, SIGTSTP, and SIGCHLD signals */
void signal_handler(int signum)
{
    if(signum == SIGINT)
    {
        // ctrl + c -> sigint
        if(pid == 0)
        {
            // Print prompt when no foreground process exists
            printf("\n" ANSI_COLOR_GREEN "%s " ANSI_COLOR_RESET, prompt);
            fflush(stdout);
        }
    }
    else if(signum == SIGTSTP)
    {
        // ctrl + z  -> sigtstp
        if(pid == 0)
        {
            if(pid == 0)
            {
                // Print prompt when no process is running
                printf("\n" ANSI_COLOR_GREEN "%s " ANSI_COLOR_RESET, prompt);
                fflush(stdout);
            }
        }
        else 
        {
            // Stop foreground process and add it to job list
            kill(pid, SIGTSTP);
            insert_at_first(pid, input_string);
            pid = 0;
            printf("\n");
        }
    }
    // Handle terminated background processes
    else if(signum == SIGCHLD)
    {
        while(waitpid(-1, &status, WNOHANG) > 0);
    }
}

/* Insert a stopped job at the beginning of the job list */
void insert_at_first(pid_t pid,  char* cmd)
{
    job_t* new = malloc(sizeof(job_t));
    new -> pid = pid;
    strcpy(new -> cmd, cmd);

    new -> link = head;
    head = new;
}

/* Remove the most recent job from the job list */
void delete_at_first(job_t **head)
{
    if(*head == NULL)
        return;

    job_t *temp = *head;
    *head = temp -> link;
    free(temp);
}


void print_jobs_list(job_t* head)
{
    job_t* temp = head;
    int job_count = 1;

    while(temp)
    {
        printf("[%d] Stopped %d %s\n", job_count, temp->pid, temp->cmd);
        temp = temp -> link;
        job_count++;
    }
}