#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int max_arg;
int min_arg;
float avg_arg;
int len;

void *Cal_Max(void *num)
{
    max_arg = *((int *)num);
    printf("Threading Cal_Max is running!\n");
    for (int i = 1; i < len; i++)
    {
        if (*((int *)num + i) > max_arg)
            max_arg = *((int *)num + i);
    }

    pthread_exit(0);
}

void *Cal_Min(void *num)
{
    min_arg = *(int *)num;
    printf("Threading Cal_Min is running!\n");
    for (int i = 1; i < len; i++)
    {
        if (*((int *)num + i) < min_arg)
            min_arg = *((int *)num + i);
    }

    pthread_exit(0);
}

void *Cal_Avg(void *num)
{
    avg_arg = 0;
    printf("Threading Cal_Avg is running!\n");
    for (int i = 0; i < len; i++)
    {
        avg_arg += *((int *)num + i);
    }
    avg_arg = avg_arg / len;

    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    pthread_t tid_1, tid_2, tid_3;
    pthread_attr_t attr_1, attr_2, attr_3;

    if (argc <= 2)
    {
        fprintf(stderr, "Please input numbers\n");
        return -1;
    }
    int num[1000];
    for (int i = 1; i < argc; i++)
        num[i - 1] = atoi(argv[i]);
    len = argc - 1;

    pthread_attr_init(&attr_1);
    pthread_attr_init(&attr_2);
    pthread_attr_init(&attr_3);

    pthread_create(&tid_1, &attr_1, Cal_Max, num);
    pthread_create(&tid_2, &attr_2, Cal_Min, num);
    pthread_create(&tid_3, &attr_3, Cal_Avg, num);

    pthread_join(tid_1, NULL);
    pthread_join(tid_2, NULL);
    pthread_join(tid_3, NULL);

    printf("Finishing Calculating\n");
    printf("The result is:\nMax: %d \nMin: %d \nAvg: %f\n", max_arg, min_arg, avg_arg);
    return 0;
}