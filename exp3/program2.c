#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define M 3
#define K 2
#define N 3

int A[M][K] = {{1, 4}, {2, 5}, {3, 6}};
int B[K][N] = {{8, 7, 6}, {5, 4, 3}};
int C[M][N];

typedef struct
{
    int i;
    int j; /* data */
} Data;

void *Cal_Matrix(void *arg)
{
    Data *tmp = (Data *)arg;
    printf("Threading at row:%d line:%d is running!\n", tmp->i, tmp->j);
    C[tmp->i][tmp->j] = 0;
    for (int i = 0; i < K; i++)
        C[tmp->i][tmp->j] += A[tmp->i][i] * B[i][tmp->j];

    pthread_exit(0);
}

int main()
{
    pthread_t tid[M * N];
    pthread_attr_t attr[M * N];

    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            Data *tmp = (Data *)malloc(sizeof(Data));
            tmp->i = i;
            tmp->j = j;
            pthread_attr_init(&attr[i * N + j]);
            pthread_create(&tid[i * N + j], &attr[i * N + j], Cal_Matrix, tmp);
        }
    }

    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
            pthread_join(tid[i * N + j], NULL);

    printf("Result:\n");
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            printf("%d ", C[i][j]);
        printf("\n");
    }
    return 0;
}