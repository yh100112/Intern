#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<map>
#include<iostream>
#include<sys/epoll.h>
#include<fcntl.h>
#include<errno.h>
using namespace std;

#define BUF_SIZE 4
#define EPOLL_SIZE 50

void setnonblockingmode(int fd);
void error_handling(const char* buf);
const char* fileName(void);

int main(int argc, char* argv[]){
  int serv_sock,clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;
  socklen_t adr_sz;
  int str_len, i;
  char buf[BUF_SIZE];
  map<int,string> client_map;

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

  epfd = epoll_create(EPOLL_SIZE); //epoll생성
  ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

  setnonblockingmode(serv_sock); // 넌블로킹으로 설정함
  event.events = EPOLLIN;
  event.data.fd = serv_sock;
  epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);

  while(1){

    event_cnt = epoll_wait(epfd,ep_events, EPOLL_SIZE, -1);
    if(event_cnt == -1){
      puts("epoll_wait error!");
      break;
    }

    puts("epoll_wait 이벤트 발생");
    for(i = 0; i < event_cnt; i++){
      if(ep_events[i].data.fd == serv_sock){
        adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr, &adr_sz);
        setnonblockingmode(clnt_sock);
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = clnt_sock;
        epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);
        printf("connected client : %d\n", clnt_sock);

      }else{
        str_len = read(ep_events[i].data.fd, buf, BUF_SIZE);
        while(1){

        }
        
      }
    }
  }


}

void setnonblockingmode(int fd){
  int flag = fcntl(fd,F_GETFL,0);
  fcntl(fd,F_SETFL, flag | O_NONBLOCK);
}

void error_handling(const char* buf){
  fputs(buf,stderr);
  fputc('\n',stderr);
  exit(1);
}