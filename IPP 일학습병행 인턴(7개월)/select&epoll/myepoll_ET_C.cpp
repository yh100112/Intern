#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
using namespace std;

void error_handling(const char* message);
void send_file(int* arg);

#define BUF_SIZE 1024

int main(int argc,char* argv[]){
  int serv_sock, fd;
  struct sockaddr_in serv_addr;

  serv_sock = socket(AF_INET,SOCK_STREAM,0);
  if(serv_sock == -1){
    error_handling("socket error!!\n");
  }

  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_addr.sin_port = htons(atoi(argv[2]));

  if(connect(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1){
    error_handling("connect error!!\n");
  }else{
    fputs("server connect success!!\n",stdout);
  }
  send_file(&serv_sock);
  close(serv_sock); // 이부분에서 소켓을 닫을 때의 신호까지 read()함수는 읽어온다. 
                    // 그렇기 때문에 이 읽어온 소켓을 닫는 신호도 select()는 하나의 이벤트로 받아들인다.
                    // 그렇기 때문에 읽어온 데이터 message의 바이트 수 다음에도 select()를 탈출해 한번 더 신호를 보낸다(이때의 신호는 read()의 반환값이 0이다)
  
  return 0;
}



void send_file(int* arg){
  int serv_sock = *arg;
  char file_name[BUF_SIZE] = {0, };
  char message[BUF_SIZE] = {0, };
  printf("보낼 파일명 : ");
  scanf("%s",file_name);    
    
  FILE* fp = fopen(file_name,"rb");
  if(fp == NULL){
    printf("file이 존재하지 않습니다.\n");
    return;
  }
  fread(message,sizeof(char),BUF_SIZE,fp);
  write(serv_sock,message,strlen(message));
}

void error_handling(const char* errMsg){
  fputs(errMsg,stderr);
  exit(1);
}