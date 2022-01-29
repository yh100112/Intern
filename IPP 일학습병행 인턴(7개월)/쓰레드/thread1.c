#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

void* thread_main(void *arg);

int main(){
  pthread_t t_id;
  int thread_param = 5;

  // thread_main함수의 호출을 시작으로 별도의 실행흐름을 구성하는 쓰레드의 생성을 요청  
  if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param) != 0){ 
      puts("pthread_create() error");
      return -1;
  }
  sleep(10); // 메인함수의 실햏을 10초간 중지시킴.16행의 return이 실행되면 프로세스는 종료됨. 
            //프로세스의 종료는 그 안에서 생성된 쓰레드의 종료로 이어짐. 따라서 쓰레드의 실행을 보장하기 위해 이 문장 삽입.
            
  puts("end of main");
  return 0;
}

void* thread_main(void *arg){ // arg에전달되는 것 : pthread_create()함수의 4번째 인자의 주소값이 전달됨
  int i;
  int cnt = *((int*)arg);
  for(i=0;i<cnt;i++){
    sleep(1);
    puts("running thread");
  }

  return NULL;
}