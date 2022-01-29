#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include<iostream>
#include<list>
// #include<signal.h>
using namespace std;

#define BUF_SIZE 4096
#define SMALL_BUF 100
#define DELIM " "

#define MAX_THREAD_NUM 3

void request_handler(int *arg);
void send_data(FILE* fp,char* file_name,char* req_line,char* version);
void send_error(FILE* fp);
void error_handling(const char* message);
void* thread_func(void* arg);
void* mon_thread(void* data);

pthread_cond_t mycond[MAX_THREAD_NUM] = {PTHREAD_COND_INITIALIZER,};
pthread_mutex_t mutex[MAX_THREAD_NUM] = {PTHREAD_MUTEX_INITIALIZER, };
pthread_cond_t cond_main = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_main = PTHREAD_MUTEX_INITIALIZER;

struct schedule{
    list<int> thread_list; // 쓰레드 풀
    list<int> clnt_list[MAX_THREAD_NUM]; // 접속한 클라이언트 소켓번호
}schedule;

int thread_idx = 0;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_size;
	pthread_t t_id;	

    
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); 
	serv_adr.sin_port = htons(atoi(argv[1]));
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1) {
        error_handling("bind error");
    }
	if(listen(serv_sock, 20)==-1) {
        error_handling("listen error");
    }

    pthread_create(&t_id,NULL,mon_thread,(void*)NULL); // 모니터링


    for(int i=0;i < MAX_THREAD_NUM;i++){
        pthread_create(&t_id,NULL,thread_func,(void*)&i); // (void*)&i or &i 로 정석대로 변수의 주소값을 보내면 쓰레드가 생성하기 전에 for문이 더 빨리 돌아 i값이 이상한 값이 이상한 값이 들어가 예측불가능
        schedule.thread_list.push_back(i); // thread_list에 만들어진 쓰레드 번호를 넣는다.
        pthread_mutex_lock(&mutex_main);
        pthread_cond_wait(&cond_main,&mutex_main);
        pthread_mutex_unlock(&mutex_main);
    }


	while(1)
	{
		clnt_adr_size=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
        printf("clnt_sock : %d  /  Connection Request : %s:%d  /  thread_idx % MAX_THREADS :%d\n",clnt_sock,inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port),thread_idx % MAX_THREAD_NUM);
        if(clnt_sock == -1){error_handling("accept error");}
		
        //접속하는 클라이언트마다 mutex를 다르게 준다.
        //접속한 순서대로 넣으려고 뮤텍스로 락 걸어줌
        pthread_mutex_lock(&mutex[thread_idx % MAX_THREAD_NUM]);
        schedule.clnt_list[thread_idx % MAX_THREAD_NUM].push_back(clnt_sock);
        pthread_mutex_unlock(&mutex[thread_idx % MAX_THREAD_NUM]);

        //조건변수도 다르게 줌
        pthread_cond_signal(&mycond[thread_idx % MAX_THREAD_NUM]); 
        pthread_mutex_lock(&mutex_main);
        thread_idx++;
        pthread_mutex_unlock(&mutex_main);
	}
	close(serv_sock);
	return 0;
}

void* thread_func(void* arg){
    int thread_num  = *(int*)arg; 
    /* -fpermissive 쳐줘야 함. 예전 컴파일러는 안잡는데 요즘 컴파일러는 포인터에 int값 넣으면 에러잡음.
        포인터도 주소이지만 정수값이므로 int를 넣어줘도 됨 대신 야매임이건
    */
    int clnt_sock;

    printf("create thread_num : %d\n",thread_num);
    pthread_cond_signal(&cond_main);

    while(1){
        pthread_mutex_lock(&mutex[thread_num]);

        while(schedule.clnt_list[thread_num].begin() == schedule.clnt_list[thread_num].end()){ // 접속한 클라이언트가 없으면(일이 없으면) while문을 게속 돌면서 쓰레드는 대기상태
            pthread_cond_wait(&mycond[thread_num],&mutex[thread_num]);
        }
        
        printf("깨운 thread num : %d\n",thread_num);
        if(schedule.clnt_list[thread_num].begin() != schedule.clnt_list[thread_num].end()){
            while(schedule.clnt_list[thread_num].begin() != schedule.clnt_list[thread_num].end()){
                clnt_sock = schedule.clnt_list[thread_num].front();
                schedule.clnt_list[thread_num].pop_front();
            }
        }

        pthread_mutex_unlock(&mutex[thread_num]);  
        request_handler(&clnt_sock); // 실제 업무는 락을 빠져나와 여기서 하느게 좋다. 락 걸어둔 안에서 모든 일을 처리하고 처리한 후의 클라이언트 소켓 번호만 여기로 넘겨줌. 깔끔!!


    }
}

void request_handler(int *arg)
{
	int clnt_sock=*arg;
	char req_line[SMALL_BUF];
    char rem_line[BUF_SIZE];

    char method[10];
	char ct[15];
	char file_name[30];
    char version[30];

    char* next_ptr;


	
	FILE* clnt_read = fdopen(clnt_sock, "r"); 
	FILE* clnt_write = fdopen(dup(clnt_sock), "w"); 
	

    fgets(req_line, SMALL_BUF, clnt_read); // 첫줄 읽음
    while(strstr(req_line,"\r\n") == NULL){ // 첫줄에 \r\n이 없으면 계속 반복문 돌게 함
        fgets(req_line,SMALL_BUF,clnt_read); // 다음 줄 읽음 ( 첫줄에서 누락된다면 그 다음 첫줄에 남아있던 것을 읽겠지?)
    }
	strcpy(method, strtok_r(req_line, " ",&next_ptr)); //GET
	strcpy(file_name, strtok_r(NULL, " ",&next_ptr)); // index.html
    strcpy(version,strtok_r(NULL," ",&next_ptr));// HTTP/1.1\r\n

    // printf("Method : %s\n",method);
    // printf("URL : %s\n",file_name);
    // printf("Version : %s\n",version);

    //위치 바뀌었을때 에러나는 이유 찾기 => 아래 while문과 위치를 바꾸면 화면이 로딩이 안되고 보낸 패킷이 게속 pending상태로 됨
	send_data(clnt_write,file_name,req_line,version); 

    // while(fgets(rem_line,BUF_SIZE,clnt_read) != NULL){
    //     printf("%s",rem_line);
    //     memset(rem_line,0,sizeof(BUF_SIZE));
    // }

	fclose(clnt_read);
    
}

void send_data(FILE* fp, char* file_name,char* req_line,char* version)
{
    const char *const MESSAGE_200 =
        "HTTP/1.1 200 OK\r\n"
        "Cache-Control: no-cache, no-store, max-age=0, must-revalidate\r\n"
        "Expires: 0\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 112\r\n"
        "\r\n"
        "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>SYSONE</title></head><body><h1>SYSONE</h1></body></html>";

    char buf[BUF_SIZE];
    strcat(req_line," "); //GET 
    strcat(req_line,file_name); // GET /index.html
    strcat(req_line," ");//GET /index.html 
    strcat(req_line,version); 


    printf("클라이언트의 request헤더에서 읽어온 첫문장 : %s",req_line);
    if(strcmp(req_line,"GET / HTTP/1.1\r\n") == 0){
        fprintf(fp,"%s\0",MESSAGE_200);
        fflush(fp);
    }else if(strcmp(req_line,"GET /favicon.ico HTTP/1.1\r\n") == 0){
        fprintf(fp,"%s\0",MESSAGE_200);
        fflush(fp);
    }else{
        send_error(fp);
        fclose(fp);
        return;
    }

	fclose(fp);
    return;
}

void send_error(FILE* fp)
{	
    const char * const MESSAGE_404 = 
    "HTTP/1.1 404 Not Found\r\n"
    "Cache-Control: no-cache, no-store, max-age=0, must-revalidate\r\n"
    "Expires: 0\r\n"
    "Connection: close\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 110\r\n\r\n"
    "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>ERROR</title></head><body><h1>ERROR</h1></body></html>";

    fprintf(fp,"%s",MESSAGE_404);
	fflush(fp);
}

void error_handling(const char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void* mon_thread(void* data){
    cout << "moniter thread" << endl;
    list<int>::iterator mi;
    list<int>::iterator mi2;
    list<int>::iterator mi3;

    while(1){
        sleep(5);
        mi = schedule.clnt_list[0].begin();
        mi2 = schedule.clnt_list[1].begin();
        mi3 = schedule.clnt_list[2].begin();

        printf("-------------------------------\n");
        cout  << "clnt_list[0] size :  " << schedule.clnt_list[0].size() << endl;
        cout  << "clnt_list[1] size :  " << schedule.clnt_list[1].size() << endl;
        cout  << "clnt_list[2] size :  " << schedule.clnt_list[2].size() << endl;

        printf("clnt_list[0]에 접속한 소켓 디스크립터 : ");
        while(mi != schedule.clnt_list[0].end()){
            printf("%d ",*mi);
            mi++;
        }
        printf("\n");

        printf("clnt_list[1]에 접속한 소켓 디스크립터 : ");
        while(mi2 != schedule.clnt_list[1].end()){
            printf("%d ",*mi2);
            mi2++;
        }
        printf("\n");   

        printf("clnt_list[2]에 접속한 소켓 디스크립터 : ");
        while(mi3 != schedule.clnt_list[2].end()){
            printf("%d ",*mi3);
            mi3++;
        }
        printf("\n");
        printf("-------------------------------\n");
    }
}