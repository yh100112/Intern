#include "LinuxArpTablePrint.h"

void print(ArpTablePrint* a);
void del(ArpTablePrint* a);

ArpTablePrint* newLinuxArpTablePrint(){
  ArpTablePrint *arpTablePrint = (ArpTablePrint*)malloc(sizeof(ArpTablePrint));

  if(arpTablePrint){
    arpTablePrint->print = print;
    arpTablePrint->del = del;
  }

  return arpTablePrint;
}

