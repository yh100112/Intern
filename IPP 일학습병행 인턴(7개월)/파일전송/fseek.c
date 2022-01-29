#include<stdio.h>

int main(int argc, char* argv[]){
    char tmp[10];
    FILE* fp = fopen("test.txt","r");
    fgets(tmp,3,fp); // fgets는 맨 뒤에 널값을 자동으로 넣어주기 때문에 널크기 1바이트를 제외한 01까지 읽어옴
    printf("%s ",tmp);

    fseek(fp,4,0); // 0123을 건너뜀. 이제 현재위치는 4이다.
    fgets(tmp,3,fp);
    printf("%s ",tmp);

    fseek(fp,2,1); // 이제 현재위치가 6이므로 6에서 + 2로 6,7을 건너뜀
    fgets(tmp,3,fp); // 89를 읽어옴
    printf("%s ",tmp);

    fseek(fp,-2,2); // 파일의 끝(eof)에서 두칸 당겨오므로 현재 위치는 e이다.
    fgets(tmp,3,fp);
    printf("%s ",tmp);

    return 0;

}