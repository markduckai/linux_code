#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>


// simulate buffer 
typedef int buffer_item;
#define MAX_BUFFERSIZE 5
buffer_item buffer[MAX_BUFFERSIZE] = {-1};


// semaphore definition
sem_t empty;
sem_t full;
sem_t mutex_buffer;

// buffer queue head and tail
int in_buf = 0;
int out_buf = 0;

// producer thread
void *producer(void *p_id)
{
    while (1)
    {
        int item;
        sleep(rand() % 2 + 1);
        sem_wait(&empty);
        sem_wait(&mutex_buffer);

        srand((unsigned)time(NULL));
        item = rand() % 50;
        buffer[in_buf] = item;
        in_buf = (in_buf + 1) % MAX_BUFFERSIZE;
        printf("Producer%d put %d into the buffer\n", *((int *)p_id), item);
        
        sem_post(&mutex_buffer);
        sem_post(&full);

    }
}

// consumer thread
void *consumer(void *p_id)
{
    while (1)
    {
        srand((unsigned)time(NULL));
        sleep(rand() % 2 + 1);
        sem_wait(&full);
        sem_wait(&mutex_buffer);
        
        int item;
        item = buffer[out_buf];
        out_buf = (out_buf + 1) % MAX_BUFFERSIZE;
        printf("Consumer%d get %d from the buffer\n", *((int *)p_id), item);
        
        sem_post(&mutex_buffer);
        sem_post(&empty);

    }
}

// convert a char array into a int number
int convert_string_to_num(char *buf)
{
    int num = 0;
    while (*buf)
    {
        if (!(*buf >= '0' && *buf <= '9'))
            return -1;
        else
            num = num * 10 + (*buf - '0');
        buf++;
    }
    return num;
}

int main(int argc, char *argv[])
{
    // command line arguments initialization
    int waiting_time, producer_num, consumer_num;
    if (argc < 4)
        printf("Too few arguments!usage:[waiting time],[producer_num],[consumer_num]\n");
    else
    {
        waiting_time = convert_string_to_num(argv[1]);
        producer_num = convert_string_to_num(argv[2]);
        consumer_num = convert_string_to_num(argv[3]);
        if (waiting_time == -1 || producer_num == -1 || consumer_num == -1)
        {
            printf("Error occur!Please check your arguments\n");
            return 0;
        }
    }

    // semaphore initialization
    sem_init(&empty, 0, MAX_BUFFERSIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex_buffer, 0, 1);

    //used for identifying different pthread
    int num[producer_num + consumer_num];
    // thread tid
    pthread_t tid[producer_num + consumer_num];

    // producer and consumer thread initialization
    for (int i = 0; i < producer_num; i++)
    {
        pthread_attr_t attr_tmp;
        pthread_attr_init(&attr_tmp);
        num[i] = i;
        pthread_create(&tid[i], &attr_tmp, producer, &num[i]);
    }

    for (int i = 0; i < consumer_num; i++)
    {
        pthread_attr_t attr_tmp;
        pthread_attr_init(&attr_tmp);
        num[i + producer_num] = i;
        pthread_create(&tid[i + producer_num], &attr_tmp, consumer, &num[i + producer_num]);
    }

    // main process waits given seconds and terminates
    sleep(waiting_time);
    printf("Process exit successfully after running %d seconds\n", waiting_time);
    return 0;
}