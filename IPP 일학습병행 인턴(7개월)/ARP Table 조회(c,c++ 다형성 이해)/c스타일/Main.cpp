#include "LinuxArpTablePrint.h"
#include "WinArpTablePrint.h"

int main(){
  #ifdef _WIN32
    ArpTablePrint *test = newWinArpTablePrint();
  #else
    ArpTablePrint *test = newLinuxArpTablePrint();
  #endif
  
  test->print(test);
  test->del(test);
  return 0;
}