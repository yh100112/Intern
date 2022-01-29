#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>

int main(){
  int fd[2], nbytes, rc = 0;
  pid_t childpid;
  char string[] = "Hello, world!";
  char readbuffer[80];

  if((rc = pipe(fd)) < 0){
    printf("Creating Pipe is Error [%d]\n", rc);
  }

  if((childpid = fork()) == -1){
    perror("fork");
    return 0;
  }

  if(childpid == 0){
    /*자식 프로세스는 Write할꺼기 때문에 Read fd는 닫아준다.*/
    close(fd[0]);

    /*  Pipe에 메시지 보내기  */
    write(fd[1],string,strlen(string));
    return 0;
  }else{
    /*부모 프로세스는 Read할꺼기 때문에 Write fd는 닫아준다.*/
    close(fd[1]);

    /* Pipe에서 메시지 읽기 */
    nbytes = read(fd[0],readbuffer,sizeof(readbuffer));
    printf("Received Parent string : %s [%d]\n", readbuffer, nbytes);
  }

    return 0;
}