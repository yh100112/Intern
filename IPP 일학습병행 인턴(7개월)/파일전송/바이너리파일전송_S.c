#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
    int server_sockfd, client_sockfd;
    int state, client_len;

    FILE *file = NULL;

    int ret;
    struct stat file_stat;

    struct sockaddr_in clientaddr, serveraddr;
    size_t fsize = 0, nsize = 0, fpsize = 0;
    size_t fsize2 = 0;
    size_t fsize3 = 0;

    char buf[256];
    memset(buf, 0x00, 256);
    state = 0;

    client_len = sizeof(clientaddr);

    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error : ");
        exit(0);
    }

    memset(&serveraddr,0x00, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(8002);

    state = bind(server_sockfd , (struct sockaddr *)&serveraddr,sizeof(serveraddr));
    if (state == -1) {
        perror("bind error : ");
        exit(0);
    }
    state = listen(server_sockfd, 5);
    if (state == -1) {
        perror("listen error : ");
        exit(0);
    }



    file = fopen("test.jpg", "rb");
    fseek(file, 0, SEEK_END); //파일의 마지막 위치를 기준으로 0칸 이동 => 마지막 위치가 됨
    fsize = ftell(file); // 파일의 현재 위치가 마지막 위치이므로 마지막 위치 포인터 반환
    fseek(file, 0, SEEK_SET); // 파일의 처음 위치를 기준으로 0칸 이동 => 마지막 위치로 간 offset을 다시 처음 위치로 바꿔줌

    client_sockfd = accept(server_sockfd,(struct sockaddr *)&clientaddr, &client_len);
    printf("file size [%d]\n", fsize);
    fsize2 = htonl(fsize);
    printf("file size [%d]\n", fsize2);

    send(client_sockfd, &fsize2, sizeof(fsize), 0);

    while(nsize != fsize) // 읽은 바이트수가 파일의 총길이가 되면  while문을 탈출
    {
        fpsize = fread(buf, 1, 256, file); //읽은 바이트수 반환
        nsize += fpsize; // nsize에 fpsize(읽은 바이트수) 를 더함
        send(client_sockfd, buf, fpsize, 0);
    }
    printf("file send \n");
    close(client_sockfd);
    fclose(file);

    return 0;
}