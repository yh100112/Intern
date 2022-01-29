#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

void error_handling(char* message);
void send_file(int* arg,char* file_name,char* file_name2);

#define BUF_SIZE 100
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


  char file_name[BUF_SIZE] = {0, };
  char file1[BUF_SIZE] = {0, };
  char file2[BUF_SIZE] = {0, };
  char* nextptr;

  printf("보낼 파일명 : ");
  scanf("%[^\n]s",file_name);
  strcpy(file1,strtok_r(file_name," ",&nextptr));
  strcpy(file2,strtok_r(NULL," ",&nextptr));

  
  send_file(&serv_sock,file1,file2);
  close(serv_sock); 
  return 0;
}



void send_file(int* arg,char* file_name,char* file_name2){
  int serv_sock = *arg;
  int length;
  int n,p;
  int file_size;
  char file_comment[BUF_SIZE] = {0,};
  char file_comment2[BUF_SIZE] =  {0,};

  FILE* fp = fopen(file_name,"rb");
  if(fp == NULL){
    error_handling("file이 존재하지 않습니다.\n");
  }
  fseek(fp,0,SEEK_END);
  p = ftell(fp);
  fseek(fp,0,SEEK_SET);
  file_size = htonl(p); // 파일 사이즈

  n = strlen(file_name); 
  length = htonl(n); // 파일이름 길이
  printf("length :%d / n : %d\n",length,n);
  long str_len;

  str_len = write(serv_sock,&length,sizeof(int)); //파일이름길이
  printf("str_len :%d\n",str_len);
  write(serv_sock,file_name,n);//파일 이름 전송

  write(serv_sock,&file_size,sizeof(int));  // 파일 크기 전송 (4바이트)

  fread(file_comment,sizeof(char),file_size,fp);
  write(serv_sock,&file_comment,p); // 파일내용 전송
  printf("file_content1 : %s\n",file_comment);


  fclose(fp);

  //---------------------------------------------------------------------

  FILE* fp2 = fopen(file_name2,"rb");
  if(fp2 == NULL){
    error_handling("file이 존재하지 않습니다.\n");
  }
  fseek(fp2,0,SEEK_END);
  p = ftell(fp2);
  fseek(fp2,0,SEEK_SET);
  file_size = htonl(p); // 파일 사이즈

  n = strlen(file_name2); 
  length = htonl(n); // 파일이름 길이
  write(serv_sock,&length,sizeof(int)); // 파일이름 길이 전송  (4바이트)
  write(serv_sock,file_name2,n);//파일 이름 전송

  write(serv_sock,&file_size,sizeof(int));  // 파일 크기 전송 (4바이트)


  fread(file_comment2,sizeof(char),file_size,fp2);
  write(serv_sock,&file_comment2,p); // 파일내용 전송
  printf("file_content2 : %s\n",file_comment2);


  fclose(fp2);
}

void error_handling( char* errMsg){
  fputs(errMsg,stderr);
  exit(1);
}