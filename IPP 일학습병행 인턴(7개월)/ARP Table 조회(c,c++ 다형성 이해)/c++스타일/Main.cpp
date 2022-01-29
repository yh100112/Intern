#include "WinArpTablePrint.h"
#include "LinuxArpTablePrint.h"

int main(){
  #ifdef _WIN32
    ArpTablePrint* test = new WinArpTablePrint;
  #else
    ArpTablePrint *test = new LinuxArpTablePrint; 
  #endif

  test->print();

  delete test;
  return 0;

}