#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<errno.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 50

void setnonblockingmode(int fd);
void error_handling(const char* errMsg);

int main(int argc, char* argv[]){
  int serv_sock,clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;
  socklen_t clnt_adr_sz;
  int str_len, i;

  struct epoll_event* ep_events;
  struct epoll_event event;
  int epfd, event_cnt;

  serv_sock = socket(AF_INET,SOCK_STREAM,0);
  memset(&serv_adr,0,sizeof(serv_adr));
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_adr.sin_port = htons(atoi(argv[1]));

  if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr)) == -1) { error_handling("bind error");}
  if(listen(serv_sock,5) == -1) { error_handling("listen error");}

  epfd = epoll_create(EPOLL_SIZE);
  ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

  setnonblockingmode(serv_sock); // 넌블로킹으로 설정함
  event.events = EPOLLIN; // 수신할 데이터가 존재하는 상황
  event.data.fd = serv_sock;
  epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);
  
  while(1){
    event_cnt = epoll_wait(epfd,ep_events, EPOLL_SIZE, -1);
    if(event_cnt == -1){puts("epoll_wait error!");break;}

    printf("event 발생\n");
    for(i = 0; i < event_cnt; i++){ 
      if(ep_events[i].data.fd == serv_sock){//서버에 클라이언트 접속 연결
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        setnonblockingmode(clnt_sock);
        event.events = EPOLLIN | EPOLLET; // 수신할 데이터가 존재하는 상황 , 이벤트의 감지를 엣지트리거 방식으로 설정
        event.data.fd = clnt_sock;
        epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);
        printf("connected client : %d\n", clnt_sock);
      }else{
        int n,p = 0;
        int file_name_length = 0; // 파일 이름 길이 저장
        char* file_name;
        int file_size = 0;
        char file_comment[BUF_SIZE] = {0,};
        char file_recv[BUF_SIZE] = {0,};
        int nsize = 0;
        int cnt = 0;

        while(1){
          printf("while문\n");
          str_len = read(ep_events[i].data.fd,&n,sizeof(int)); // 파일이름 길이 읽어옴( 4바이트 만큼만 )
          if(str_len == 0){
            epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
            close(ep_events[i].data.fd);
            printf("closed client : %d\n",ep_events[i].data.fd);
            break;
          }else if(str_len < 0){ // 버퍼가 비어있다는 뜻
            if(errno == EAGAIN){
              printf("-1 come in~(버퍼에 읽을게 더 없을때)\n");
              break;
            }
          }else{
            printf("----------------------------\n");
            file_name_length = ntohl(n); // htonl()로 전송된 파일이름 길이를 다시 리틀엔디안으로 변환해서 받음
            file_name = (char*)malloc(file_name_length + 1); // 파일 이름 저장할 변수 동적할당
            memset(file_name,0,sizeof(file_name));
            printf("읽은 바이트수 : %d / 파일 이름 길이 : %d\n",str_len, file_name_length);
            
            str_len = read(ep_events[i].data.fd,file_name,file_name_length);  // 파일 이름 읽어옴
            printf("읽은 바이트수 : %d / file_name : %s\n",str_len,file_name);

            str_len = read(ep_events[i].data.fd,&p,sizeof(int));  // 파일 사이즈 읽어옴
            file_size = ntohl(p); // htonl()로 전송된 파일사이즈를 다시 리틀엔디안으로 변환해서 받음
            printf("읽은 바이트수 : %d / file_size : %d\n",str_len, file_size);

            str_len = read(ep_events[i].data.fd,file_comment,file_size); // 파일 내용 읽어옴
            printf("읽은 바이트수 : %d / file_comment : %s\n",str_len,file_comment);

            sprintf(file_recv,"../../../C_C++_Projects/%s",file_name);            
            FILE* fp = fopen(file_recv,"wb");
            if(fp == NULL){
              error_handling("file_open_error");
            }
            while(nsize != file_size){
              cnt = fwrite(file_comment,sizeof(char),strlen(file_comment),fp);
              nsize += cnt;
            }
            fclose(fp);
          }
        }
        free(file_name);
      }
    }
  }
}

void setnonblockingmode(int fd){
  int flag = fcntl(fd,F_GETFL,0);
  fcntl(fd,F_SETFL, flag | O_NONBLOCK);
}
void error_handling(const char* errMsg){
  fputs(errMsg,stderr);
  fputc('\n',stderr);
  exit(1);
}