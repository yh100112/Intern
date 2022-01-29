#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include<unistd.h> // sleep()이 들어있음
void* thread_main(void *arg);
#define _REENTRANT

int main() 
{
	pthread_t t_id;
	int thread_param=5;
	void* thr_ret;
	
  //pthread_create()를 하는 순간 쓰레드는 만들어져서 돌기 시작함
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)
	{
		puts("pthread_create() error");
		return -1;
	}; 	
  usleep(500000);
  printf("start run~\n");

  //main함수에서 생성한 쓰레드를 대상으로 pthread_join을 호출해 main함수는 t_id에 저장된 id의 쓰레드가 종료될 때까지 대기하게 된다.
	if(pthread_join(t_id, &thr_ret)!=0) 
	{
		puts("pthread_join() error");
		return -1;
	};

	printf("Thread return message: %s \n", (char*)thr_ret); 
  // void포인터인 thr_ret을 char형 포인터로 형변환 해준 것. 그런데 전달받은 값이 "Hello, I am thread~" 라는 char형 배열이므로 그냥 배열 이름만 써도 값이 출력된다!!
  //기본형 변수(int,float,... 등등)라면 역참조해야 값이 나온다!! 헷갈리지 말자!!
	free(thr_ret);
	return 0;
}

void* thread_main(void *arg) 
{
  printf("coming run~\n");
	int i;
	int cnt=*((int*)arg); // void포인터를 int형 포인터로 변환 후 역참조 한 것
	char * msg=(char *)malloc(sizeof(char)*50);
	strcpy(msg, "Hello, I am thread~ \n");

	for(i=0; i<cnt; i++)
	{
		sleep(1);  
    puts("running thread");	 
	}
	return (void*)msg; // 반환된 값은 pthread_join()의 두번째 인자로 전달된 변수 thr_ret에 저장된다.그리고 이 반환값은 thread_main()함수 내에서 동적으로 할당된 메모리 공간의 주소 값이다.
}