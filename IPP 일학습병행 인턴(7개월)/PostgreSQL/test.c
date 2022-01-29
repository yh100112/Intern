#include<stdio.h>

int main(){
  unsigned char flag = 0;

  flag |= 1;
  flag |= 2;
  flag |= 4;

  printf("%u\n",flag);

}