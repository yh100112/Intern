#include <stdio.h>                                                              
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

typedef struct _shm_info{ 
  char         str_ip[40];
  unsigned int int_ip;    
  unsigned int int_id;     
}SHM_INFOS;   

#define SHM_INFO_COUNT 5

int main(){
  int shmid;
  SHM_INFOS *shm;

  // 1번 그림의 (1)에 해당, 공유메모리 공간을 가져온다.
  shmid = shmget((key_t)3836, sizeof(SHM_INFOS)*SHM_INFO_COUNT, 0666);

  if (shmid == -1){
    perror("shmget failed : ");
    exit(0);
  }
  shm = (SHM_INFOS*)shmat(shmid,NULL,0); // 연결된 공유 메모리의 시작주소를 반환하므로 그 값을 구조체 _shm_info 형태로 캐스팅해서 받아옴
                                         // => 접근하고자 하는 프로세스의 주소 공간에 공유 메모리 주소를 매핑한 것!!
  for(int i = 0; i < SHM_INFO_COUNT; i++){
    printf("%s\n",shm[i].str_ip);
  }
  
  // 가져온 공유 메모리를 제거한다.
  if ( -1 == shmctl( shmid, IPC_RMID, 0)){
    printf( "공유 메모리 제거 실패\n");
    return -1;
  }
  else
  {
    printf( "공유 메모리 제거 성공\n");
  }
  return 0;

}