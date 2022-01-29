#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<time.h>
#include<pthread.h>

#define _REENTRANT

void error_handling(const char* message);
void* handling(void* arg);

int clnt_cnt = 0; // 서버에 접속한 클라이언트수
int clnt_socks[256];

pthread_mutex_t mutx;

int main(int argc, char* argv[]){
    int serv_sock,client_sock;
    int thread_param;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_addr_size;
    pthread_t t_id;

    pthread_mutex_init(&mutx, NULL);
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

    while(1){
        client_addr_size = sizeof(client_addr_size);
        client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&client_addr_size);
        if(client_sock == -1){
            error_handling("accept error!!\n");
        }else{
            pthread_mutex_lock(&mutx);
            printf("Connected client : %d\n",clnt_cnt);
            clnt_socks[clnt_cnt] = client_sock;
            pthread_mutex_unlock(&mutx);        
            pthread_create(&t_id,NULL,handling,(void*)&clnt_socks[clnt_cnt]);
            //create로 쓰레드를 생성하는 순간
            //자식 쓰레드는 부모 쓰레드인 main과 별개로 동작한다. 이 순간부터
            //자식은 자식대로 돌고 부모는 부모대로 돌아서 else문 끝에  close(client_sock)을 하면 
            //자식쓰레드의 메인함수가 도는 것 보다 main의 close()함수가 더 빨리 동작해서 입력을 클라이언트에게 
            //받기도 전에 클라이언트 소켓이 닫히므로 파일명을 null로 읽어버려 fopen에서 에러나서 종료된다.
            //=> 이런 문제가 발생 안하려면 close()로 소켓을 닫아주는 것은 쓰레드의 main()함수 안에서 해주는게 좋다.  
            pthread_mutex_lock(&mutx);
            clnt_cnt++;
            pthread_mutex_unlock(&mutx);
            pthread_detach(t_id);
        }
    }

    close(serv_sock);
    return 0;
}

void* handling(void* arg){
    int client_sock = *(int*)arg;

    char message[1024] = {0, };
    read(client_sock,message,sizeof(message));
    close(client_sock);//read를하고 여기서 클라이언트 소켓을 닫아준다. 그러면 읽기 전까진 소켓이 종료되지 않도록 보장해준다.
    

    printf("받은 파일 : %s\n",message);

    FILE* fp = fopen(message,"r");
    if(fp == NULL){ error_handling("file open error!!\n"); }

    char text[1024] = {0, };
    fread(text,sizeof(text),1,fp); // text에 파일 내용 읽어서 저장
    fclose(fp);

    time_t tm_time = time(NULL);
    struct tm* tm = localtime(&tm_time);
    char time[100]={0, }; //현재시간 저장
    strftime(time,100,"%Y_%m_%d_%H_%M_%S",tm);
    strcat(time,".txt");

    FILE* fp2 = fopen(time,"w");
    if(fp2 == NULL){ error_handling("file open error!!\n"); }

    fwrite(text,sizeof(char),strlen(text),fp2); //저장된 파일 내용을 새로운 파일에 쓰기
    fclose(fp2);

}

void error_handling(const char* message){
    fputs(message,stderr);
    exit(1);
}