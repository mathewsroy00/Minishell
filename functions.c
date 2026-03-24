#include "shell.h"


static char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", "jobs", "fg", "bg", NULL};


void execute_external_commands(char* args[])
{
    int argc = 0;
    while(args[argc] != NULL)
        argc++;

    // Counting number of pipes
    int pipe_count = 0;
    for(int i = 0; i < argc; i++)
    {
        if(strcmp(args[i], "|") == 0)
            pipe_count++;
    }

    // No pipe case
    if(pipe_count == 0)
    {
        pid = fork();
        if(pid > 0)
        {
            waitpid(pid, &status, WUNTRACED);    // wait for foreground process
            pid = 0;
        }
        else if(pid == 0)
        {
            execvp(args[0], &args[0]);          // Executing command
            perror("execvp");
            exit(1);
        }
        return;
    }

    // Backup stdin and stdout for pipe handling
    int bkp_stdin = dup(STDIN_FILENO);
    int bkp_stdout = dup(STDOUT_FILENO);

     // if pipe is last or first
    if(strcmp("|", args[0]) == 0 || strcmp("|", args[argc - 1]) == 0)
    {
        printf("Error: Pipe cannot be first or last\n");
        printf("Usage: <command1> '|' <command2> '|' <command3> etc\n");
        return;
    }

    // checking for consecutive pipes
    for(int i = 0; i < argc - 1; i++)
    {
        if(strcmp("|", args[i]) == 0 && strcmp("|", args[i + 1]) == 0)
        {
            printf("Error: Pipe cannot be consecutive\n");
            printf("Usage: <command1> '|' <command2> '|' <command3> etc\n");
            return;
        }
    }

    int cmd_index[argc];       // for storing commmand starting index
    int idx = 0;

    cmd_index[idx++] = 0;       // storing the first command index 

    // splitting commands by replacing pipe with NULL
    for(int i = 0; i < argc; i++)
    {
        if(strcmp("|", args[i]) == 0)
        {
            args[i] = NULL;             // replacing pipe with null
            cmd_index[idx++] = i + 1;
        }
    }

    int pipefd[2]; 

    // creating one process per command
    for(int i = 0; i < idx; i++)
    {
        // creating pipe except for last command
        if(i != idx - 1)
        {
            if(pipe(pipefd) == -1)
            {
                perror("Pipe");
                exit(1);
            }
        }

        pid = fork();
        if(pid > 0)           // parent
        {
            if(i != idx - 1)
            {
                dup2(pipefd[0], STDIN_FILENO);  // next child reads from pipe

                close(pipefd[0]);
                close(pipefd[1]);
            }
        }
        else if(pid == 0)
        {
            if(i != idx - 1)
            {
                dup2(pipefd[1], STDOUT_FILENO); // sending output into pipe

                close(pipefd[0]);
                close(pipefd[1]);
            }

            execvp(args[cmd_index[i]], &args[cmd_index[i]]);
            perror("execvp");
            exit(1);
        }
        else
        {
            perror("fork");
            return;
        }
    }

    // waiting for all the children
    for(int i = 0; i < idx; i++)
        wait(NULL);

    dup2(bkp_stdin, STDIN_FILENO);
    dup2(bkp_stdout, STDOUT_FILENO);

    close(bkp_stdin);
    close(bkp_stdout);

    pid = 0;
}

void execute_internal_commands(char* args[])
{
    // Exit : terminates the shell
    if(strcmp(args[0], "exit") == 0)
        exit(0);
    // cd
    else if(strcmp(args[0], "cd") == 0)
    {
        if(args[1])                       // cd path 
        {
            if(chdir(args[1]) != 0)
                perror("cd");
        }
        else                                 // cd (go to home)
            chdir(getenv("HOME"));
    }
    // Printing current working directory
    else if(strcmp(args[0], "pwd") == 0)
    {
        char cwd[100];
        getcwd(cwd, sizeof(cwd));

        printf("%s\n", cwd);
    }

    // Echo
    else if(strcmp(args[0], "echo") == 0)
    {
        if(args[1] && strcmp(args[1], "$$") == 0)
        {
            printf("%d\n", getpid());
        }
        else if(args[1] && strcmp(args[1], "$?") == 0)
        {
            printf("%d\n",WEXITSTATUS(status));
        }
        else if(args[1] && strcmp(args[1], "$SHELL") == 0)
        {   
            printf("%s\n", getenv("SHELL"));
        }
        else
        {
            for(int i = 1; args[i] != NULL; i++)
                printf("%s ", args[i]);

            printf("\n");
        }
    }
    // fg : resumes the most recent stopped job background
   else if(strcmp(args[0], "fg") == 0)
    {
        if(head == NULL)
        {
            printf("fg: no current job\n");
            return;
        }

        pid = head->pid;                 // updating global pid          
        strcpy(input_string, head->cmd); 

        kill(head -> pid, SIGCONT);             // resume job
        printf("%s\n", head->cmd);

        // Wait for it
        waitpid(head -> pid, &status, WUNTRACED);

        delete_at_first(&head);         // remove job from list

        // Reset global pid
        pid = 0;
    }
    else if(strcmp(args[0], "bg") == 0)
    {
        static int bg_count = 0;

        if(head == NULL)
        {
            printf("bg: no current job\n");
            return;
        }
 
        kill(head -> pid, SIGCONT);              // Resumes job in background
        printf("[%d] %s &\n", ++bg_count, head->cmd);
        delete_at_first(&head);
    }
    else if(strcmp(args[0], "jobs") == 0)
    {
        print_jobs_list(head);
    }
}


int check_command_type(char* command)
{
    for(int i = 0; builtins[i] != NULL; i++)
    {
        if(strcmp(command, builtins[i]) == 0)                                      
            return BUILTIN;
    }

    for(int i = 0; external_commands[i] != NULL; i++)
    {
        if(strcmp(command, external_commands[i]) == 0)  
            return EXTERNAL;
    }

    return NO_COMMAND;
}

void extract_external_commands(char **external_commands)
{
    FILE* fp = fopen("external.txt", "r");
    if(fp == NULL)
    {
        perror("fopen");
        return;
    }

    char buffer[100];

    for(int i = 0; i < 152; i++)
    {
        fgets(buffer, sizeof(buffer), fp);

        // removing newline and replacing with null
        int j = 0;
        while(buffer[j] != '\0')
        {
            if(buffer[j] == '\n')
            {
                buffer[j] = '\0';
                break;
            }
            j++;
        }

        external_commands[i] = malloc(strlen(buffer) + 1);
        strcpy(external_commands[i], buffer);
    }
    
    external_commands[152] = NULL;
    fclose(fp);
}