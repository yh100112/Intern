#include<stdio.h>

int main(){
    char tmp[20];
    FILE* fp = fopen("test.txt","r");
    printf("fscanf전 위치 :%d\n", ftell(fp)); // 아직 파일포인터를 사용한 작업이 없으므로 현재위치는 0

    fgets(tmp,5,fp); // fgets를 통해 null을 포함한 4글자를 가져옴(null이 1바이트이므로 5글자가 아닌 4글자가져옴)
    printf("fgets로 4개를 가져온 후 위치 : %d\n",ftell(fp));

    fscanf(fp,"%s", tmp); // 나머지 문자들을 fscanf를 통해 모두 긁어옴

    printf("fscanf 후 위치 :%d\n", ftell(fp));
    printf("%s\n",tmp); // 456789abcdef




}