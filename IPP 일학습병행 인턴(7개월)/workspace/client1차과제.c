#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>

void error_handling(const char* message);
void *send_file(void* arg);

#define BUF_SIZE 1024

int main(int argc,char* argv[]){
    int serv_sock, fd;
    struct sockaddr_in serv_addr;
    pthread_t t_id;

    serv_sock = socket(AF_INET,SOCK_STREAM,0);
    if(serv_sock == -1){
        error_handling("socket error!!\n");
    }

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if(connect(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1){
        error_handling("connect error!!\n");
    }else{
        fputs("server connect success!!\n",stdout);
    }

    int thread_param;
    pthread_create(&t_id,NULL, send_file,(void*)&serv_sock);
    pthread_join(t_id,(void*)&thread_param);
    close(serv_sock);
    return 0;
}


void* send_file(void* arg){
    int serv_sock = *((int*)arg);
    char file_name[BUF_SIZE] = {0, };
    char message[BUF_SIZE] = {0, };
    printf("보낼 파일명 : ");
    scanf("%s",file_name);    
    
    FILE* fp = fopen(file_name,"rb");
    fread(message,sizeof(char),BUF_SIZE,fp);
    write(serv_sock,message,strlen(message));
}

void error_handling(const char* errMsg){
    fputs(errMsg,stderr);
    exit(1);
}