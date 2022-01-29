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

struct MultipleArg{
    int clnt_sock;
    char msg[1024];
};

pthread_mutex_t mutx;

int main(int argc, char* argv[]){
    int serv_sock,client_sock;
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
        struct MultipleArg multi;
        client_addr_size = sizeof(client_addr_size);
        client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&client_addr_size);
        if(client_sock == -1){
            error_handling("accept error!!\n");
        }else{
            printf("Connected client src port : %d~\n",client_addr.sin_port);
        }
        multi.clnt_sock = client_sock; // 클라이언트 소켓 저장

        char message[1024] = {0, };
        read(client_sock,message,sizeof(message));
        printf("읽어온 파일 : %s\n",message);
        strcpy(multi.msg,message); // 파일 이름 읽어온 거 저장

        pthread_create(&t_id,NULL,handling,(void*)&multi);
        pthread_detach(t_id);
    }
    close(serv_sock);
    return 0;
}

void* handling(void* arg){
    struct MultipleArg *multi = (struct MultipleArg*)arg;
    int client_sock = multi->clnt_sock;
    char *message= multi->msg; // msg[1024]
    close(client_sock);    


    FILE* fp = fopen(message,"r");
    if(fp == NULL){
        error_handling("file open error!!\n");
    }

    char text[1024] = {0, };
    fread(text,sizeof(text),1,fp); // text에 파일 내용 읽어서 저장
    printf("%s\n",text);
    fclose(fp);

    time_t tm_time = time(NULL);
    struct tm* tm = localtime(&tm_time);
    char time[100]={0, }; //현재시간 저장
    strftime(time,100,"%Y_%m_%d_%H_%M_%S",tm);
    strcat(time,".txt");

    FILE* fp2 = fopen(time,"w");
    if(fp2 == NULL){
        error_handling("file open error!!\n");
    }

    fwrite(text,sizeof(char),strlen(text),fp2); //저장된 파일 내용을 새로운 파일에 쓰기
    fclose(fp2);
    return NULL;
}

void error_handling(const char* message){
    fputs(message,stderr);
    exit(1);
}