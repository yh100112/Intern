#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pcap/pcap.h>
#include<signal.h>
#include <fcntl.h>
#include<pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <list>
#include<string>
#include<iostream>
using namespace std;

#define PROMISCUOUS 0
#define NONPROMISCUOUS 1
#define MAXLEN 4096

//pcap파일 헤더 구조체
typedef struct _Pcap_file_header{
  bpf_u_int32 magic;
  unsigned short version_major;
  unsigned short version_minor;
  bpf_int32 thiszone;
  bpf_u_int32 sigfigs;
  bpf_u_int32 snaplen;
  bpf_u_int32 linktype;
}Pcap_file_header;

//패킷헤더 구조체
typedef struct _pcap_data_header{
  int sec;
  int usec;
  unsigned int caplen;
  unsigned int len;
}Pcap_data_header;

//공유메모리
typedef struct SharedMemory_S{
  pthread_mutex_t sharedMutex;
  pthread_mutexattr_t sharedMutexAttr;
  pthread_cond_t sharedCond;
  pthread_condattr_t sharedCondAttr;
  Pcap_data_header pcapHdr;
  unsigned char buf[MAXLEN];
}SharedMemory;

static volatile __sig_atomic_t is_terminated = 0;
const char* PCAP_WRITE_PATH = "/home/kdy/project/패킷캡처과제/pcap_file_namespace/";
pthread_mutex_t file_management_mutex;
pthread_cond_t file_management_cond;

list<string> real_file_list;
list<string> shared_file_list;

void signal_handler(int unused);
void initialize(Pcap_file_header *pcapFileHeader,pcap_t *pcd);
void* thread_file_management(void* arg);

int main(){
  int fd;
  Pcap_file_header pcapFileHeader;
  Pcap_data_header pcapDataHeader;
  pthread_t file_management_thr;
  pcap_t *pcd;
  const char *dev;
  char errbuf[PCAP_ERRBUF_SIZE];
  int count = 0;
  int res;
  
  int shmid;
  SharedMemory *shm = NULL;
  void *shared_memory = 0;

  shmid = shmget((key_t)4000, sizeof(SharedMemory), 0666 | IPC_CREAT);
  if(shmid == -1){
    printf("shmid error\n");
    exit(1);
  }

  shared_memory = shmat(shmid, 0, 0); // 맵핑된 공유메모리 주소
  if(shared_memory == (void*)-1){
    printf("shmat error\n");
    exit(1);
  }
  shm = (SharedMemory*)shared_memory; //공유메모리 주소를 프로세스에 매핑함

  //뮤텍스 설정
  pthread_mutexattr_init(&(shm->sharedMutexAttr));
  pthread_mutexattr_setpshared(&(shm->sharedMutexAttr), PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(shm->sharedMutex), &(shm->sharedMutexAttr));

  pthread_condattr_init(&(shm->sharedCondAttr));
  pthread_condattr_setpshared(&(shm->sharedCondAttr),PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&(shm->sharedCond),&(shm->sharedCondAttr));//뮤텍스 조건변수 설정

  //메인쓰레드와 파일관리 쓰레드 뮤텍스 설정
  pthread_mutex_init(&file_management_mutex,NULL);
  pthread_cond_init(&file_management_cond,NULL);


  //시그널 핸들러 설정
  struct sigaction new_act;
  new_act.sa_handler = signal_handler;
  sigemptyset(&new_act.sa_mask);
  sigaction(SIGINT, &new_act,NULL);

  // dev = pcap_lookupdev(errbuf);
  dev = "enp0s3";
  pcd = pcap_open_live(dev,MAXLEN, NONPROMISCUOUS, 10, errbuf); // 메모리 누수 발생
  pcap_set_immediate_mode(pcd,1);
  // pcap_setnonblock(pcd,1,errbuf);

  initialize(&pcapFileHeader, pcd); // Pcap_file_header 초기세팅

  while(!is_terminated){
    //첫번째 파일 생성
    struct timeval tv;
    struct tm tm;
    gettimeofday(&tv,NULL);
    time_t secCurr = tv.tv_sec;
    time_t secLast = 0;
    char timestr[32];
    char filename[256];
    char list_filename[256];
    int cnt = 0;
    int file_cnt = 0;
    localtime_r(&secCurr, &tm);
    strftime(timestr,sizeof(timestr), "%Y%m%d_%H%M%S",&tm);
    snprintf(filename,sizeof(filename),"%s%s_%06d_%02d.pcap",PCAP_WRITE_PATH,timestr,tv.tv_usec,file_cnt+1);
    fd = open(filename,O_WRONLY | O_CREAT, 0644);
    write(fd, &pcapFileHeader, sizeof(pcapFileHeader));

    pthread_create(&file_management_thr, NULL, thread_file_management, NULL); // 파일관리 쓰레드 시작

    //처음 파일 만든 후 파일 관리 쓰레드에 시그널 전달
    snprintf(list_filename,sizeof(list_filename),"%s_%06d_%02d.pcap",timestr,tv.tv_usec,file_cnt+1);
    real_file_list.push_back(list_filename);


    const unsigned char *data;
    struct pcap_pkthdr *header;

    while((res = pcap_next_ex(pcd, &header, &data)) >= 0 && (!is_terminated)){
      usleep(1000000); // 패킷캡쳐가 일어나서 공유메모리에 저장하는 속도를 pcapanalyzer가 공유메모리의 내용을 화면에 출력하는 속도가 따라가지 못해 sleep으로 속도를 늦춤
      
      if(real_file_list.size() > 5){
        const char* f = real_file_list.front().c_str();
        char del_file_name[256];
        strcpy(del_file_name,PCAP_WRITE_PATH);
        strcat(del_file_name,f);
        printf("del : %s\n",del_file_name);
        unlink(del_file_name);
        real_file_list.pop_front(); // 5개가 넘어갈 경우 맨 앞에 저장되어 있는 파일 list에서 삭제
      }
      
      memset(&tv,0,sizeof(tv));
      gettimeofday(&tv,NULL);
      secLast = tv.tv_sec;

      if((secLast - secCurr) % 120 == 0 && (secLast - secCurr) > 120){ // 첫 파일 생성된 후 최소 120초 이상 경과한 후만 들어옴
        pthread_mutex_lock(&file_management_mutex);
          file_cnt++;
          close(fd);
          memset(&timestr, 0, sizeof(timestr));
          memset(&filename,0,sizeof(filename));
          memset(&tm,0,sizeof(tm));
          
          localtime_r(&secLast, &tm);
          strftime(timestr,sizeof(timestr), "%Y%m%d_%H%M%S",&tm);
          snprintf(filename,sizeof(filename),"%s/%s_%06d_%02d.pcap",PCAP_WRITE_PATH,timestr,tv.tv_usec,file_cnt+1);
          fd = open(filename,O_WRONLY | O_CREAT | O_EXCL, 0644);

          if(fd == -1){ // 파일명이 겹칠 때 순번 증가시킴
            file_cnt++;
            snprintf(filename,sizeof(filename),"%s/%s_%06d_%02d.pcap",PCAP_WRITE_PATH,timestr,tv.tv_usec,file_cnt+1);
            fd = open(filename,O_WRONLY | O_CREAT | O_EXCL, 0644);
          }

          write(fd, &pcapFileHeader, sizeof(pcapFileHeader));

          memset(list_filename,0,sizeof(list_filename));
          snprintf(list_filename,sizeof(list_filename),"%s_%06d_%02d.pcap",timestr,tv.tv_usec,file_cnt+1);
          shared_file_list.push_back(list_filename);
          pthread_cond_signal(&file_management_cond);
        pthread_mutex_unlock(&file_management_mutex);

        // 리스트 목록 출력
        // list<string>::iterator itor;
        // itor = real_file_list.begin();
        // while(itor != real_file_list.end()){
        //   cout << *itor++ << endl;
        // }            

      }

      pthread_mutex_lock(&(shm->sharedMutex));
        printf("캡처한 패킷 갯수 : %d\n", cnt+1);
        if(res > 0){
          /* pcap파일의 타임스탬프는 하위 호환성 유지를 위해 무조건 32비트임
            (long으로 되어있는걸 64bit에서는 int로 직접 지정해줘야함) 
            -> 아래와 같이 pacp_pkthdr의 내용을 사용자가 만든 구조체에 직접 대입
          */

          pcapDataHeader.sec = (int)header->ts.tv_sec;
          pcapDataHeader.usec = (int)header->ts.tv_usec;
          pcapDataHeader.caplen = header->caplen;
          pcapDataHeader.len = header->len;

          write(fd, &pcapDataHeader, sizeof(pcapDataHeader));
          shm->pcapHdr = pcapDataHeader; // packet header 정보 공유메모리에 저장

          write(fd,data,header->caplen);
          memset(shm->buf,0,sizeof(shm->buf));

          int length = header->caplen;
          int count = 0;

          while(length--){
            shm->buf[count++] = *data++; 
            // 공유메모리 buf에 캡쳐한 데이터를 집어넣는 부분 (strcpy()로 아무리 해도 안되서 하나하나 직접 넣도록 구현 ) 
          }
        }
        printf("\n");
        if(is_terminated){
          break;
        }
        pthread_cond_signal(&(shm->sharedCond));
      pthread_mutex_unlock(&(shm->sharedMutex));
      cnt++;
    }

  } // end of while

  pthread_mutex_unlock(&(shm->sharedMutex)); // unlock을 해주지않고 while문을 빠져나올 경우를 대비해 while문을 빠져나와 lock을 해제해줌

  printf("while문 탈출!\n");
  pthread_mutex_lock(&(shm->sharedMutex));
    pthread_cond_signal(&(shm->sharedCond));
  pthread_mutex_unlock(&(shm->sharedMutex));


  pthread_mutex_lock(&file_management_mutex);
    pthread_cond_signal(&file_management_cond);
  pthread_mutex_unlock(&file_management_mutex);

  pthread_join(file_management_thr, NULL);
  shmdt(shared_memory);
  if(shmctl(shmid, IPC_RMID, 0) != -1){
    printf("공유 메모리 제거 성공\n");
  }

  close(fd);
  pcap_close(pcd);
  return 0;
}

void* thread_file_management(void* arg){
  while(!is_terminated){
    pthread_mutex_lock(&file_management_mutex);
    if(is_terminated){
      break;
    }
      pthread_cond_wait(&file_management_cond, &file_management_mutex);
      if(is_terminated){
        break;
      }
      real_file_list.push_back(shared_file_list.back());
      real_file_list.sort();
      shared_file_list.pop_back(); // 공유리스트 clear
    pthread_mutex_unlock(&file_management_mutex);
  }

  printf("파일 관리 쓰레드 종료\n");
  return nullptr;
}

void initialize(Pcap_file_header *pcapFileHeader, pcap_t *pcd){
  pcapFileHeader->magic = 0xA1B2C3D4;
  pcapFileHeader->version_major = PCAP_VERSION_MAJOR;
  pcapFileHeader->version_minor = PCAP_VERSION_MINOR;
  pcapFileHeader->thiszone = 0;
  pcapFileHeader->sigfigs = 0;
  pcapFileHeader->snaplen = MAXLEN; // 캡처 최대 길이. pcap_open_live의 두번째 매개변수와 같은 값 사용할 것
  pcapFileHeader->linktype = pcap_datalink(pcd);
}

void signal_handler(int unused){
  printf("signal~\n");
  is_terminated = 1;
}

