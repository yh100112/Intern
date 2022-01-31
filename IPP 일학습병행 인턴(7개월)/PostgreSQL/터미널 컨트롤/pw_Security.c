#include<stdio.h>
#include<unistd.h>
#include<termios.h>

int main(int argc, char* argv[]){
  printf("enter password : ");

  struct termios term; //터미널을 제어하기 위해 termios 구조체 선언
  tcgetattr(fileno(stdin), &term); // 현재 터미널 상태 term에 저장

  term.c_lflag &= ~ECHO; // ECHO 속성 off로 세팅 : 입력값 반향 X 
  tcsetattr(fileno(stdin),TCSANOW,&term);

  char passwd[32];
  fgets(passwd,sizeof(passwd),stdin); // 입력할 때 입력하는 값이 콘솔에 나타나지 않는다. 

  term.c_lflag |= ECHO; // ECHO 속성 다시 on으로 세팅 : 입력값 반향됨 
  tcsetattr(fileno(stdin),TCSANOW,&term); //다시 변경해서 터미널 설정 원상태로 돌려놓지 않으면 터미널에 입력값 계속 반향 안되는 상태로 남음

  printf("\nYour password is :%s\n",passwd);

}