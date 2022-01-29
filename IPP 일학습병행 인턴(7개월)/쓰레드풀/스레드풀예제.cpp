#include <map>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREAD_POOL 256
using namespace std;

//전역 쓰레드 구조체
typedef struct _ph{
    int sockfd; // 현재 사용중인 소켓fd
    int index_num; // 인덱스 번호
}ph;

//전역쓰레드 구조체로써 현재 쓰레드 상황을 파악함
struct schedul_info{
    int client_num; // 현재 연결된 클라이언트 수
    int current_sockfd; // 가장 최곤에 만들어진 소켓지시자
    multimap<int, ph> phinfo; // 스레드 구조체 map
};

pthread_cond_t *mycond; // 각 쓰레드 별 조건변수
pthread_cond_t async_cond = PTHREAD_COND_INITIALIZER; //쓰레드 동기화를 위한 조건변수

pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER; //각 쓰레드별 조건변수의 크리티컬세션 지정을 위한 뮤텍스
pthread_mutex_t async_mutex = PTHREAD_MUTEX_INITIALIZER; // 쓰레드 동기화용 조건변수의 크리티컬세션 지정을 위한 뮤텍스

void* thread_func(void* data); // 클라이언트와의 통신용 쓰레드
void* mon_thread(void* data); // 현재 클라이언트 상태 모니터링용 쓰레드 => 디버깅용

struct schedul_info s_info;

int main(int argc, char* argv[]){
    int i;
    ph myph;
    int status;
    int pool_size = atoi(argv[2]);
    pthread_t p_thread;
    struct sockaddr_in clientaddr,serveraddr;
    int server_sockfd;
    int client_sockfd;
    int client_len;

    if((pool_size < 0) || (pool_size > MAX_THREAD_POOL)){
        printf("Pool size Error\n");
        exit(0);
    }

    server_sockfd = socket(AF_INET,SOCK_STREAM,0);
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));

    bind(server_sockfd,(struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(listen(server_sockfd,5) == -1){
        perror("listen error : ");
        exit(0);
    }

    mycond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * pool_size); // 쓰레드 갯수만큼 조건변수 생성
    
    //쓰레드 전역변수 초기화
    s_info.client_num = 0;
    s_info.current_sockfd = 0;

    //쓰레드 풀 생성
    for(i = 0; i < pool_size; i++){
        memset(&myph,0,sizeof(myph));
        myph.index_num = i;
        s_info.phinfo.insert(pair<int,ph>(0,myph)); // multimap에 키 0으로 100개를 넣어줌

        pthread_mutex_lock(&async_mutex);
        pthread_create(&p_thread,NULL,thread_func,(void*)&i);// 인자로 쓰레드 번호(index_num)인 i를 넘겨줌
        pthread_cond_wait(&async_cond,&async_mutex);
        pthread_mutex_unlock(&async_mutex);
    }

    //디버깅용 쓰레드 생성
    pthread_create(&p_thread,NULL,mon_thread,(void*)NULL);

    while(1){
        client_len = sizeof(clientaddr);
        multimap<int,ph>::iterator mi;
        client_sockfd = accept(server_sockfd,(struct sockaddr*)&clientaddr,(socklen_t *)&client_len);
        
        if(client_sockfd > 0){

            
            mi = s_info.phinfo.begin(); //multimap의 첫번째 원소를 가리키는 iterator
            //multimap은 자동정렬되므로 key가 0이었다가 역할이 부여되 쓰레드가 할당되면서 1로 바꿔 넣으면 알아서 1이 맨 뒤로 정렬된다(오름차순)
            //그렇기 때문에 시작 키값이 1이라는 뜻은 multimap이 꽉 차서 여기서 더 클라이언트 연결이 오면 연결을 종료시키도록 
            //mi->first (iterator로 키에 접근) == 1 이면 꽉찼다는 뜻으로 조건을 걸어서 연결을 종료시켜준다.
            if(mi->first == 1){ 
                printf("socket is full\n");
                close(client_sockfd);
            }else{
                //그렇지 않다면 연결을 받아 클라이언트를 세팅 후 해당 처리쓰레드에게 시그널을 보내 처리하게 한다.
                ph tmpph;
                int psockfd;
                int pindex_num;
                s_info.current_sockfd = client_sockfd;
                
                //multimap에 제일 첫번째 키가 0인 요소에 일을 할당시킴
                tmpph.sockfd = client_sockfd;
                tmpph.index_num = mi->second.index_num;// multimap의 제일 첫번째 키의 요소값인 ph구조체의 index_num멤버값을 넣어줌 ( 위에서 for문으로 i = 0부터 99까지 넣어줌)
                s_info.phinfo.erase(mi); // 첫번째를 할당해주고 난 후 그 첫번째를 맵에서 삭제! => 키가 0인 것을 삭제하고 1로 다시 넣어줘야함 (multimap은 키값 수정이 안되기 때문!!!)
                s_info.phinfo.insert(pair<int, ph>(1,tmpph));//키를 1로 다시 넣어주면 알아서 정렬되서 맨 뒤로 감
                s_info.client_num ++; //연결된 클라이언트 수 1 증가
                printf("send signal %d\n",mi->second.index_num);
                pthread_cond_signal(&mycond[mi->second.index_num]);
            }

        }else{
            cout << "accept error" << endl;
        }
    }
    pthread_join(p_thread, (void**)status);

}

void* thread_func(void* data){
    char buf[255];
    int mysocket;
    int mynum = *((int*)data);
    multimap<int,ph>::iterator mi;

    //쓰레드 동기화용 조건변수
    pthread_mutex_lock(&async_mutex);
    pthread_cond_signal(&async_cond);
    pthread_mutex_unlock(&async_mutex);

    cout << "Thread create " << mynum << endl;
    while(1){
        //main쓰레드로부터 신호를 기다림. 신호가 도착하면 쓰레드 전역변수로부터 현재 처리해야 할 소켓지정값을 가져옴
        pthread_mutex_lock(&async_mutex);
        pthread_cond_wait(&mycond[mynum],&async_mutex);
        mysocket = s_info.current_sockfd;
        pthread_mutex_unlock(&async_mutex);
        memset(buf,0x00,sizeof(buf));

        //데이터를 처리함. 만약 quit문자열을 만나면 쓰레드 전역변수를 세팅한 다음 연결종료 한다.
        while(1){
            read(mysocket,buf,255);
            if(strstr(buf,"quit") == NULL){
                write(mysocket,buf,255);
            }else{
                mi = s_info.phinfo.begin();
                while(mi != s_info.phinfo.end()){
                    cout << "search " << mi->second.index_num << endl;
                    if(mi->second.index_num == mynum){
                        ph tmpph;
                        tmpph.index_num = mynum;
                        tmpph.sockfd = 0;
                        s_info.phinfo.erase(mi);
                        s_info.phinfo.insert(pair<int, ph>(0, tmpph));
                        s_info.client_num --;
                        close(mysocket);
                        break;   
                    }
                    mi ++;
                }
                break;
            }
            memset(buf,0x00,255);
        }
    }
}

void* mon_thread(void* data){
    cout << "moniter thread" << endl;
    while(1){
        sleep(10);
        multimap<int,ph>::iterator mi;
        mi = s_info.phinfo.begin();
        cout << "size " << s_info.phinfo.size() << endl;
        while(mi != s_info.phinfo.end()){
            cout << mi->first << " : "  << mi->second.index_num << " : "<<mi->second.sockfd << endl;
            mi++;
        }
    }
}
