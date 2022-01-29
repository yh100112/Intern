#include "LinuxArpTablePrint.h"

#define DELIM "            "



LinuxArpTablePrint::LinuxArpTablePrint(){}

LinuxArpTablePrint::~LinuxArpTablePrint(){}

void LinuxArpTablePrint::print(){
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

