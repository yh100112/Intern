#include <stdio.h>
#include<pthread.h>
#define NUM_THREAD	100

void * thread_inc(void * arg);
void * thread_des(void * arg);
long long num=0;
pthread_mutex_t mutex; //pthread_mutex_t형 변수 선언 => mutex의 참조값 저장을 위한 변수.전역변수로 선언된 이유는 뮤텍스의 접근이 inc,des 두 개의 함수 내에서 이루어지기 때문이다.

int main(int argc, char *argv[]) 
{
	pthread_t thread_id[NUM_THREAD];
	int i;

  pthread_mutex_init(&mutex, NULL); // 뮤텍스 생성

	for(i=0; i<NUM_THREAD; i++)
	{
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);	
	}	

	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i],NULL);

	printf("result: %lld \n", num);
  pthread_mutex_destroy(&mutex); // 뮤텍스 소멸
	return 0;
}

void * thread_inc(void * arg) 
{
	int i;

    pthread_mutex_lock(&mutex);

	for(i=0; i<50000000; i++)
		num+=1; // 임계영역
    pthread_mutex_unlock(&mutex);
	return NULL;
}
void * thread_des(void * arg)
{
	int i;
	for(i=0; i<50000000; i++){
        pthread_mutex_lock(&mutex);
		num-=1; // 임계영역
        pthread_mutex_unlock(&mutex);
    }
	return NULL;
}