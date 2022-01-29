#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{

    int client_len;
    int serv_sockfd;
    char buf[256];
    struct sockaddr_in clientaddr;

    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientaddr.sin_port = htons(8002);
    client_len = sizeof(clientaddr);

    serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect (serv_sockfd, (struct sockaddr *)&clientaddr,client_len) < 0)
    {
        perror("connect error :");
        exit(0);
    }

    int nbyte;
    size_t filesize = 0, bufsize = 0;
    FILE *file = fopen("test.jpg", "wb");

    ntohl(filesize); // 네트워크(서버)에서 호스트(클라이언트)로 long(ip주소)를 바꿔줌
    recv(serv_sockfd, &filesize, sizeof(filesize), 0);
    printf("file size = [%d]\n", filesize);
    bufsize = 256;
    while(filesize != 0)
    {
        if(filesize < 256)
            bufsize = filesize;

        nbyte = recv(serv_sockfd, buf, bufsize, 0);
        filesize = filesize -nbyte;

        fwrite(buf, sizeof(char), nbyte, file);

        nbyte = 0;

    }

    close(serv_sockfd);
    fclose(file);
    return 0;
}