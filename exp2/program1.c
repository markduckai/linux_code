#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <wait.h>

int main()
{
    int num;
    printf("Please input a number:");
    scanf("%d", &num);
    pid_t pid;
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "ForkFailed");
        exit(-1);
    }
    else if (pid == 0)
    {
        printf("Child Process Running!\n");
        printf("num list:\n");
        while (num != 1)
        {
            if (num % 2 == 0)
                num = num / 2;
            else
                num = num * 3 + 1;
            printf("%d ", num);
        }
        printf("\n");
    }
    else
    {
        wait(NULL);
        printf("Child Complete!\n");
        exit(0);
    }
}
