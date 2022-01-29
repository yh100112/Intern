#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pcap/pcap.h>

#define PROMISCUOUS 1
#define NONPROMISCUOUS 0

int main(int argc, char **argv){
  char *dev;
  char *net;
  char *mask;
  char ebuf[PCAP_ERRBUF_SIZE];
  int res = 0;
  int ret;

  bpf_u_int32 netp;
  bpf_u_int32 maskp;
  struct in_addr net_addr;
  struct in_addr mask_addr;

  dev = pcap_lookupdev(ebuf);
  printf("device : %s\n",dev);

  struct pcap_pkthdr *header;
  const unsigned char *Data;
  int count = 0;

  pcap_t *pcd = pcap_open_live(dev,65535,NONPROMISCUOUS,0,ebuf); // to_ms 에 -1이 아니라 0임. 에러를 저장할 문자열 ebuf는 *ebuf로 하면 segfault남

  if(pcd == NULL){
    printf("pcd %s\n",ebuf);
    exit(1);
  }

  while((res = pcap_next_ex(pcd, &header, &Data)) >= 0){
    int length = header->len;
    time_t tv_sec = header->ts.tv_sec;
    struct tm* tm = localtime(&tv_sec);
    char buffer[100]= {0,};
    strftime(buffer,100,"%Y_%m_%d_%H_%M_%S", tm);
    printf("길이 : %d / 초 : %s\n", length,buffer);
    count = 0; // 16바이트씩 잘라서 출력하기위해 16을 카운트함

    while(length--){
      printf("%02x ", *Data++);
      if(++count % 16 == 0){
        printf("\n");
      }
    }

    printf("\n");
  }

}
/*
pcap_next_ex()는 패킷이 들어올 때마다, 그때에 해당하는 패킷의 헤더의 길이, timestamp, 데이터를 알 수 있다.
while루프를 한번 돌 때 while 내부에서는 패킷 하나를 볼 수 있는 상태이다.
*/