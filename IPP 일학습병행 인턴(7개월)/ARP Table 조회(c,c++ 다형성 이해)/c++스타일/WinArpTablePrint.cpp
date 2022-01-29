#include "WinArpTablePrint.h"
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <Ws2tcpip.h>

WinArpTablePrint::~WinArpTablePrint(){
}

void WinArpTablePrint::print(){
  DWORD result;
  DWORD lenbuf = sizeof(MIB_IPNETTABLE);
  char *buf = (char*)malloc(lenbuf);
  if(!buf){
    printf("out of memory 1\n");
    return;
  } 
  

  while((result = GetIpNetTable((MIB_IPNETTABLE *)buf, &lenbuf, FALSE)) == ERROR_INSUFFICIENT_BUFFER){
    char *newbuf = (char*)realloc(buf, lenbuf);
    if(!newbuf){
      printf("Out of memory 2\n");
      free(buf);
      return;
    }
  }

  if(result == NO_ERROR){
    char ip[32];
    MIB_IPNETTABLE *table2 = (MIB_IPNETTABLE *)buf;
    for(int i = 0;i < table2->dwNumEntries; ++i){
      struct in_addr inAddr;
      inAddr.S_un.S_addr = table2->table[i].dwAddr;
      printf("Phys : %02X:%02X:%02X:%02X:%02X:%02X\tIp : %s\n",
            table2->table[i].bPhysAddr[0], table2->table[i].bPhysAddr[1],
            table2->table[i].bPhysAddr[2], table2->table[i].bPhysAddr[3],
            table2->table[i].bPhysAddr[4], table2->table[i].bPhysAddr[5], inet_ntoa(inAddr));
    }
  }else{
    printf("Arp Table gather error...\n");
  }

  free(buf);

  return;
}