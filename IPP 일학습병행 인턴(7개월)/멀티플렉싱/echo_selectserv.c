#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<sys/select.h>

#define BUF_SIZE 4
void error_handling(const char* buf);

int main(int argc, char* argv[]){
  int serv_sock,clnt_sock;
  struct sockaddr_in serv_adr,clnt_adr;
  socklen_t adr_sz;
  struct timeval timeout;
  fd_set reads,cpy_reads;

  char buf[BUF_SIZE];

  serv_sock = socket(AF_INET,SOCK_STREAM,0);
  memset(&serv_adr,0,sizeof(serv_adr));
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_adr.sin_port = htons(atoi(argv[1]));

  if(bind(serv_sock,(struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
      error_handling("bind() error");
  }
  if(listen(serv_sock, 5) == -1){
      error_handling("listen() error");
  }

  FD_ZERO(&reads);
  FD_SET(serv_sock,&reads); // fd_set형 변수 reads에 서버소켓을 등록 => 이로써 데이터의 수신여부를 관찰하는 관찰대상에 서버소켓이 포함됨
                              //클라이언트의 연결요청도 데이터의 전송을 통해 이뤄지므로 서버 소켓으로 수신된 데이터가 존재한다는 것은 연결요청이 있었다는 뜻이로 해석!
  
  int fd_max, str_len, fd_num,i;
  fd_max = serv_sock;

  while(1){
      cpy_reads = reads;
      timeout.tv_sec = 5;
      timeout.tv_usec = 5000;

      if( (fd_num = select(fd_max + 1,&cpy_reads,0,0,&timeout)) == -1){
          break;
      }
      // 여기서 매번 새롭게 관찰대상을 최신화함
      //매번 select를 호출할 때마다 관찰대상에 대한 정보(이번에 이벤트가 발생한 게 누구인지) 전달


      if(fd_num == 0){
          continue;
      }

      printf("fd_num :%d\n",fd_num); // 항상 1임 이벤트가 발생할 때마다 while을 돌기 때문

      //이벤트가 발생할 때마다 전체 fd를 순차검색으로 반복문을 돌려서 상태변화가 있는 fd를 찾아야 한다.
      for(i = 0; i<fd_max + 1; i++){
        if(FD_ISSET(i,&cpy_reads) > 0){ // 상태변화가 있었던 즉, 1인 파일 디스크립터를 찾는다.
            
            printf("select호출 후에도 1인 상태인 1은 들어와~\n"); //클라이언트 연결되고 새로 생긴 클라이언트 소켓 번호 4는 여기로 안들어옴
            
            if(i == serv_sock){ // 상태변화가 확인되면 제일먼저 서버 소켓에서 변화가 있었는지 확인.
                printf("server~\n");
                adr_sz = sizeof(clnt_adr);
                clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr, &adr_sz);//서버 소켓의 상태변화가 맞다면 이어서 연결요청 수락
                FD_SET(clnt_sock,&reads); // 클라이언트와 연결된 소켓의 파일 디스크립터를 fd_set형 변수 reads에 1로 저장
                if(fd_max < clnt_sock){
                    fd_max = clnt_sock;
                }
                printf("connected client : %d \n",clnt_sock);
            }else{ // 상태변화가 발생한 소켓이 서버소켓이 아닌 경우. 클라이언트 소켓인 경우 실행
                str_len = read(i,buf,BUF_SIZE); // i가 clnt_sock이다.
                if(str_len == 0){ // 수신한 데이터가 eof인 경우 소켓 종료하고 변수 reads에서 해당정보를 삭제
                    FD_CLR(i,&reads); // 보낸 데이터가 eof면 전송이 다 끝났으므로 fd_set에서 i, 즉 clnt_sock을 0으로 바꿔줌
                    close(i); // 통신이 끝난 clnt_sock 닫음
                    printf("closed client : %d \n",i);
                }else{ // 수신한 데이터가 문자열 데이터
                    write(i,buf,str_len);
                }
            }
        
        }
      }
  }
  close(serv_sock);
  return 0;
    
}

void error_handling(const char *buf){
    fputs(buf,stderr);
    fputc('\n',stderr);
    exit(1);
}