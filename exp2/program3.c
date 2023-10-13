// 普通管道通信
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
#define READ_END 0
#define WRITE_END 1

void Process_Msg(char *msg);

int main()
{
    char write_msg[MAX_BUF_SIZE] = "I am Here";
    char read_msg[MAX_BUF_SIZE];
    int pipe1[2];
    int pipe2[2];

    pid_t pid;

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    pid = fork();

    if (pid < 0)
    {
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid == 0)
    {
        close(pipe1[WRITE_END]);
        close(pipe2[READ_END]);

        read(pipe1[READ_END], read_msg, MAX_BUF_SIZE);
        Process_Msg(read_msg);
        write(pipe2[WRITE_END], read_msg, MAX_BUF_SIZE);

        close(pipe1[READ_END]);
        close(pipe2[WRITE_END]);
    }
    else
    {
        close(pipe1[READ_END]);
        write(pipe1[WRITE_END], write_msg, MAX_BUF_SIZE);
        close(pipe1[WRITE_END]);

        wait(NULL);

        char res[MAX_BUF_SIZE];

        close(pipe2[WRITE_END]);
        read(pipe2[READ_END], res, MAX_BUF_SIZE);
        close(pipe2[READ_END]);
        printf("%s\n", res);
    }
    return 0;
}

void Process_Msg(char *msg)
{
    int len = strlen(msg);
    for (int i = 0; i < len; i++)
    {
        if (msg[i] >= 'a' && msg[i] <= 'z')
            msg[i] = msg[i] - ('a' - 'A');
        else if (msg[i] >= 'A' && msg[i] <= 'Z')
            msg[i] = msg[i] + ('a' - 'A');
    }
}