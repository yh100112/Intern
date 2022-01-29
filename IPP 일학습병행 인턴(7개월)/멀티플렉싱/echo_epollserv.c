#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>

#define BUF_SIZE 100
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

    struct epoll_event* ep_events; // epoll_wait()에서 이벤트가 발생한 fd가 채워질 epoll_event[] ( 동적으로 생성 )
    struct epoll_event event; // epoll_ctl()에서 fd를 epoll인스턴스에 추가,삭제할때 관찰 이벤트 유형(이벤트가 발생하는 상황)을 지정해주기 위해 사용
    int epfd, event_cnt;
    

    epfd = epoll_create(EPOLL_SIZE); // epoll 인스턴스 생성
    ep_events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * EPOLL_SIZE); // epoll_event[] 생성

    event.events = EPOLLIN; //수신할 데이터가 존재할 때
    event.data.fd = serv_sock;
    epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event); // epoll 인스턴스 epfd에 serv_sock(fd)를 추가하는데, 수신할 데이터가 존재하는 상황에서만 이벤트가 발생하도록 함!!

    while(1){
      event_cnt = epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);  
      // 여기서 epoll인스턴스 epfd에 속한 fd들 중 이벤트가 발생할 때까지 대기한다.클라이언트가 접속해 이벤트가 발생하면 값을 반환한다.
      // select와 달리 매번 호출할 때마다 관찰대상의 기준(새롭게 최신화된 fd_set변수)을 운영체제에 보내줄 필요가 없다.

      if(event_cnt == -1){
        puts("epoll_wait error");
        break;
      }

      //이벤트가 발생할 떄마다 ep_events에 순서대로 이벤트가 발생한 fd의 정보가 구조체로 들어간다.
      for(i = 0;i<event_cnt; i++){//(이벤트가 발생한 fd 갯수만큼만 돔)
        if(ep_events[i].data.fd == serv_sock){
          adr_sz = sizeof(clnt_adr);
          clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr, &adr_sz);
          event.events = EPOLLIN;//수신할 데이터가 존재할 때
          event.data.fd = clnt_sock;
          epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock,&event); // epoll 인스턴스 epfd에 clnt_sock을 추가하는데, 수신한 데이터가 존재할 때만 이벤트가 발생하도록 함!!
          printf("connected client: %d \n",clnt_sock);
        }else{
          str_len = read(ep_events[i].data.fd,buf,BUF_SIZE);
          printf("str_len :%d\n",str_len);
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