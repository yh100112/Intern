#include<sys/time.h>
#include<pcap/pcap.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<stdio.h>
#include<string>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<net/ethernet.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<unordered_map>
#include "/home/kdy/postgresql/include/libpq-fe.h"
#include<iostream>
using namespace std;

#define MAXLEN 4096
#define MAC_ADDR_LEN 6

struct IpPort{
  uint32_t srcIp;
  uint32_t dstIp;
  uint16_t srcPort;
  uint16_t dstPort;
};

struct TcpSession{
  string srcFilename; // srcIp_srcPort_dstIp_dstPort_생성시간
  string dstFilename; // dstIp_dstPort_srcIp_srcPort_생성시간
  bool srcFinReceived;
  bool dstFinReceived;
  bool rstReceived;

  TcpSession():srcFinReceived(false),dstFinReceived(false),rstReceived(false){}
};

struct IpPortHash{
  size_t operator () (const IpPort& refIpPort) const {
    return (size_t)(refIpPort.srcIp + refIpPort.dstIp + refIpPort.srcPort + refIpPort.dstPort);
  }
};

// flow는 양방향이므로 IP, 포트 방향이 뒤바뀌어 있는 것도 같은 flow로 취급해야 한다.
struct IpPortCompare{
  bool operator () (const IpPort& refIpPortL, const IpPort& refIpPortR) const {
    return (((refIpPortL.srcIp == refIpPortR.srcIp) && (refIpPortL.dstIp == refIpPortR.dstIp) && (refIpPortL.srcPort == refIpPortR.srcPort) && (refIpPortL.dstPort == refIpPortR.dstPort)) ||
            ((refIpPortL.srcIp == refIpPortR.dstIp) && (refIpPortL.dstIp == refIpPortR.srcIp) && (refIpPortL.srcPort == refIpPortR.dstPort) && (refIpPortL.dstPort == refIpPortR.srcPort)));
  }
};

//패킷헤더 구조체
typedef struct _pcap_data_header{
  int sec;
  int usec;
  unsigned int caplen;
  unsigned int len;
}pcap_data_header;

//공유메모리
typedef struct SharedMemory_S{
  pthread_mutex_t sharedMutex;
  pthread_mutexattr_t sharedMutexAttr;
  pthread_cond_t sharedCond;
  pthread_condattr_t sharedCondAttr;
  pcap_data_header pcapHdr;
  unsigned char buf[MAXLEN];
}SharedMemory;

typedef unordered_map<IpPort,TcpSession,IpPortHash,IpPortCompare> FlowMap;

static volatile __sig_atomic_t is_terminated = 0;
void signal_handler(int unused);

const char* PCAP_WRITE_PATH = "/home/kdy/project/패킷캡처과제/tcp_data";
int fin_check = 0;

int main(int argc, char* argv[]){
  int shmid;
  int length = 0;
  int count;
  SharedMemory *shm;


  PGconn* conn;
  conn = PQconnectdb("host=/tmp user=dkyong dbname=dkyongdb");
  if(PQstatus(conn) == CONNECTION_BAD){
    fprintf(stderr,"%s\n",PQerrorMessage(conn));
    PQfinish(conn);
    return -1;
  }

  shmid = shmget((key_t)4000, sizeof(SharedMemory), 0666);
  if(shmid == -1){
    printf("shmget error\n");
    exit(1);
  }

  shm = (SharedMemory*)shmat(shmid, 0, 0);

  pthread_mutexattr_init(&(shm->sharedMutexAttr));
  pthread_mutexattr_setpshared(&(shm->sharedMutexAttr),PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(shm->sharedMutex),&(shm->sharedMutexAttr));

  pthread_condattr_init(&(shm->sharedCondAttr));
  pthread_condattr_setpshared(&(shm->sharedCondAttr),PTHREAD_PROCESS_SHARED);
  pthread_cond_init(&(shm->sharedCond),&(shm->sharedCondAttr)); 

  struct sigaction new_act;
  new_act.sa_handler = signal_handler;
  sigemptyset(&new_act.sa_mask);
  sigaction(SIGINT, &new_act,NULL);

  int cnt = 0;
  const unsigned char *data;
  char query[200];
  FlowMap map;

  while(!is_terminated){
    pthread_mutex_lock(&(shm->sharedMutex));
      pthread_cond_wait(&(shm->sharedCond),&(shm->sharedMutex));
      printf("읽은 패킷 갯수 : %d\n\n",cnt+1);
      printf("===== PCAP ===== \n");
      printf("Captured Time   : %d.%06d\n",shm->pcapHdr.sec,shm->pcapHdr.usec);
      printf("Captured Length : %d\n",shm->pcapHdr.caplen);
      printf("Length          : %d\n",shm->pcapHdr.len);

      PGresult* resp;

      memset(query,0,sizeof(query));
      snprintf(query,sizeof(query),
      "insert into packet(captured_time,captured_length,length) values(%d.%06d,%d,%d)",
      shm->pcapHdr.sec,shm->pcapHdr.usec,shm->pcapHdr.caplen,shm->pcapHdr.len);
      resp = PQexec(conn,query);
      if(PQresultStatus(resp) != PGRES_COMMAND_OK){
        fprintf(stderr,"%s\n",PQresultErrorMessage(resp));
        PQclear(resp);
        PQfinish(conn);
        exit(-1);
      }

      int pkt_length = shm->pcapHdr.len;
      struct ether_header *ethernet;
      const unsigned char *data = shm->buf;
      ethernet = (struct ether_header *)data;
      data += sizeof(struct ether_header);

      pkt_length -= sizeof(struct ether_header);

      printf("===== Ethernet ===== \n");
      printf("Source MAC      : %02x:%02x:%02x:%02x:%02x:%02x\n",
              ethernet->ether_shost[0],
              ethernet->ether_shost[1],
              ethernet->ether_shost[2],
              ethernet->ether_shost[3],
              ethernet->ether_shost[4],
              ethernet->ether_shost[5]);
      printf("Destination MAC : %02x:%02x:%02x:%02x:%02x:%02x\n",
              ethernet->ether_dhost[0],
              ethernet->ether_dhost[1],
              ethernet->ether_dhost[2],
              ethernet->ether_dhost[3],
              ethernet->ether_dhost[4],
              ethernet->ether_dhost[5]);
      printf("EtherType       : 0x%x\n", ntohs(ethernet->ether_type));

      char s_mac[100] = {0,};
      char d_mac[100] = {0,};
      
      snprintf(s_mac,sizeof(s_mac),"%02x:%02x:%02x:%02x:%02x:%02x",
      ethernet->ether_shost[0],
      ethernet->ether_shost[1],
      ethernet->ether_shost[2],
      ethernet->ether_shost[3],
      ethernet->ether_shost[4],
      ethernet->ether_shost[5]);

      snprintf(d_mac,sizeof(d_mac),"%02x:%02x:%02x:%02x:%02x:%02x",
      ethernet->ether_dhost[0],
      ethernet->ether_dhost[1],
      ethernet->ether_dhost[2],
      ethernet->ether_dhost[3],
      ethernet->ether_dhost[4],
      ethernet->ether_dhost[5]);      

      memset(query,0,sizeof(query));
      snprintf(query,sizeof(query),
      "UPDATE packet SET source_mac='%s', destination_mac='%s',ethertype=%x where captured_time=%d.%06d",
      s_mac,d_mac,ntohs(ethernet->ether_type),shm->pcapHdr.sec,shm->pcapHdr.usec);
      resp = PQexec(conn,query);
      if(PQresultStatus(resp) != PGRES_COMMAND_OK){
        fprintf(stderr,"%s\n",PQresultErrorMessage(resp));
        PQclear(resp);
        PQfinish(conn);
        exit(-1);
      }

      struct ip *iph;
      struct tcphdr *tcph;

      if(ntohs(ethernet->ether_type) == ETHERTYPE_IP){
        iph = (struct ip*)data;
        pkt_length -= sizeof(struct ip);

        char ip_src[100]={0,};
        char ip_dst[100]={0,};

        printf("===== IpV4 =====\n");
        printf("Source Addr       : %s\n",inet_ntoa(iph->ip_src));
        snprintf(ip_src,sizeof(ip_src),"%s",inet_ntoa(iph->ip_src));
        printf("Destination Addr  : %s\n",inet_ntoa(iph->ip_dst));
        snprintf(ip_dst,sizeof(ip_dst),"%s",inet_ntoa(iph->ip_dst));
        printf("Header Length     : %d\n",iph->ip_hl * 4);
        printf("Ip Data Length    : %d\n",iph->ip_len);
        
        memset(query,0,sizeof(query));
        snprintf(query,sizeof(query),
        "UPDATE packet SET source_addr='%s', destination_addr='%s', header_length=%d, ip_data_length=%d where captured_time=%d.%06d",
        ip_src, ip_dst, iph->ip_hl*4, iph->ip_len, shm->pcapHdr.sec, shm->pcapHdr.usec);
        resp = PQexec(conn,query);
        if(PQresultStatus(resp) != PGRES_COMMAND_OK){
          fprintf(stderr,"%s\n",PQresultErrorMessage(resp));
          PQclear(resp);
          PQfinish(conn);
          exit(-1);
        }

        //ipv4이면서 tcp인 경우
        if(iph->ip_p == IPPROTO_TCP){
          data += (iph->ip_hl * 4);
          tcph = (struct tcphdr *)data;
          pkt_length -= sizeof(struct tcphdr);

          data += sizeof(struct tcphdr);

          printf("===== Tcp/Udp =====\n");
          printf("Source Port       : %d\n",ntohs(tcph->th_sport));
          printf("Destination Port  : %d\n",ntohs(tcph->th_dport));
          printf("Tcp Data Length   : %d\n",pkt_length);

          memset(query,0,sizeof(query));
          snprintf(query,sizeof(query),
          "UPDATE packet SET source_port=%d, destination_port=%d, tcp_data_length=%d where captured_time=%d.%06d",
          ntohs(tcph->th_sport), ntohs(tcph->th_dport), pkt_length, shm->pcapHdr.sec, shm->pcapHdr.usec);
          resp = PQexec(conn,query);
          if(PQresultStatus(resp) != PGRES_COMMAND_OK){
            fprintf(stderr,"%s\n",PQresultErrorMessage(resp));
            PQclear(resp);
            PQfinish(conn);
            exit(-1);
          }   

          struct IpPort ipport;
          struct TcpSession tcp;
          ipport.srcIp = inet_addr(ip_src);
          ipport.dstIp = inet_addr(ip_dst);
          ipport.srcPort = ntohs(tcph->th_sport);
          ipport.dstPort = ntohs(tcph->th_dport);

          char srcFname[256];
          char dstFname[256];
          char timestr[64];
          char buf[64];
          char buf2[64];
          struct timeval tv;
          time_t time;
          struct tm tm;
          gettimeofday(&tv,NULL);
          time = tv.tv_sec;
          localtime_r(&time,&tm);
          strftime(timestr,sizeof(timestr),"%Y%m%d_%H%M%S",&tm);
          snprintf(buf,sizeof(buf),"%d_%d_%d_%d %s_%06d.bin",ipport.srcIp,ipport.srcPort,ipport.dstIp,ipport.dstPort,timestr,tv.tv_usec);
          snprintf(buf2,sizeof(buf2),"%d_%d_%d_%d %s_%06d.bin",ipport.dstIp,ipport.dstPort,ipport.srcIp,ipport.srcPort,timestr,tv.tv_usec);
          
          tcp.srcFilename = buf;
          tcp.dstFilename = buf2;
          
          pair<unordered_map<IpPort,TcpSession,IpPortHash,IpPortCompare>::iterator,bool> test;
          test = map.insert({ipport,tcp});

          if(test.second == false){ // tcp flow가 이미 있을 때
            printf("이미 동일한 키가 있음 (동일한 flow임)\n");

            char src_filename[256];
            char dst_filename[256];
            char open_filename[256];
            int fd;

            snprintf(src_filename,sizeof(src_filename),"%s",map[ipport].srcFilename.c_str());
            snprintf(dst_filename,sizeof(dst_filename),"%s",map[ipport].dstFilename.c_str());

            char *srcTest;
            char *next_ptr;
            srcTest = strtok_r(src_filename,"_",&next_ptr);

            if(ipport.srcIp == atoi(srcTest)){
              printf("src->dst 패킷!!\n");

              snprintf(open_filename,sizeof(open_filename),"%s/%s",PCAP_WRITE_PATH,map[ipport].srcFilename.c_str());
              fd = open(open_filename,O_WRONLY | O_CREAT | O_APPEND, 0644);
              if(fd < 0){
                printf("file_open_error!!!\n");
                break;
              }else{
                printf("파일 %s 오픈 or 생성\n",open_filename);
              }

              int write_len = write(fd,data,pkt_length);
              if( write_len == -1){
                printf("write error\n");
                break;
              }else{
                printf("쓴 길이 : %d\n", write_len);
              }

              close(fd);
            }else{
              printf("dst->src 패킷!!\n");

              snprintf(open_filename,sizeof(open_filename),"%s/%s",PCAP_WRITE_PATH,dst_filename);
              fd = open(open_filename,O_WRONLY | O_CREAT | O_APPEND, 0644);
              if(fd < 0){
                printf("file_open_error!!!\n");
                break;
              }else{
                printf("파일 %s 오픈 or 생성\n",open_filename);
              }

              int write_len = write(fd,data,pkt_length);
              if( write_len == -1){
                printf("write error\n");
                break;
              }else{
                printf("쓴 길이 : %d\n", write_len);
              }

              close(fd);              
            }

          }else // tcp flow가 새로운 게 생성됐을 때
          {
            printf("새로운 flow 생성\n");
            int fd;
            char f_name[256];
            char real_filename[256];
            snprintf(f_name,sizeof(f_name),"%s",tcp.srcFilename.c_str());
            snprintf(real_filename,sizeof(real_filename),"%s/%s",PCAP_WRITE_PATH,f_name);
            fd = open(real_filename,O_WRONLY | O_CREAT, 0644);
            if(fd > 0){
              printf("파일 생성! %s\n",f_name);
            }
            int write_len = write(fd,data,pkt_length);
            if( write_len == -1){
              printf("write error\n");
              exit(1);
            }else{
              printf("%s에 쓴 길이 : %d\n",f_name, write_len);
            }
            close(fd);
          }

          if(tcph->th_flags & TH_RST){
            map.erase(ipport); //맵에서 tcp flow 삭제-
            printf("tcp flow 삭제 완료 ( RST flag로 인한 삭제 ★★★★★★★★★★★★★★ )\n");
          }

          if(tcph->th_flags & TH_FIN){
            printf("FIN 전송!★★★★★★★★★★★★★★★★★★★\n");
            fin_check++;

            if(fin_check >= 2){
              fin_check = 0;
              map.erase(ipport);
              printf("tcp flow 삭제 완료★★★★★★★★★★★★★★★★★★★★★★★★★★★★★v\n");
            }

          }

          printf("unordered_map size : %d\n",map.size());

        }
      }

      length = shm->pcapHdr.caplen;
      count = 0;
      data = shm->buf;
      while(length--){
        printf("%02x ", *data++);
        if(++count % 16 == 0){
          printf("\n");
        }
      }
      printf("\n\n");        
      PQclear(resp);
      cnt++;
    pthread_mutex_unlock(&(shm->sharedMutex));
  }

  printf("while문 탈출\n");
  PQfinish(conn);
}

void signal_handler(int unused){
  printf("signal~\n");
  is_terminated = 1;
  exit(1); // pthread_cond_wait()부분에서 멈춰있어서 신호를 받을 때까지 대기한다.
           //만약 writer에서 캡처를 하는 도중 중간에 종료해버리면 signal을 안 받기 때문에 계속 대기해서 꺼지지 않아
          // exit() 사용
}