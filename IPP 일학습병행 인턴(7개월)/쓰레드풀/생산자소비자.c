#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<unistd.h>

#define Q_MAX 16

//큐 구성 시작
//fifo 구조
typedef struct queue_t{
    int data[Q_MAX];
    int nitem;
    int head;
    int tail;

    pthread_mutex_t mutex;
    pthread_cond_t consumer_cond;
    pthread_cond_t producer_cond;
}queue_t;

queue_t queue = {
    .producer_cond = PTHREAD_COND_INITIALIZER,
    .consumer_cond = PTHREAD_COND_INITIALIZER,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .head = 0,
    .tail = 0
};

int is_full(){
    return queue.nitem == Q_MAX - 1; // true : 1 false : 0 => true면 꽉 찼다는 뜻
}

int is_empty(){
    return queue.nitem == 0; //  true면 비어있따는 뜻
}

void put(int d){
    queue.data[queue.head] = d;
    queue.head = (queue.head + 1)%Q_MAX;
    queue.nitem++;
}

int get(){
    int temp = queue.data[queue.tail];
    queue.tail = (queue.tail+1)%Q_MAX;
    queue.nitem--;
    return temp;
}

//큐 구성 끝

void* produce(void* arg){
    int i=0;
    while(1){
        pthread_mutex_lock(&queue.mutex);
        if(is_full()){
            printf("꽉 찼다!! 대기해\n");
            pthread_cond_wait(&queue.consumer_cond,&queue.mutex);
        }

        printf("produce:%d\n", i);

        put(i);
        pthread_cond_signal(&queue.producer_cond);

        pthread_mutex_unlock(&queue.mutex);
        i++;

        if(i == 100) break;
        usleep(rand()%1000);
    }
    return 0;
}

void* consume(void* arg){
    while(1){
        pthread_mutex_lock(&queue.mutex);

        if(is_empty()){
            printf("비었어 빨리 만들어놔\n");
            pthread_cond_wait(&queue.producer_cond,&queue.mutex);
        }
        int item = get();
        pthread_cond_signal(&queue.consumer_cond);
        printf("\t\tconsumer : %d\n",item);

        pthread_mutex_unlock(&queue.mutex);

        usleep(rand()%1000);
    }

    return 0;
}

int main(int argc, char* argv[]){
    int n;
    pthread_t producer,consumer;
    srand(time(0));

    pthread_create(&producer, NULL, &produce, NULL);
    pthread_create(&consumer,NULL,&consume,NULL);

    pthread_join(producer,NULL);
    pthread_join(producer,NULL);

    return 0;

}

























