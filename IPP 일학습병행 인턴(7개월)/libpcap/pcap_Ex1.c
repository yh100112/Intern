// pcap파일을 열어 1. 수집한 패킷 수 2.패킷별로 바이트 수
// => 위 두개를 콘솔 화면에 출력하는 프로그램 
// => 사용자가 파일명을 입력 => 파일을 열어 수집한 패킷수, 패킷별로 바이트 수를 콘솔 화면에 출력

//                          PCAP File Header
//  __________________________________________________________________________
//  |             4          |    2      |      2    |         4             |
//  __________________________________________________________________________
//  |  magic(0xa1b2c3d4)     | maj.ver   |  min.ver  |        시간 차이       |
//  _________________________________________________________________________
//  |             4          |           4           |          4            |
//   _________________________________________________________________________
//  |      캡쳐한 시각         |   snap의 최대 길이  |     datalink type     |
//  _________________________________________________________________________

//                             PCAP Data Header
//  _______________________________________________________________________________
//  |                    8                   |        4          |     4          |
//   ______________________________________________________________________________
//  | seconds(4)         | micro seconds(4)  |    캡쳐한 길이    |    패킷 길이   |
//  _______________________________________________________________________________


#include <stdio.h>
#include<string.h>
#define MAX_PACKET 30 // 최대 분석할 수 있는 패킷 개수
#define MAC_ADDR_LEN 6
#define MAGIC 0xa1b2c3d4 //pcap magic

//PCAP파일 헤더 구조체 정의
typedef struct _pcap_file_header
{
  int magic;
  unsigned short version_major;
  unsigned short version_minor;
  int thiszone; /* gmt to localcorrection */
  unsigned sigfigs; /* accuracy oftimestamps */
  unsigned snaplen; /* maxlength savedportion of each pkt */
  unsigned linktype; /* datalink type (LINKTYPE_*) */
} pcap_file_header;

// 타임 스탬프를 초단위와 마이크로초 단위의 구조체로 정의
typedef struct _timeval
{
  int tv_sec; /* seconds*/
  int tv_usec; /*andmicroseconds */
} timeval;

//패킷 헤더 구조체 정의
typedef struct pcap_pkthdr
{
  timeval ts; /*time stamp */
  unsigned int caplen; /* length of portionpresent */
  unsigned int len; /*length thispacket (off wire) */
} pcap_header;

typedef struct _ethernet{
  unsigned char dest_mac[MAC_ADDR_LEN]; // 맥주소 => 보통 a:b:c:d:e:f  와 같은 형태로 출력함
  unsigned char src_mac[MAC_ADDR_LEN];
  unsigned short type; // 프로토콜 타입 ( 2바이트 )
}ethernet;


pcap_header headers[MAX_PACKET]; //패킷 헤더들을 보관할배열
int pcnt;

int Parsing(FILE *fp); // 추적 파일을 분석하는 함수
void ViewPacketHeader(pcap_header *ph);
void ViewEthernet(char *buf);
void ParsingEthernet(FILE *fp);
void ViewMac(unsigned char *mac); 
void ViewIP(char *buf);
void ViewARP(char *buf);
unsigned short ntohs(unsigned short value);

int main(){
  char fname[256] = "demo.pcap";
  FILE *fp = 0;

  fp = fopen(fname,"rb");

  Parsing(fp); //분석하기

  fclose(fp); //파일 닫기
  return 0;
}

int Parsing(FILE *fp){
  pcap_file_header pfh;
  fread(&pfh, sizeof(pfh), 1, fp); //pcap file header 읽기
  if (pfh.magic != MAGIC){//magic이 다르면 에러
    printf("file format is not correct \n");
    return -1;
  }
  printf("version:%d.%d\n", pfh.version_major, pfh.version_minor); //2.4 출력됨 (pcap 헤더 정보 출력)
  
  //Ethernet 방식으로 분석 ( 이더넷 일 때만 분석하도록 코드 구현)
  switch (pfh.linktype){//링크 타입에 따라
  case 1: // ethernet 방식
    ParsingEthernet(fp);
    break; 
  case 6:
    printf("Not support Token Ring\n");
    break;
  case 10:
    printf("Not support FDDI\n");
    break;
  case 0:
    printf("Not support Loopback\n");
    break;
  default:
    printf("Unknown\n");
    break;
  }
  return 0;
}

//패킷헤더 정보를 출력하고 ethernet정보를 출력하는 함수 ( ethernet정보를 출력하는 부분은 빈 상태로 놔둘거임 )
void ParsingEthernet(FILE *fp){
  char buf[65536];
  pcap_header *ph = headers; //ph를 패킷 헤더들을 보관할 배열의 시작 위치로 초기화
  int i = 0;

  while (feof(fp) == 0){//파일의 끝이 아니면 반복

    //ph에 패킷 헤더 구조체 1개 사이즈만큼 fp에서 읽어와서 저장
    if (fread(ph, sizeof(pcap_header), 1, fp) != 1) {
      break;
    }

    if(pcnt == MAX_PACKET){ // 설정한 갯수만큼 패킷을 캡처하면 while문 탈출
      break;
    }

    ViewPacketHeader(ph); //패킷 헤더 정보 출력

    fread(buf, 1, ph->caplen, fp); //패킷 헤더의 캡쳐된 길이만큼 읽어서 buf에 저장
    ViewEthernet(buf);  // 이더넷 정보 출력
    
    ph++; //ph를 다음 위치로 이동
  }
}
void ViewPacketHeader(pcap_header *ph){
  pcnt++;
  printf("%d time:%08d:%06d caplen:%u length:%u\n",pcnt,ph->ts.tv_sec, ph->ts.tv_usec, ph->caplen, ph->len);
  return;
}

void ViewEthernet(char *buf){
  ethernet *ph = (ethernet*)buf;
  printf("====================ETHERNET Header===================\n");
  printf("dest mac : 0x");
  ViewMac(ph->dest_mac);
  printf("    src mac :0x");
  ViewMac(ph->src_mac);
  printf("     type : 0x%x\n\n", ntohs(ph->type)); // link타입 출력 (2바이트 이상 데이터는 network byte order -> host byte order로 변환해야함)

  switch(ntohs(ph->type)){
    case 0x0800:
      ViewIP(buf + sizeof(ethernet));
      break;
    case 0x0806:
      ViewARP(buf + sizeof(ethernet));
      break;
    default:
      printf("Not support Protocol\n");
      break;
  }
}

void ViewMac(unsigned char *mac){
  int i;
  for(i = 0;i < MAC_ADDR_LEN; ++i){
    printf("%02x ", mac[i]);
  }
}

unsigned short ntohs(unsigned short value){
  return (value << 8) | (value >> 8); // 상위 바이트와 하위바이트 위치 교환
}

void ViewIP(char *buf){
  //to be defined
}

void ViewARP(char *buf){
  //to be defined
}