#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#pragma once

typedef struct ArpTablePrint_S {
  void (*print) (ArpTablePrint_S*); // 함수포인터는 매개변수를 타입만 써도 됨
  void (*del) (ArpTablePrint_S*);
} ArpTablePrint;