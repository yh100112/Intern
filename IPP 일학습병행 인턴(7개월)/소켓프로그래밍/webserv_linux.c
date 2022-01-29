#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void* request_handler(void* arg);
void send_data(FILE* fp, char* ct, char* file_name);
char* content_type(char* file);
void send_error(FILE* fp);
void error_handling(char* message);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_size;
	char buf[BUF_SIZE];
	pthread_t t_id;	
	if(argc!=2) {
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY); 
	serv_adr.sin_port = htons(atoi(argv[1]));
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1) error_handling("bind() error");
	if(listen(serv_sock, 20)==-1) error_handling("listen() error");

	while(1)
	{
		clnt_adr_size=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_size);
		printf("Connection Request : %s:%d\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
		pthread_create(&t_id, NULL, request_handler, &clnt_sock);
		pthread_detach(t_id);
	}
	close(serv_sock);
	return 0;
}

void* request_handler(void *arg)
{
	int clnt_sock=*((int*)arg);
	char req_line[SMALL_BUF];

    char method[10];
	char ct[15];
	char file_name[30];
	
    //입출력 분할
	FILE* clnt_read = fdopen(clnt_sock, "r"); // 클라이언트 소켓에서 데이터를 읽어옴 ( 클라이언트 소켓 디스크립터번호로 클라이언트 소켓을 파일처럼 염)
                                                //처음에는 request headers만 있고 response headers는 사용자가 써줘야함
	FILE* clnt_write = fdopen(dup(clnt_sock), "w"); // 클라이언트 소켓에 데이터를 보냄
	
	fgets(req_line, SMALL_BUF, clnt_read);	//클라이언트로부터 데이터 읽어들임 request headers(한줄씩)
    printf("%s\n",req_line); // "GET /index.html HTTP/1.1"
    
	if(strstr(req_line, "HTTP/")==NULL) // "HTTP/"가 없으면 NULL반환
	{
		send_error(clnt_write);
		fclose(clnt_read);
		fclose(clnt_write);
		return NULL;
	 }
	
	strcpy(method, strtok(req_line, " /")); //GET
	strcpy(file_name, strtok(NULL, " /")); // index.html
	strcpy(ct, content_type(file_name)); //content-type을 넣어줌
	if(strcmp(method, "GET")!=0) // method가 GET과 일치하지 않으면
	{
		send_error(clnt_write);
		fclose(clnt_read);
		fclose(clnt_write);
		return NULL;
	 }

	fclose(clnt_read);
	send_data(clnt_write, ct, file_name); 
}

void send_data(FILE* fp, char* ct, char* file_name)
{
	char protocol[]="HTTP/1.0 200 OK\r\n";
	char server[]="Server:Linux Web Server \r\n";
	char cnt_len[]="Content-length:2048\r\n";
	char cnt_type[SMALL_BUF];
	char buf[BUF_SIZE];
	FILE* send_file = fopen(file_name, "r"); // 클라이언트가 요청한 index.html파일 열기
	
	sprintf(cnt_type, "Content-type:%s\r\n\r\n", ct); //메시지 헤더와 몸체 사이 공백 라인으로 구분하기 위해 \r\n 한번더 해줌
    //즉, Content-type:%s 이 문장이 메시지 헤더의 마지막 라인이라는 뜻!!
    //메시지 몸체를 헤더와 분리하지 않으면 html파일의 코드가 그대로 응답헤더에 기록됨. 웹 브라우저 화면에 출력 안됨

    if(send_file==NULL)
	{
		send_error(fp);
		return;
	}

	/* 헤더 정보 전송 */
	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);

	/* 요청 데이터 전송 */
	while(fgets(buf, BUF_SIZE, send_file)!=NULL)  // 한 줄씩 읽어옴
	{
		fputs(buf, fp);//읽어온 걸 씀
		fflush(fp);
	}
	fflush(fp); // 혹시 버퍼에 남아있는 게 있을 수 있으니 다 보내줌
	fclose(fp); // HTTP프로토콜에 의해 응답 후 종료
}

char* content_type(char* file) //Content-type 구분
{
	char extension[SMALL_BUF];
	char file_name[SMALL_BUF];
	strcpy(file_name, file); // index.html
	strtok(file_name, "."); // index
	strcpy(extension, strtok(NULL, ".")); // html
	
	if(!strcmp(extension, "html")||!strcmp(extension, "htm")) 
		return "text/html";
	else
		return "text/plain";
}

//클라이언트 소켓(웹 브라우저)의 response headers에 써줄 내용
void send_error(FILE* fp)
{	
	char protocol[]="HTTP/1.0 400 Bad Request\r\n";
	char server[]="Server:Linux Web Server \r\n";
	char cnt_len[]="Content-length:2048\r\n";
	char cnt_type[]="Content-type:text/html\r\n\r\n";
	char content[]=
    "<meta charset=\"UTF-8\">"
    "<html><head><title>NETWORK</title></head>"
    "<body><font size=+5><br>오류 발생! 요청 파일명 및 요청 방식 확인!"
    "</font></body></html>";

	fputs(protocol, fp);
	fputs(server, fp);
	fputs(cnt_len, fp);
	fputs(cnt_type, fp);
    fputs(content,fp);
	fflush(fp);
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
