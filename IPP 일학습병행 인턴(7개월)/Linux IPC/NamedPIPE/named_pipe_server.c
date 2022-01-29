/* Reader */
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#define MSG_SIZE 80
#define PIPENAME "./named_pipe_file"

int main(){
  char msg[MSG_SIZE];
  int fd;
  int nread, rc;

  /* 기존에 named pipe가 있으면 삭제 */
  if(access(PIPENAME, F_OK) == 0){
    unlink(PIPENAME);
  }

  /* named pipe 생성하기  => 여기서 파이프가 생성됨 (파이프의 이름은 named_pope_file)*/
  if((rc = mkfifo(PIPENAME,0666)) < 0){ // 0666 : 읽기 + 쓰기
    printf("fail to make named pipe\n");
    return 0;
  }

  /* named pipe열기, Read Write가능해야 함*/
  /* named pipe파일을 생성할 때 받는 쪽에서 O_RDONLY가 아니라 O_RDWR로 열어야 하는 이유는?
    => 읽기전용으로 열면 보내는 쪽에서 종료를 하면 이제 파이프에 쓰기를 하는 프로세스가 없어진다. 그렇게 되면 read()는 block해서 읽을 게 생길 때 까지
      대기하지 않고 쓰는 프로세스가 없으므로 그냥 0을 바로 반환해버린다. 그래서 무한루프를 돌게 된다. 그렇기 때문에 쓰는 프로세스가 없으면 나라도 쓰겠다. 라고 선언해
      read()가 반환되는 것을 막기 위해 받기만 하는 쪽이라도 O_RDWR로 설정해야 한다!!
   */
  if((fd = open(PIPENAME, O_RDWR)) < 0){ 
    printf("fail to open named pipe\n");
    return 0;
  }

  while(1){
    if((nread = read(fd, msg, sizeof(msg))) < 0){
      printf("fail to call read()\n");
      return 0;
    }
    printf("recv : %s\n", msg);
  }
  close(fd);

  return 0;
}