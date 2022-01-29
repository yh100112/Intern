#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
메시지를 담고 있는 메시지 버퍼는 msgbuf 구조체를 사용합니다.
msgbuf 구조체는 <sys/msg.h> 파일에 다음과 같이 정의되어 있습니다.
*/
struct mymsgbuf{  
  long mtype;
  char mtext[80];
};


int main(void){
  
  key_t key;
  int msgid;
  struct mymsgbuf mesg;
  
  //ftok 함수는 path에 지정한 경로명과 id에 지정한 정수값을 조합해 새로운 키를 생성
  key = ftok("keyfile2", 1);
  
  //첫번째 인자인 key에는 IPC_PRIVATE 나 ftok 함수로 생성한 키를 지정
  //IPC_CREAT(0001000) : 새로운 키면 식별자를 새로 생성한다.
  //IPC_EXCL(0002000) : 이미 존재하는 키면 오류가 발생한다.
  msgid = msgget(key, IPC_CREAT|0644);
  if (msgid == -1) {
    perror("msgget");
    exit(1);
  }
  
  
  // 전송할 메시지 버퍼를 설정한다.
  mesg.mtype = 1;     // 메시지 유형을 1로 정의
  strcpy(mesg.mtext, "메시지 Q Test"); // 메시지 버퍼의 배열에 문자열을 복사.
  
  // msgsnd 함수를 사용해 메시지를 전송한다.
  if (msgsnd(msgid, (void *)&mesg, sizeof(mesg.mtext), IPC_NOWAIT) == -1) {
      perror("msgsnd");
      exit(1);
  }
  
  return 0;
}