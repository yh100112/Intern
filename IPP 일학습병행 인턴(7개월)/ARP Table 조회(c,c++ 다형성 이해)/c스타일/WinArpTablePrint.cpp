#include "WinArpTablePrint.h"
void print(ArpTablePrint *a);
void del(ArpTablePrint *a);

ArpTablePrint *newWinArpTablePrint(){
  ArpTablePrint *arpTablePrint = (ArpTablePrint*)malloc(sizeof(ArpTablePrint));

  if(arpTablePrint){
    arpTablePrint->print = print;
    arpTablePrint->del = del;
  }

  return arpTablePrint;
}
