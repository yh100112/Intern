#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct mymsgbuf {
  long mtype;
  char mtext[80];
}mymsgbuf;

int main(void) {
  mymsgbuf inmsg;
  key_t key;
  int msgid, len;

  // msgsnd 예제와 같은 경로명과 정수값을 사용해 키를 생성한다.
  key = ftok("keyfile2", 1);
  // msgget 함수의 두번째 인자를 0으로 지정해 기존 메시지 큐의 식별자를 리턴하게 한다.
  if((msgid = msgget(key, 0)) < 0) {
    perror("msgget");
    exit(1);
  }

  // msgrcv 함수를 사용해 메시지를 읽어온다.
  // 버퍼의 크기는 80바이트로 지정하고, 큐가 비었을 경우 기다리도록 지정한다.
  len = msgrcv(msgid, &inmsg, sizeof(inmsg.mtext), 1, 0); // mtype이 1인 메시지를 받아옴
  printf("Received Msg = %s, Len=%d\n", inmsg.mtext, len);

  return 0;
}

/*
msgrcv 함수는 메시지 큐로 메시지를 수신하는 데 사용합니다.
#include <sys/msg.h>
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long int msgtyp, int msgflg);
msqid: msgget 함수로 생성한 메시지 큐 식별자
msgp: 메시지를 담고 있는 메시지 버퍼의 주소
msgsz: 메시지 버퍼의 크기
msgtyp: 읽어올 메시지의 유형
msgflg: 블록 모드(0)/비블록 모드(IPC_NOWAIT)
 */