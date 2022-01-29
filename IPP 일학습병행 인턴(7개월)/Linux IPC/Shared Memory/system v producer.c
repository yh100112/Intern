#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
typedef struct _shm_info
{
  char str_ip[40];
  unsigned int int_ip;
  unsigned int int_id;
} SHM_INFOS;

#define SHM_INFO_COUNT 5

int main()
{
  int shmid;
  int i;
  SHM_INFOS *shm_info = NULL;
  void *shared_memory = (void *)0;

  // 1.번 그림의 (1)에 해당, 공유메모리 공간을 가져온다. 없을시 생성
  // SHM_INFOS 5개 크기만큼 공유메모리 생성
  shmid = shmget((key_t)3836, sizeof(SHM_INFOS) * SHM_INFO_COUNT, 0666 | IPC_CREAT);
  if (shmid == -1){
    perror("shmget failed : ");
    exit(0);
  }

  // 1번 그림의 (2)에 해당, 공유 메모리를 process와 연결 시킨다.
  shared_memory = shmat(shmid, 0, 0); // 공유 메모리 시작주소값 반환
  if (shared_memory == (void *)-1){
    perror("shmat failed : ");
    exit(0);
  }

  // 공유메모리에 데이터 쓰기
  shm_info = (SHM_INFOS *)shared_memory;  // 공유메모리의 주소를 캐스팅해서 가져온 것
  printf("%d\n",sizeof(*shm_info));
  
  for (i = 0; i < SHM_INFO_COUNT; i++)
  {
    snprintf(shm_info[i].str_ip, sizeof(shm_info[i].str_ip), "1.1.1.%d", i);
    shm_info[i].int_ip = 12891010 + i;
    shm_info[i].int_id = 128 + i;
  }
}