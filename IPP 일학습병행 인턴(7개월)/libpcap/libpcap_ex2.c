#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>     //libpcap 헤더 포험
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
  char *dev;    // network device
  char *net;    // network address 
  char *mask;   // network mask address 
  int ret;       
  char errbuf[PCAP_ERRBUF_SIZE]; // error를 담을 문자열
  bpf_u_int32 netp;  // ip 
  bpf_u_int32 maskp; // submet mask
  struct in_addr addr;

  // 네트웍 디바이스 이름을 얻어온다. 
  dev = pcap_lookupdev(errbuf);

  // 에러가 발생했을경우 
  if(dev == NULL)
  {
    printf("%s\n",errbuf);
    exit(1);
  }

  printf("DEV: %s\n",dev); // 네트웍 디바이스 이름 출력

  ret = pcap_lookupnet(dev,&netp,&maskp,errbuf); // 네트웍 디바이스 이름 dev 에 대한 mask,ip정보 얻어오기 

  if(ret == -1){
    printf("%s\n",errbuf);
    exit(1);
  }

  // 네트웍 어드레스를 점박이 3형제 스타일로
  addr.s_addr = netp;
  net = inet_ntoa(addr);

  if(net == NULL){
    perror("inet_ntoa");
    exit(1);
  }

  printf("NET: %s\n",net);

  // 마찬가지로 mask 어드레스를 점박이 3형제 스타일로
  addr.s_addr = maskp;
  mask = inet_ntoa(addr);

  if(mask == NULL){
    perror("inet_ntoa");
    exit(1);
  }

  printf("MASK: %s\n",mask);
  return 0;
}