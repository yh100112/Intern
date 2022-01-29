#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> // sleep
#include<sys/ipc.h>
#include<sys/shm.h>
#include<pthread.h>
#include<semaphore.h>

typedef struct shLock
{
  pthread_mutex_t mutex;
  pthread_mutexattr_t mutexAttr;
  char buffer[100];
}shLock;

int main(){
  int shmid;
  key_t key = 5678; //이 키로 공유메모리를 생성한다. read부분에서도 이 키를 사용해 공유메모리를 생성함
  shLock *lockptr; // 공유메모리를 맵핑할 구조체 ( 이안에 뮤텍스를 넣어서 사용 )
  int i = 1;

  shmid = shmget(key,sizeof(shLock),IPC_CREAT | 0666); // 공유메모리 생성
  if(shmid == -1){
    printf("error : cannot create shmrn\n");
  }

  lockptr = (shLock*)shmat(shmid, 0, 0); // 공유메모리에 맵핑 -> mutex가 공유메모리에 생성되는 것임!! 구조체에 공유메모리 주소를 맵핑했기 때문에!!
  if(lockptr == -1){
    printf("error : failed attach memoryrn\n");
  }

  printf("%p\n",lockptr);

  memset(lockptr->buffer,0,sizeof(lockptr->buffer));

  pthread_mutexattr_init(&(lockptr->mutexAttr)); // 뮤텍스 속성 객체 생성


  //생성된 뮤텍스 속성 객체(pthread_mutexattr_t)의 프로세스 공유 속성을 설정 (여기선 PTHREAD_PROCESS_SHARED로 설정함)
  pthread_mutexattr_setpshared(&(lockptr->mutexAttr), PTHREAD_PROCESS_SHARED);


  //설정한 뮤텍스 속성 객체 lockptr->mutexAttr을 이용해 뮤텍스 객체 pthread_mutex_t 생성
  pthread_mutex_init(&(lockptr->mutex), &(lockptr->mutexAttr)); 

  while(1){
    char buff[128] = {0,};
    int waitsec = 3;

    sleep(1);

    sprintf(buff,"write test %d ",i);
    pthread_mutex_lock(&(lockptr->mutex));
      memcpy(lockptr->buffer, buff, strlen(buff));
    pthread_mutex_unlock(&(lockptr->mutex));

    printf("[writeop] 작성 완료\n");
    i++;
  }
}