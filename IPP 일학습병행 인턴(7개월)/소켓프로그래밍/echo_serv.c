#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]){
    int serv_sock,clnt_sock;//서버소켓,클라이언트소켓
    char message[BUF_SIZE]; // 클라이언트에서 받은 메시지를 저장할 변수
    int str_len, i;

    struct sockaddr_in serv_addr; // 서버소켓 정보담고있는 변수
    struct sockaddr_in clnt_addr; //클라이언트소켓 정보담고있는 변수
    socklen_t clnt_addr_size;     

    if(argc != 2){
        printf("Usage :%s <port>\n",argv[0]);
        exit(1);
    }

    serv_sock = socket(AF_INET,SOCK_STREAM,0); // 서버소켓 생성 (ipv4,tcp형태로)
    if(serv_sock == -1){
        printf("socket error\n");
    }

    memset(&serv_addr,0,sizeof(serv_addr)); // 서버정보를 담을 구조체sockaddr_in을 0으로 초기화해줌
    serv_addr.sin_family = AF_INET;         //ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//현재 이프로그램이 돌아가는 곳의 주소를 빅엔디안으로변경
    serv_addr.sin_port = htons(atoi(argv[1])); //포트번호를 입력받고 그걸 숫자로변경후 빅엔디안으로 변경

    //서버소켓 디스크립터에 서버의 정보(ip,포트번호)가 담긴 sockaddr구조체를 할당해줌
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
        printf("bind error\n");
    }

    //서버소켓이 클라이언트의 접속을 대기하게 해줌 ( 대기열 크기 = 5까지)
    if(listen(serv_sock,5) == -1){
        printf("listen error\n");
    }

    clnt_addr_size = sizeof(clnt_addr);

    //대기열이 5까지니까 클라이언트 5개까지는 허용
    for(i = 0;i < 5; i++){
        //서버소켓디스크립터 번호에 connect()로 접속을 요청하는 클라이언트소켓의 소켓정보를 받아서 clnt_sock에 저장
        clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
        //accept()로 서버로 연결을 시도하는 소켓을 수락하면 수락한 순간 clnt_addr변수에 클라이언트 소켓의 주소정보(ip,post번호)들이 들어간다.
        //그리고 이 순간부터는 write()와 read()함수를 통해 연결된 클라이언트와 상호데이터 교환이 가능하다!
        //read() => 보내온 데이터를 여기서 읽는것!
        //write() => 데이터를 상대편한테 보내는 것!
    
        if(clnt_sock == -1){
            printf("Accept error\n");
        }else{
            printf("Connected client %d\n",i+1);
        }
        //clnt_sock에서 보낸 데이터를 message에 buf_size만큼 저장 반환값이 0이면 더이상읽을게없다는뜻
        while((str_len = read(clnt_sock,message,BUF_SIZE)) != 0){
            write(clnt_sock, message, str_len); //clnt_sock 즉 클라이언트소켓에 저장한 message를 전송.
            printf("%d\n",str_len); // 데이터를 받을 때마다 읽은 데이터길이를 출력
        }
        printf("close client %d\n",i+1);//클라이언트단에서 q를 입력해서 종료했을 때 서버단에서 while문을 빠져나가는게아님
                                        //새로 다른 클라이언트의 접속이 들어올때 while문을 빠져나옴
        close(clnt_sock); // 전송 후 클라이언트 소켓 연결 끊기
    }

    close(serv_sock);
    return 0;










}