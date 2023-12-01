// 内存管理实验
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <queue>

#define SEQ_RATE 0.7
#define FRONT_RATE 0.1
#define REAR_RATE 0.2
#define INS_PER_PAGE 10

// 生成0-99 100个地址的地址序列,映射成页地址并存储页地址流,返回长度
int generate_random_addrstream(int *pagestream)
{
    int addr_stream[100];
    int page_stream[100];
    int cur_addr = 0;
    srand(time(NULL));
    for (int i = 0; i < 100; i++)
        addr_stream[i] = 0;
    for (int i = 0; i < 100; i++)
    {
        float random_rate = (rand() % 100) / 100.0;
        addr_stream[i] = cur_addr;
        // 顺序指令
        if (random_rate < SEQ_RATE)
        {
            cur_addr = (cur_addr + 1) % 100;
        }
        // 前地址指令
        else if (random_rate < SEQ_RATE + FRONT_RATE)
        {
            if (cur_addr == 0)
                cur_addr = 99;
            else
                cur_addr = rand() % cur_addr;
        }
        // 后地址指令
        else
        {
            if (cur_addr == 99)
                cur_addr = 0;
            else
                cur_addr = rand() % (99 - cur_addr) + cur_addr + 1;
        }
    }
    for (int i = 0; i < 100; i++)
        page_stream[i] = addr_stream[i] / INS_PER_PAGE;

    int index = 0;
    int last_page = 0;
    // 合并连续的页面访问为1个
    for (int i = 0; i < 100; i++)
    {
        if (i == 0)
        {
            pagestream[index++] = page_stream[i];
            last_page = page_stream[i];
        }
        else
        {
            if (page_stream[i] == last_page)
                ;
            else
            {
                pagestream[index++] = page_stream[i];
                last_page = page_stream[i];
            }
        }
    }
    return index;
}

float optimal_algorithum(const int *pagestream, int length, int buffer_size);
float lru_algorithum(const int *pagestream, int length, int buffer_size);
float fifo_algorithum(const int *pagestream, int length, int buffer_size);

int main()
{
    int pagestream[100];

    int length = generate_random_addrstream(pagestream);
    printf("pagestream:");
    for (int i = 0; i < length; i++)
        printf("%d ", pagestream[i]);
    printf("\n");

    int page_number;
    if (100 % INS_PER_PAGE != 0)
        page_number = 100 / INS_PER_PAGE + 1;
    else
        page_number = 100 / INS_PER_PAGE;
    printf("page number:%d\n", page_number);
    printf("optimal algorithum:\n");
    for (int i = 1; i <= page_number; i++)
        printf("buffer size:%d, page fault rate:%f\%\n", i, optimal_algorithum(pagestream, length, i) * 100);
    printf("lru algorithum:\n");
    for (int i = 1; i <= page_number; i++)
        printf("buffer size:%d, page fault rate:%f\%\n", i, lru_algorithum(pagestream, length, i) * 100);
    printf("FIFO algorithum:\n");
    float tmp = fifo_algorithum(pagestream, length, 1) * 100;
    int flag = 0;
    for (int i = 1; i <= page_number && !flag; i++)
    {
        printf("buffer size:%d, page fault rate:%f\%\n", i, fifo_algorithum(pagestream, length, i) * 100);
        if (fifo_algorithum(pagestream, length, i) * 100 > tmp)
        {
            flag = 1;
            printf("pagestream:");
            for (int i = 0; i < length; i++)
                printf("%d ", pagestream[i]);
            printf("\n");
        }
        tmp = fifo_algorithum(pagestream, length, i) * 100;
    }

    printf("\nBelady anomaly(FIFO):");
    printf("pagestream:");
    for (int i = 0; i < length; i++)
        printf("%d ", pagestream[i]);
    printf("\n");
    int teststream[50] = {0, 3, 4, 5, 0, 1, 0, 1, 2, 5, 6, 8, 9, 6, 0, 1, 2, 5, 2, 1, 2, 3, 0, 4, 7, 5, 0, 5, 6, 7, 9, 6};
    for (int i = 1; i <= 10; i++)
        printf("buffer size:%d, page fault rate:%f\%\n", i, fifo_algorithum(teststream, 32, i) * 100);
    return 0;
}

float optimal_algorithum(const int *pagestream, int length, int buffer_size)
{
    std::vector<int> buffer;
    int page_fault_count = 0;
    for (int i = 0; i < length; i++)
    {
        int inbuffer_flag = 0;
        for (int j = 0; j < buffer.size() && !inbuffer_flag; j++)
            if (buffer[j] == pagestream[i])
                inbuffer_flag = 1;
        // 当前页不在内存中，启动调页算法,否则不用进行置换算法
        if (!inbuffer_flag)
        {
            // 当前块没满，加到栈顶
            if (buffer.size() < buffer_size)
                buffer.push_back(pagestream[i]);
            // 内存满了，用optimal策略进行页表置换
            else
            {
                std::vector<int> index(buffer.size(), -1);
                for (int j = 0; j < buffer.size(); j++)
                {
                    for (int k = i + 1; k < length; k++)
                    {
                        if (pagestream[k] == buffer[j] && index[j] == -1)
                            index[j] = k;
                    }
                }

                int max = index[0], max_index = 0;
                for (int j = 1; j < index.size(); j++)
                {
                    // 该页之后用不到了，直接换出
                    if (index[j] == -1)
                    {
                        max_index = j;
                        break;
                    }
                    else if (index[j] > max)
                    {
                        max = index[j];
                        max_index = j;
                    }
                }
                buffer[max_index] = pagestream[i];
            }
            page_fault_count++;
        }
    }

    return page_fault_count / (float)length;
}

float lru_algorithum(const int *pagestream, int length, int buffer_size)
{
    std::vector<int> buffer;
    int page_fault_count = 0;
    for (int i = 0; i < length; i++)
    {
        int inbuffer_flag = 0;
        for (int j = 0; j < buffer.size() && !inbuffer_flag; j++)
            if (buffer[j] == pagestream[i])
                inbuffer_flag = 1;
        // 当前页不在内存中，启动调页算法,否则不用进行置换算法
        if (!inbuffer_flag)
        {
            // 当前块没满，加到栈顶
            if (buffer.size() < buffer_size)
                buffer.push_back(pagestream[i]);
            // 内存满了，用least recently used策略进行页表置换
            else
            {
                std::vector<int> index(buffer.size(), -1);
                for (int j = 0; j < buffer.size(); j++)
                {
                    for (int k = 0; k < i; k++)
                    {
                        if (pagestream[k] == buffer[j])
                            index[j] = k;
                    }
                }

                int min = index[0], min_index = 0;
                for (int j = 1; j < index.size(); j++)
                {
                    if (index[j] < min && index[j] >= 0)
                    {
                        min = index[j];
                        min_index = j;
                    }
                }
                buffer[min_index] = pagestream[i];
            }
            page_fault_count++;
        }
    }

    return page_fault_count / (float)length;
}

float fifo_algorithum(const int *pagestream, int length, int buffer_size)
{
    std::vector<int> buffer;
    // std::queue<int> fifo_q;
    int front = 0;
    int page_fault_count = 0;
    for (int i = 0; i < length; i++)
    {
        int inbuffer_flag = 0;
        for (int j = 0; j < buffer.size() && !inbuffer_flag; j++)
            if (buffer[j] == pagestream[i])
                inbuffer_flag = 1;
        // 当前页不在内存中，启动调页算法,否则不用进行置换算法
        if (!inbuffer_flag)
        {
            // 当前块没满，加到栈顶
            if (buffer.size() < buffer_size)
            {
                buffer.push_back(pagestream[i]);
                // fifo_q.push(pagestream[i]);
            }
            // 内存满了，用fifo策略进行页表置换
            else
            {
                // fifo_q.push(pagestream[i]);
                // int fifo_first = fifo_q.front();
                // fifo_q.pop();
                // for (int j = 0; j < buffer.size(); j++)
                // {
                // if (buffer[j] == fifo_first)
                // buffer[j] = pagestream[i];
                // }
                buffer[front] = pagestream[i];
                front = (front + 1) % buffer.size();
            }
            page_fault_count++;
        }
    }

    return page_fault_count / (float)length;
}