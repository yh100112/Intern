#include <stdio.h>
#include <pthread.h>
void * thread_summation(void * arg); 

#define _REENTRANT

int sum=0;

int main()
{
	pthread_t id_t1, id_t2;
	int range1[]={1, 5};
	int range2[]={6, 10};
	
	pthread_create(&id_t1, NULL, thread_summation, (void *)range1); // 4번째 인자 : 주소값을 전달해야 하는데 range1은 배열이므로 이름 자체가 주소값이므로 &를 안 붙인거임!! 기본 변수들(int,double ..)은 & 붙여야함!!
	pthread_create(&id_t2, NULL, thread_summation, (void *)range2);

	//6~10이 먼저 계산될 수도 있고, 1~5가 먼저 계산될 수도 있다. 하지만 분명한 건 두개 다 끝날 때까지 main함수는 블로킹 상태가 된다.
	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);
	printf("result: %d \n", sum);
	return 0;
}

void * thread_summation(void * arg) 
{
	int start=((int*)arg)[0]; // 배열을 역참조할 때는 * 또 안써줌!! * 를써주면 그 배열의 시작주소에 해당하는 값을 출력하는 것임!!
	int end=((int*)arg)[1];

	while(start<=end)
	{
		printf("%d\n",start);
		sum+=start;
		start++;
	}
	return NULL;
}

//두 쓰레드가 하나의 전역변수 sum에 접근한다!   =>  전역변수가 저장되는 데이터 영역을 두 쓰레드가 함께 공유하기 때문에 가능!!!