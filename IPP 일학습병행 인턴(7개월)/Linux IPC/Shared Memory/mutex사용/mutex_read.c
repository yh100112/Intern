#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> // sleep()
#include<sys/ipc.h>
#include<sys/shm.h>

#include<pthread.h>
#include<semaphore.h>

typedef struct shLock{
  pthread_mutex_t mutex;
  pthread_mutexattr_t mutexAttr;
  char buffer[100];
}shLock;

int main(){
  int shmid;
  key_t key = 5678; // write와 같은 키를 사용
  shLock *lockptr;

  shmid = shmget(key,sizeof(shLock),IPC_CREAT | 0666);
  if(shmid == -1){
    printf("[readop] error : cannot create shm\n");
  }

  lockptr = (shLock*)shmat(shmid, 0, 0); // 공유메모리를 맵핑해옴
  if(lockptr == -1){
    printf("[readop] errpr : failed attach memory\n");
  }
  printf("%p\n",lockptr);

  while(1){
    sleep(1);

    pthread_mutex_lock(&(lockptr->mutex));
      printf("[readop] buffer is [%s]\n", lockptr->buffer);
    pthread_mutex_unlock(&(lockptr->mutex));
  }
}