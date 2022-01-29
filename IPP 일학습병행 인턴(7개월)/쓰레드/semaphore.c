/*
    쓰레드 a가 프로그램 사용자로부터 값을 입력받아서 전역변수 num에 저장을 하면, 쓰레드 b는 이 값을 가져다가 누적해 나간다.
    이 과정은 총 5회 진행이 되고, 진행이 완료되면 총 누적금액을 출력하면서 프로그램은 종료된다.
    => 변수 num의 접근이 쓰레드a, 쓰레드b의 순으로 이뤄져야 함. 동기화 필요!
*/
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
void* read(void* arg);
void* accu(void* arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(int argc, char* argv[]){
    pthread_t id_t1, id_t2;
    //세마포어를 두개 생성하고 있다. 1개만 있으면 블로킹상태가 됨.
    sem_init(&sem_one,0,0); 
    sem_init(&sem_two,0,1);

    pthread_create(&id_t1,NULL,read,NULL);
    pthread_create(&id_t2,NULL,accu,NULL);

    pthread_join(id_t1,NULL);
    pthread_join(id_t2,NULL);

    sem_destroy(&sem_one);
    sem_destroy(&sem_two);
    return 0;
}

void* read(void* arg){
    for(int i=0;i<5;i++){
        fputs("Input num: ",stdout);

        sem_wait(&sem_two);
        scanf("%d", &num);
        sem_post(&sem_one);
    }
    return NULL;
}

void* accu(void* arg){
    int sum=0;
    for(int i=0;i<5;i++){
        sem_wait(&sem_one);
        sum += num;
        sem_post(&sem_two);
    }
    printf("Result : %d \n", sum);
    return NULL;
}

