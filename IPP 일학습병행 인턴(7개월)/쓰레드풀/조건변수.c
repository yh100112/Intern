#include<pthread.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#define _REENTRANT

pthread_mutex_t mutex;
pthread_cond_t cond; // 조건변수

int data = 0;

void* increase(void *arg){
  while(1){
    sleep(1);
    pthread_mutex_lock(&mutex);
      data++;
      pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
  }
}

void* printData(void* arg){
  while(1){
    pthread_mutex_lock(&mutex);
      pthread_cond_wait(&cond,&mutex);
      printf("data : %d\n",data);
    pthread_mutex_unlock(&mutex);
  }
}

int main(){
  pthread_t thread1,thread2;

  pthread_mutex_init(&mutex,NULL);
  pthread_cond_init(&cond,NULL);

  pthread_create(&thread1, NULL, increase,NULL);
  pthread_create(&thread2,NULL,printData,NULL);

  pthread_join(thread1,NULL);
  pthread_join(thread2,NULL);

  return 0;

}