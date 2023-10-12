// 以共享内存技术编成实现Collatz猜想
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024
typedef struct
{
    int len;
    int list[MAX_BUF_SIZE];
    /* data */
} Collatz;

int main()
{
    key_t key = ftok("?", 233);                                  // 获得唯一IPC标识符
    int shm_id = shmget(key, sizeof(Collatz), IPC_CREAT | 0666); // 通过IPC申请共享内存

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
        Collatz *pointer = (Collatz *)shmat(shm_id, NULL, 0); // 在子进程创建共享内存的虚拟内存映射
        pointer->len = 0;
        printf("Child Process Running!\n");
        pointer->list[pointer->len++] = num;
        while (num != 1)
        {
            if (num % 2 == 0)
                num /= 2;
            else
                num = num * 3 + 1;
            pointer->list[pointer->len++] = num;    //共享内存的写
        }
    }
    else
    {
        Collatz *pointer = (Collatz *)shmat(shm_id, NULL, 0);   //在主进程创建共享内存的虚拟内存映射
        wait(NULL);
        printf("Child Complete!\n");
        for (int i = 0; i < pointer->len; i++)
            printf("%d ", pointer->list[i]);
        printf("\n");
        shmdt(pointer); //解除当前进程的内存映射
        exit(0);
    }
}