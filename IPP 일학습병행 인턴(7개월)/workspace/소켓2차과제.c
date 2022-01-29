#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 4096
#define SMALL_BUF 100
#define DELIM " "

void *request_handler(void *arg);
void send_data(FILE *fp, char *file_name, char *req_line, char *version);
char *content_type(char *file);
void send_error(FILE *fp);
void error_handling(char *message);

int main(int argc, char *argv[])
{
  int serv_sock, clnt_sock;
  struct sockaddr_in serv_adr, clnt_adr;
  int clnt_adr_size;
  char buf[BUF_SIZE];
  pthread_t t_id;
  if (argc != 2)
  {
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_adr.sin_port = htons(atoi(argv[1]));
  if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
    error_handling("bind() error");
  if (listen(serv_sock, 20) == -1)
    error_handling("listen() error");

  while (1)
  {
    clnt_adr_size = sizeof(clnt_adr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_size);
    printf("Connection Request : %s:%d\n", inet_ntoa(clnt_adr.sin_addr), ntohs(clnt_adr.sin_port));
    pthread_create(&t_id, NULL, request_handler, &clnt_sock);
    pthread_detach(t_id);
  }
  close(serv_sock);
  return 0;
}

void *request_handler(void *arg)
{
  int clnt_sock = *((int *)arg);
  char req_line[SMALL_BUF];
  char rem_line[BUF_SIZE];

  char method[10];
  char ct[15];
  char file_name[30];
  char version[30];

  char *next_ptr;

  FILE *clnt_read = fdopen(clnt_sock, "r");
  FILE *clnt_write = fdopen(dup(clnt_sock), "w");

  fgets(req_line, SMALL_BUF, clnt_read);
  while (strstr(req_line, "\r\n") == NULL)
  {
    fgets(req_line, 4, clnt_read);
    printf("req_line -> %s\n",req_line);
  }

  strcpy(method, strtok_r(req_line, " ", &next_ptr)); //GET
  strcpy(file_name, strtok_r(NULL, " ", &next_ptr));  // index.html
  strcpy(version, strtok_r(NULL, " ", &next_ptr));    // HTTP/1.1\r\n

  printf("Method : %s\n", method);
  printf("URL : %s\n", file_name);
  printf("Version : %s\n", version);

  send_data(clnt_write, file_name, req_line, version);

  while (fgets(rem_line, BUF_SIZE, clnt_read) != NULL)
  {
    printf("%s", rem_line);
  }
  fclose(clnt_read);
}

void send_data(FILE *fp, char *file_name, char *req_line, char *version)
{
  const char *const MESSAGE_200 =
      "HTTP/1.1 200 OK\r\n"
      "Cache-Control: no-cache, no-store, max-age=0, must-revalidate\r\n"
      "Expires: 0\r\n"
      "Connection: close\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 112\r\n"
      "\r\n"
      "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>SYSONE</title></head><body><h1>SYSONE</h1></body></html>";

  char buf[BUF_SIZE];

  strcat(req_line, " ");
  strcat(req_line, file_name);
  strcat(req_line, " ");
  strcat(req_line, version);

  if (strcmp(req_line, "GET / HTTP/1.1\r\n") == 0)
  {
    fprintf(fp, "%s\0", MESSAGE_200);
    fflush(fp);
  }
  else
  {
    send_error(fp);
    fclose(fp);
    return;
  }

  fclose(fp);
  return;
}

void send_error(FILE *fp)
{
  const char *const MESSAGE_404 =
      "HTTP/1.1 404 Not Found\r\n"
      "Cache-Control: no-cache, no-store, max-age=0, must-revalidate\r\n"
      "Expires: 0\r\n"
      "Connection: close\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 110\r\n\r\n"
      "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>ERROR</title></head><body><h1>ERROR</h1></body></html>";

  fprintf(fp, "%s", MESSAGE_404);
  fflush(fp);
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
