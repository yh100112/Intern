#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<time.h>
#include<sys/time.h>
#include<map>
#include<iostream>
#include<string>
using namespace std;

void error_handling(const char* message);


int main(int argc, char* argv[]){
  int serv_sock,clnt_sock;
  struct sockaddr_in serv_addr, client_addr;
  socklen_t clnt_addr_size;
  struct timeval timeout;
  fd_set reads,reads_copy;
  map<int,string> client_map;

  serv_sock = socket(AF_INET,SOCK_STREAM,0);

  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if(bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
    error_handling("bind error!!\n");
  }
  if(listen(serv_sock, 5) == -1){
    error_handling("listen error!!\n");
  }

  FD_ZERO(&reads);
  FD_SET(serv_sock,&reads);//fd_set 변수 reads에 serv_sock 등록

  int fd_max = serv_sock;
  int fd_num,str_len;
  char message[1024];


  while(1){
    reads_copy = reads;
    timeout.tv_sec = 5;
    timeout.tv_usec = 5000;

    if((fd_num = select(fd_max + 1,&reads_copy,0,0,&timeout)) == -1){
      break;
    }
    if(fd_num == 0){
      continue;
    }



    for(int i=0;i < fd_max + 1; i++){
      if(FD_ISSET(i,&reads_copy) > 0){
        if(i == serv_sock){
          clnt_addr_size = sizeof(clnt_addr_size);
          clnt_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);
          FD_SET(clnt_sock,&reads);
          if(fd_max < clnt_sock){
            fd_max = clnt_sock;
          }
          printf("connected client fd : %d\n",clnt_sock);
        }else{
          str_len = read(i,message,sizeof(message));

          printf("Str_len :%d\n",str_len);  //보낸 데이터 바이트가 8인데 8을 읽고 그다음 다시 select()를 탈출해 0이 전달되는 이유
                                            // => read()는 클라이언트부분에서 connect()로 연결한 소켓을 close()로 종료할 때 그 종료한 신호도 읽어들인다. 
                                            //    close()로 소켓을 닫을 때의 신호는 읽어들였을 때 read()가 0을 반환하기 때문에 0으로 또 한번 값이 전달되는 것이다!!!
          if (str_len == 0){ // 읽어올 데이터가 없다는 뜻 => 존재하지 않는 파일명을 치면 당연히 거기서 읽어올 데이터도 없기 때문에 read는 0을 반환
            FD_CLR(i, &reads);
            close(i);
            printf("closed client : %d\n", i);
            client_map.erase(i); // 접속 종료되면  map에서 삭제
            // for (pair<int, string> atom : client_map){
            //   cout << "key : " << atom.first << " / value : " << atom.second << endl;
            // }
          }else{
            time_t tm_time = time(NULL);
            struct tm *tm = localtime(&tm_time);
            char time[100];
            strftime(time, 100, "%Y_%m_%d_%H_%M_%S", tm);
            strcat(time, ".txt");
            FILE *fp2 = fopen(time, "wb");
            if (fp2 == NULL){
              error_handling("file open error!!\n");
            }
            fwrite(message, sizeof(char), strlen(message), fp2);
            client_map[i] = time; // 접속해서 데이터를 보내면 map에 저장
            fclose(fp2);
            
          }
            // time_t tm_time = time(NULL);
            // struct tm *tm = localtime(&tm_time);
            // char time[100];
            // strftime(time, 100, "%Y_%m_%d_%H_%M_%S", tm);
            // strcat(time, ".txt");
            // FILE *fp2 = fopen(time, "wb");
            // if (fp2 == NULL){
            //     error_handling("file open error!!\n");
            // }
            // fwrite(message, sizeof(char), strlen(message), fp2);
            // fclose(fp2);
            // FD_CLR(i, &reads);
            // close(i);
            // printf("closed client : %d\n", i);
        }
      }
    }
  }

  close(serv_sock);
  return 0;
}



void error_handling(const char* errMsg){
  fputs(errMsg,stderr);
  exit(1);
}