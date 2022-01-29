#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]){
    int serv_sock; //클라이언트소켓
    char message[BUF_SIZE]; // 서버로 보낼 메시지를 저장할 변수
    int str_len;
    struct sockaddr_in serv_addr; // 서버 정보(서버의 ip,port)를 저장할  sockaddr_in 구조체 변수

    if(argc != 3){
        printf("Usage : %s <IP> <port>\n",argv[0]);
        exit(1);
    }

    serv_sock = socket(AF_INET,SOCK_STREAM,0); // 클라이언트 소켓 생성
    if(serv_sock == -1){
        printf("socket error\n");
    }

    memset(&serv_addr,0,sizeof(serv_addr)); //연결할 서버의정보를 저장할 sockaddr_in 구조체변수를 0으로 초기화
    serv_addr.sin_family = AF_INET; // ipv4
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]); //연결할 서버 주소 "127.0.0.1" 를 네트워크 바이트 순서의 32비트 ip 값으로 변경해서 넣어줌 
    serv_addr.sin_port = htons(atoi(argv[2])); //연결할 서버의 포트번호를 입력받고 그걸 빅엔디안으로 변경

    //serv_addr에 담겨있는 정보에 해당하는 서버에 클라이언트소켓의 연결을 요청
    if(connect(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1){
        printf("connect error\n");
    }else{
        puts("connected...");
    }

    while(1){
        fputs("Input message ( Q to quit ) : ",stdout); // 표준출력에 입력
        fgets(message, BUF_SIZE,stdin);  //표준출력에 입력한 메시지를 표준입력으로 message에 저장함

        if(!strcmp(message, "q\n") || !strcmp(message,"Q\n")) break; //q,Q를 누르면 반복문 탈출

        write(serv_sock,message,strlen(message));//연결된 서버에게 write()로 message변수에 저장한 데이터를 전송
        str_len = read(serv_sock,message,BUF_SIZE - 1); //연결된 서버가 보낸 데이터를 읽어서 message변수에 저장
                                                        //read()는 읽어온 데이터의 길이를 반환함
        message[str_len] = 0; //char배열이니까 마지막에 공백문자 넣어줌
        printf("Message from server :%s",message);
    }

    printf("client 종료\n"); // 클라이언트에서 q를 입력하면 바로 while문 빠져나와 종료된다.
    close(serv_sock);
    
    return 0;

}