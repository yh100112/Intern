#include<stdio.h>

int main(){
  unsigned char flag = 7; // 7 : 0000 0111
  flag &= ~2; // 1111 1101 마스크 값 ( 2의 비트를 뒤집은 것 ) 과 AND로 일곱번째 비트(0000 0010)를 끔

  printf("%u\n",flag); // 5 : 0000 0101 => 일곱번째 비트가 1에서 0으로 바뀜 ( 비트가 꺼짐 )
}