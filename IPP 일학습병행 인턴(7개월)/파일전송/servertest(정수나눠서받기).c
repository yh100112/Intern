#include<stdio.h>
#include<stdlib.h> // atoi를 사용하려면 있어야 함
#include<string.h> // memset 
#include<unistd.h> // sockaddr_in, read, write 등
#include<arpa/inet.h> // htonl, htons, INADDR_ANY, sockaddr_in 등
#include<sys/socket.h> //socklen_t

void error_handling(char* message);

int main(int argc, char *argv[]){
    int serv_sock;
    int clnt_sock;

    //sockaddr_in은 소켓주소의 틀을 형성해주는 구조체로 AF_INET일 경우 사용
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr; // accept 함수에서 사용됨
    socklen_t clnt_addr_size; // socklen_t : 소켓 관련 매개 변수에 사용되는 것으로 길이 및 크기 값에 대한 정의를 내려줌

    //tcp연결지향형이고 ipv4 도메인을 위한 소켓 생성
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1) {error_handling("socket error");}

    //주소를 초기화한 후 IP주소와 포트 지정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    //소켓과 서버 주소를 바인딩
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) error_handling("listen error");

    //연결 대기열 5개 생성
    if(listen(serv_sock, 5) == -1){
        error_handling("listen error");
    }

    //클라이언트로부터 요청이 오면 연결 수락
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(clnt_sock == -1){
        error_handling("accept error");
    }
    printf("연결 성공!\n");

    int file_name_len;
    char* recv_data = (char*)&file_name_len;
    read(clnt_sock,recv_data,1);
    printf("recv_data : %d\n",file_name_len);
    read(clnt_sock,recv_data + 1,3);
    printf("recv_data2 : %d\n",file_name_len);



    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char* message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}