#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>

/*
echo_epollserv.c와 다른점
1. read함수호출시 사용할 버퍼의 크기를 4바이트로 축소
    =>버퍼 크기를 줄인 이유 : 입력버퍼에 수신된 데이터를 한번에 읽어 들이지 못하게 하기 위함
    =>즉, read호출 이후에도 입력버퍼에는 여전히 읽어들일 데이터가 존재할 것이고, 이로 인해 새로운 이벤트가 등록 되어서 epoll_wait함수가 반환을 한다면,
      문자열 "return epoll_wait"이 반복 출력될 것이다.
2. epoll_wait함수의 호출횟수를 확인하기 위한 문장 삽입
*/
#define BUF_SIZE 4 
#define EPOLL_SIZE 50
void error_handling(const char* buf);

int main(int argc, char* argv[]){
  int serv_sock,clnt_sock;
  struct sockaddr_in serv_adr,clnt_adr;
  socklen_t adr_sz;
  int str_len, i;
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

  struct epoll_event* ep_events; 
  struct epoll_event event; 
  int epfd, event_cnt;
  

  epfd = epoll_create(EPOLL_SIZE); 
  ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*EPOLL_SIZE); 

  event.events = EPOLLIN; //수신할 데이터가 존재할 때
  event.data.fd = serv_sock;
  epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event); // epoll 인스턴스 epfd에 serv_sock(fd)를 추가하는데, 수신할 데이터가 존재하는 상황에서만 이벤트가 발생하도록 함!!

  while(1){
    event_cnt = epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);  

    if(event_cnt == -1){
      puts("epoll_wait error");
      break;
    }

    puts("return epoll_wait");
    for(i = 0;i<event_cnt; i++){//(이벤트가 발생한 fd 갯수만큼만 돔)
      if(ep_events[i].data.fd == serv_sock){
        adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr, &adr_sz);
        // event.events = EPOLLIN;//수신할 데이터가 존재할 때
        event.events = EPOLLIN | EPOLLET; // 엣지 트리거 모델의 이벤트 등록방식만 확인해보자! => return epoll_wait이 한번만 출력되는걸로 보아 이벤트가 딱 한번 등록됨을 확인 가능하다.
                                          // 하지만 클라이언트의 실행결과에는 4만큼만 읽고 다음 입력을 넣어도 입력버퍼에 남은 값들이 순서대로 4씩 오게 되어 문제가 발생한다. 이유가 뭘까?
        event.data.fd = clnt_sock;
        epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock,&event); // epoll 인스턴스 epfd에 clnt_sock을 추가하는데, 수신한 데이터가 존재할 때만 이벤트가 발생하도록 함!!
        printf("connected client: %d \n",clnt_sock);
      }else{
        str_len = read(ep_events[i].data.fd,buf,BUF_SIZE);
        if(str_len == 0){
          epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd,NULL);
          close(ep_events[i].data.fd);
          printf("closed client : %d \n", ep_events[i].data.fd);
        }else{
          write(ep_events[i].data.fd,buf,str_len); // echo!!
        }
      }
    }
  }
  close(serv_sock);
  close(epfd);
  return 0;
    
}

void error_handling(const char *buf){
  fputs(buf,stderr);
  fputc('\n',stderr);
  exit(1);
}