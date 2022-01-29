#include<stdio.h>
#include<unistd.h>
#include<signal.h>

void sigint_handler(int signo){
  int ndx;
  printf("Ctrl-C 키를 누르셨죠.\n");
  printf("3초간 대기하겠습니다. 이때 Ctrl-Z키를 눌러주세요.\n");

  for(ndx = 3; 0 < ndx; ndx--){
    printf("%d 초 남았습니다.\n",ndx);
    sleep(1);
  }
}

int main(){
  struct sigaction act;

  act.sa_handler = sigint_handler; // 시그널 핸들러 지정
  sigfillset(&act.sa_mask); // 모든 시그널을 블록 => 시그널이 동작하는 도중에 다른 시그널이 들어와도 먼저 동작한 시그널이 끝날 때까지 대기함

  sigaction(SIGINT, &act, NULL);
  while(1){
    printf("badyak.com\n");
    sleep(1);
  }
}