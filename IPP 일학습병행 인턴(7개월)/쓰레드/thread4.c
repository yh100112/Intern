#include <stdio.h>
#include<pthread.h>
#define NUM_THREAD	100

//이 예제는 실제로 num에 0 이 저장되어야 하는데 
//0이 아니고 실행할 때마다 매번 그 결과도 다르다. 
//이건 임계영역 관련해 문제가 있는 코드이다!

void * thread_inc(void * arg);
void * thread_des(void * arg);
long long num=0;

int main(int argc, char *argv[]) 
{
	pthread_t thread_id[NUM_THREAD]; //쓰레드를 담을 배열 선언
	int i;

	printf("sizeof long long: %d \n", sizeof(long long));
	for(i=0; i<NUM_THREAD; i++)
	{
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);	
	}	

	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);

	printf("result: %lld \n", num);
	return 0;
}

void * thread_inc(void * arg) 
{
	int i;
	for(i=0; i<50000000; i++)
		num+=1;
	return NULL;
}
void * thread_des(void * arg)
{
	int i;
	for(i=0; i<50000000; i++)
		num-=1;
	return NULL;
}