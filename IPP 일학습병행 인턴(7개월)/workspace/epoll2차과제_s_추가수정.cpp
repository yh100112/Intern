#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<errno.h>
#include<map>
#include<iostream>
using namespace std;


#define BUF_SIZE 100
#define EPOLL_SIZE 50

void setnonblockingmode(int fd);
void error_handling(const char* errMsg);

enum current_state_e{
  current_state_filename_len = 0,
  current_state_filename,
  current_state_file_size,
  current_state_file_content,
};

struct FileReadData
{
  int current_state; // 현재 읽기상태
  char* file_name;
  int file_size;
  int file_name_len; // 얘를 0으로 초기화하면 왜 값이 계속 0일까?
  int goal_read_len; // 현재 상태에서 읽어야 할 전체 데이터의 길이
  int read_len_curr; // 현재 상태에서 read()를 반복하며 읽어들인 데이터의 길이
  char file_content[BUF_SIZE] = {0,};// 파일 내용
  char* read_curr_ptr; // 현재 읽기 상태에서 읽은 데이터를 저장할 메모리의 시작주소

  FileReadData():current_state(current_state_filename_len),
                read_len_curr(0),
                file_name(NULL),
                file_name_len(0),
                file_size(0),
                goal_read_len(sizeof(int)),
                read_curr_ptr( (char*)&file_name_len )
                {};

  ~FileReadData(){
    if(file_name != NULL){
      // free(file_name);
    } 
  }
};


int main(int argc, char* argv[]){
  int serv_sock,clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;
  socklen_t clnt_adr_sz;
  int i;
  map<int,struct FileReadData> m;

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
      if(ep_events[i].data.fd == serv_sock)
      {
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
        setnonblockingmode(clnt_sock);
        event.events = EPOLLIN | EPOLLET; // 수신할 데이터가 존재하는 상황 , 이벤트의 감지를 엣지트리거 방식으로 설정
        event.data.fd = clnt_sock;
        epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);

        m[clnt_sock]; 
        printf("connected client : %d\n", clnt_sock);
        printf("map : ");
        for(pair<int,struct FileReadData> atom : m){
          printf("%d ",atom.first);
        }
        printf("\n");
      }
      else
      {
        struct FileReadData& fileData = m[ep_events[i].data.fd];
        long read_len;
        while(1){
          read_len = read(ep_events[i].data.fd, fileData.read_curr_ptr+fileData.read_len_curr, fileData.goal_read_len - fileData.read_len_curr);

          if(read_len == 0){
            epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
            close(ep_events[i].data.fd);
            m.erase(ep_events[i].data.fd);
            printf("closed client : %d\n",ep_events[i].data.fd);
            break;
          }
          else if(read_len < 0)
          {
            if(errno == EAGAIN){
              printf("-1\n"); 
              break;
            }
          }
          else{
            FILE* fp = NULL;
            char file_recv[100];
            if(fileData.current_state == current_state_file_content){
              sprintf(file_recv,"../../../project/%s",fileData.file_name);
              fp = fopen(file_recv,"ab");
              fwrite(fileData.read_curr_ptr,sizeof(char),read_len,fp);
              fclose(fp);
            }

            fileData.read_len_curr += read_len; //현재까지 읽어온 길이 (read_len_curr)

            if(fileData.goal_read_len - fileData.read_len_curr <= 0){ 
              fileData.read_len_curr = 0;
              switch(fileData.current_state)
              {
                case current_state_filename_len:
                  fileData.file_name_len = ntohl(fileData.file_name_len); //읽어온 파일 길이를 file_name_len에 리틀엔으로변환해 저장
                  fileData.file_name = (char*)malloc(fileData.file_name_len + 1); // file_name을 파일 길이만큼 동적할당
                  fileData.read_curr_ptr = fileData.file_name; // 읽어온 데이터를 저장할 변수의 시작주소를 file_name으로 변경
                  fileData.goal_read_len = fileData.file_name_len; // 현재 읽기상태에서 읽어와야할 최종 데이터 길이를 file_name_len로 변경
                  fileData.current_state = current_state_filename; // 다음 읽기 상태로 변경
                  printf("read_len : %d / 파일 이름 길이 : %d\n",read_len,fileData.file_name_len);
                  break;
                case current_state_filename:
                  memset(file_recv,0,sizeof(file_recv));
                  sprintf(file_recv,"../../../project/%s",fileData.file_name);
                  fp = fopen(file_recv,"wb");
                  if(fp != 0) {
                    fclose(fp);
                  }
                  fileData.read_curr_ptr = (char*)&fileData.file_size;
                  fileData.goal_read_len = sizeof(int);
                  fileData.current_state = current_state_file_size;
                  printf("read_len : %d / 파일 이름 : %s\n",read_len, fileData.file_name);
                  break;
                case current_state_file_size:
                  fileData.read_curr_ptr = fileData.file_content;
                  fileData.file_size = ntohl(fileData.file_size);
                  fileData.goal_read_len = fileData.file_size;
                  fileData.current_state = current_state_file_content;
                  printf("read_len : %d / 파일 사이즈 : %d\n",read_len, fileData.file_size);
                  break;
                case current_state_file_content:
                  fileData.read_curr_ptr = (char*)&fileData.file_name_len;
                  fileData.goal_read_len = sizeof(int);
                  fileData.current_state = current_state_filename_len;
                  printf("read_len : %d / 파일 내용 : %s\n",read_len, fileData.file_content);
                  free(fileData.file_name);
                  memset(fileData.file_content,0,sizeof(fileData.file_content));
                  printf("--------------------------------------\n");
                  break;
              }
            }

          }
        }
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