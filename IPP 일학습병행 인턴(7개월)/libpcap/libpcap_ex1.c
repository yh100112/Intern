#include<sys/time.h>
#include<net/ethernet.h>
#include<pcap/pcap.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<error.h>
#include<unistd.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>

#define PROMISCUOUS 1
#define NONPROMISCUOUS 0

struct ip *iph;
struct tcphdr *tcph;

void callback(unsigned char *useless, const struct pcap_pkthdr *pkthdr, const unsigned char *packet);

int main(int argc, char **argv){
  char *dev;
  char *net;
  char *mask;
  int ret;

  bpf_u_int32 netp;
  bpf_u_int32 maskp;
  char errbuf[PCAP_ERRBUF_SIZE];
  struct pcap_pkthdr hdr;
  struct in_addr net_addr;
  struct in_addr mask_addr;
  struct ether_header *eptr;
  const unsigned char *packet;

  struct bpf_program fp;
  pcap_t *pcd;

  dev = pcap_lookupdev(errbuf);
  if(dev==NULL){ printf("%s\n", errbuf); exit(1);}
  printf("device : %s\n",dev);

  ret = pcap_lookupnet(dev, &netp, &maskp, errbuf);
  if( ret == -1){ printf("%s\n",errbuf); exit(1);}
  net_addr.s_addr = netp;
  net = inet_ntoa(net_addr);
  printf("net :%s\n",net);

  mask_addr.s_addr = maskp;
  mask = inet_ntoa(mask_addr);
  printf("MSK : %s\n",mask);
  printf("=====================\n");

  pcd = pcap_open_live(dev, BUFSIZ, NONPROMISCUOUS, -1,errbuf);
  if(pcd==NULL){printf("%s\n",errbuf); exit(1);}

  if(pcap_compile(pcd, &fp, argv[2], 0, netp) == -1){
    printf("compile error\n");
    exit(1);
  }

  if(pcap_setfilter(pcd, &fp) == -1){
    printf("Setfilter error\n");
    exit(1);
  }

  pcap_loop(pcd,atoi(argv[1]), callback, NULL);
}

void callback(unsigned char *useless, const struct pcap_pkthdr *pkthdr, const unsigned char *packet){
  static int count = 1;
  int chcnt = 0;
  struct ether_header *ep;
  unsigned short ether_type;
  int length = pkthdr->len; // 패킷의 총 길이 (패킷의 크기)

  ep = (struct ether_header *)packet; //포인터변수 packet에서 이더넷 헤더만큼만(이더넷 헤더 구조체의 크기만큼) 캐스팅해서 데이터를 읽어옴
  packet += sizeof(struct ether_header); // packet은 포인터변수이므로 주소를 저장한다. 그러므로 데이터가 저장된 주소시작값을 변경해서 이더넷
                                         //   헤더만큼 읽은 다음 offset의 주소값부터 다음 데이터를 읽어오도록 offset을 변경해주는 작업

  ether_type = ntohs(ep->ether_type); // ether_header구조체의 멤버변수 ether_type는 2바이트로 패킷유형을 저장한다.
  
  //패킷유형이 IP패킷이라면
  if(ether_type==ETHERTYPE_IP){
    iph = (struct ip*)packet; // 포인터변수 packet에서 IP헤더만큼만 ( ip헤더 구조체의 크기만큼 ) 캐스팅해서 데이터를 읽어옴
    printf("ip패킷\n");
    printf("Version    : %d\n",iph->ip_v);
    printf("Header Len : %d\n",iph->ip_hl); // 5
    printf("Ident      : %d\n", ntohs(iph->ip_id));
    printf("TTL        : %d\n",iph->ip_ttl);
    printf("Src Address: %s\n",inet_ntoa(iph->ip_src));
    printf("Dst Address: %s\n",inet_ntoa(iph->ip_dst));

    if(iph->ip_p == IPPROTO_TCP){
      packet += (iph->ip_hl * 4); // 5 * 4 = 20 => 구조체 ip의 크기가 20이라서
      tcph = (struct tcphdr *)packet;
      printf("Src Port : %d\n",ntohs(tcph->th_sport));
      printf("Dst Port : %d\n",ntohs(tcph->th_dport));
    }

    //packet bytes ( 패킷 데이터 영역 )을 출력한다. 패킷 데이터 영역은 캡쳐된 패킷을 16진수나 ASCII코드 로 표시해준다.
    //패킷의 길이만큼 반복을 돌면서 한번 돌 때마다 packet에 있는 데이터를 1바이트씩 읽어옴. *(packet++)
    //패킷 데이터 영역은 ethernet,ip,tcp 헤더 다음에 있으므로 위에서 헤더데이터들을 다 가져온 후 그 다음 offset부터 출력하면 됨 
    while(length--){
      printf("%02x ", *(packet++)); // packet은 주소값이 들어있는 포인터변수이므로 이 안에 실제 값을 출력하려면 역참조해야 한다.
      if((++chcnt % 16) == 0){
        printf("\n");
      }
    }

  }else{
    printf("NONE IP 패킷\n");
  }
  printf("\n\n");
}
