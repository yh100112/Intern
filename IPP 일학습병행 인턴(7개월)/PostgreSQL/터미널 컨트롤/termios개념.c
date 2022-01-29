#include<stdio.h>	
#include<termios.h>
#include<unistd.h>

struct termios org_term;		//기존 터미널 속성을 저장해둘 구조체
struct termios new_term;		//새로 변경해서 적용할 구조체

void save_input_mode(void){
  tcgetattr(STDIN_FILENO,&org_term);	//	stdin으로부터 터미널 속성을 받아온다.
}

void set_input_mode(void){	
  tcgetattr(STDIN_FILENO, &new_term);			// STDIN으로부터 터미널 속성을 받아온다.
  new_term.c_lflag &= ~(ICANON | ECHO);		// ICANON,ECHO 속성을 off
  new_term.c_cc[VMIN] = 1;				// canonical mode처럼 개행을 만나거나 EOF를 만날때까지 처리하는게 아니라 1바이트씩 처리하도록 설정
  new_term.c_cc[VTIME] = 0;				// 시간은 설정 안함
  tcsetattr(STDIN_FILENO, TCSANOW, &new_term);	// 변경된 속성의 터미널을 STDIN에 바로 적용
}

void reset_input_mode(void){
  tcsetattr(STDIN_FILENO, TCSANOW, &org_term);	//	STDIN에 기존의 터미널 속성을 바로 적용
}
  
int main(){
  int ch = 0;
  
  save_input_mode();		//현재 터미널 세팅 저장
  set_input_mode();			//터미널 세팅 변경
  while(read(0,&ch,sizeof(int)) > 0){
    printf("ch = %d\n",ch);
      if(ch == 4)
        break;
      else
        write(0,&ch,sizeof(int));
    ch = 0;
  }

  reset_input_mode();		//터미널 세팅 원래 상태로 초기화
  return 0;
}