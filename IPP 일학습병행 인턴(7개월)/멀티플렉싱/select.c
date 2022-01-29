#include<stdio.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/select.h>
#define BUF_SIZE 30 

int main(int argc, char* argv[]){
    fd_set reads, temps;
    int result,str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;


    FD_ZERO(&reads); // fd_set변수 reads를 0으로 초기화
    FD_SET(0,&reads); // fd 0의 비트를 1로 설정한다. => 즉,표준입력에 변화가 있는지 관심을 두고 보겠다는 뜻이다.

/*
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    여기서 타임아웃을 설정하면 안됨. => select호출 후에는 이 값이 0으로 되어 계속 while문을 돌게 된다.
    그래서 while문안에 select가 호출되기 전에 매번 새롭게 초기화해줘야 함
*/

   while(1){
       temps = reads; // select함수호출이 끝나면 변화가 생긴 fd의 비트를 제외한 나머지 비트들은 0으로 초기화됨. 따라서 원본의 유지르 위해 이렇게 복사과정을 꼭 거쳐야함!!
       timeout.tv_sec = 5;
       timeout.tv_usec = 0;
       result = select(1,&temps,0,0,&timeout);
       if(result == -1){
           puts("select error!!");
           break;
       }else if(result == 0){
           puts("Timeout!!");
       }else{
           if(FD_ISSET(0,&temps) > 0){ // fd_set변수 temps에서 fd0을 찾는다.(해당 fd가 1인지 확인)
               str_len = read(0,buf,BUF_SIZE);
               buf[str_len] = 0;
               printf("message from console : %s",buf);
           }
       }
   }
   return 0;
}