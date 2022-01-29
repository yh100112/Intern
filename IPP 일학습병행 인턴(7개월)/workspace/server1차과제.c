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
            printf("Connected client : %d\n",client_sock);
            clnt_socks[clnt_cnt] = client_sock;
            pthread_mutex_unlock(&mutx);        
            pthread_create(&t_id,NULL,handling,(void*)&clnt_socks[clnt_cnt]);

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
    close(client_sock);

    
    time_t tm_time = time(NULL);
    struct tm* tm = localtime(&tm_time);
    char time[100]={0, }; 
    strftime(time,100,"%Y_%m_%d_%H_%M_%S",tm);
    strcat(time,".txt");

    FILE* fp2 = fopen(time,"w");
    if(fp2 == NULL){ error_handling("file open error!!\n"); }

    fwrite(message,sizeof(char),strlen(message),fp2); 
    fclose(fp2);
    close(client_sock);
    printf("close client : %d\n",client_sock);
}

void error_handling(const char* errMsg){
    fputs(errMsg,stderr);
    exit(1);
}