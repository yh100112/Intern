#include "ArpTablePrint.h"
#define DELIM "            "


#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include <iphlpapi.h>
  #include <Ws2tcpip.h>
  void print(ArpTablePrint *a){
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
#else
void print(ArpTablePrint *a){
  FILE *fp = fopen("/proc/net/arp","r"); // 리눅스에서는 arp테이블이 여기에 들어있음


  char buffer[5][100] = {0,}; // 5줄까지만 읽어옴 사이즈를 500에 맞추기 위해
  char ip[10][50]={0,};
  char mac[10][50]={0,};
  int cnt = 0;
  while(fgets(buffer[cnt],sizeof(buffer[cnt]),fp) != NULL){
    buffer[cnt][strlen(buffer[cnt])] = '\0';
    cnt++;
  }

  char *ret_ptr;
  char *next_ptr;
  int cnt2 = 1;
  int idx = 0;
  for(int i = 1;i < cnt;i++){
    ret_ptr = strtok_r(buffer[i],DELIM,&next_ptr);
    strcpy(ip[idx],ret_ptr);
    while(ret_ptr != NULL){
      cnt2++;
      ret_ptr = strtok_r(NULL,DELIM,&next_ptr);
      if(cnt2 == 4){
        strcpy(mac[idx],ret_ptr);
      }
    }
    printf("ip : %s | mac : %s\n",ip[idx],mac[idx]);
    cnt2 = 1;
    idx++;
  }
  fclose(fp);  
  return;
}
#endif

void del(ArpTablePrint *a){
  free(a);
  return;
}