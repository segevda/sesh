/******************************************************************************
 *                                  sesh.c                                    *
 *                                                                            *
 *              Segev Shell v1.0 - per demands from OSTEP book project        *
 *                                                                            *
 ******************************************************************************/

#define _POSIX_C_SOURCE 200809L // strdup, getline

#include <stdio.h>     // printf, getline
#include <string.h>    // strtok, strdup
#include <stdlib.h>    // free
#include <unistd.h>    // execvp
#include <sys/types.h> // fork, wait
#include <unistd.h>    // fork
#include <sys/wait.h>  // wait

#define MAX_ARGS 256
#define UNUSED(x) ((void)(x))

// int prompt = 0;
static void RunPrompt(void);
static int ParseInput(char *input, char **parsed);
static void ExecuteCommand(int argc, char **argv);
static void FreeStringList(char **list);
static int WhichCommand(char *string);
static void ExitCommand(void);

enum builtin_cdms
{
    EXIT,
    /* CD,
    PATH, */
    NUM_OF_BUILTIN_CMDS
};

enum bool
{
    FALSE,
    TRUE
};

typedef void (*builtin_cmd)(void);

int g_exit_flag = TRUE;
char *path = "/bin";
char *builtin_cmds[NUM_OF_BUILTIN_CMDS] = {"exit", /* "cd", "path" */};
builtin_cmd commands[NUM_OF_BUILTIN_CMDS] = 
{
    ExitCommand,
    /* 0,
       0 */
};


int main(int argc, char const *argv[])
{
    UNUSED(argv);
    
    if (1 == argc)
    {
        RunPrompt();
    }

    return 0;
}

static void RunPrompt(void)
{
    char *line = NULL;
    size_t line_length = 0;
    char *argv[MAX_ARGS] = {0};
    int argc = 0;
    
    while (g_exit_flag)
    {
        printf("\033[1;32msesh> \033[0m");

        getline(&line, &line_length, stdin); // TODO - error handling

        argc = ParseInput(line, argv);

        ExecuteCommand(argc, argv);

        free(line);
        line = NULL;
        FreeStringList(argv);
    }

}

static int ParseInput(char *input, char **parsed)
{
    // int argc = CountArguments()
    int i = 1;
    char *curr_arg = NULL;

    parsed[0] = strdup(strtok(input, " \t\n"));

    while ((curr_arg = strtok(NULL, " \t\n")))
    {
        parsed[i] = strdup(curr_arg);
        ++i;
    }

    return i;
}

static void ExecuteCommand(int argc, char **argv)
{
    UNUSED(argc);

    int command_number = WhichCommand(argv[0]);
    
    if (-1 != command_number)
    {
        commands[command_number]();
    }
    else /* TODO - check if binary exists in path directories */
    {
        pid_t pid = fork();

        if (0 == pid)
        {
            execvp(argv[0], argv);
        }
    }

    wait(NULL);
}

static void FreeStringList(char **list)
{
    unsigned int i = 0;
    
    while (list[i])
    {
        free(list[i]);
        list[i] = NULL;
        ++i;
    }
}

static int WhichCommand(char *string)
{
    int i = 0;
    int command = -1;

    while (i < NUM_OF_BUILTIN_CMDS && strcmp(string, builtin_cmds[i]))
    {
        ++i;
    }

    if (i < NUM_OF_BUILTIN_CMDS)
    {
        command = i;
    }

    return command;
}

static void ExitCommand(void)
{
    g_exit_flag = FALSE;
}
