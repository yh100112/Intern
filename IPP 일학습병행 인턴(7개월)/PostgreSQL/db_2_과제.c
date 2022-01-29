#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<stdbool.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include "/home/kdy/postgresql/include/libpq-fe.h"

int getch();
void passwd();
void wrong_input();
void go_notice(PGresult** menu_name_res, PGconn** conn,int number);
void go_agora(PGresult** menu_name_res, PGconn** conn,int number);
void go_bbs(PGresult** menu_name_res, PGconn** conn,char* member_name,char* member_id,int number,int* check);

int main(int argc, char* argv[]){
  int tuple,column;
  char id[20] = {0,};
  char password[20] = {0,};
  char member_name[20] = {0,};
  char member_id[20] = {0,};
  PGresult* res;
  PGconn* conn = PQconnectdb("user=dkyong dbname=dkyongdb");

  if(PQstatus(conn) == CONNECTION_BAD){
    fprintf(stderr,"%s\n",PQerrorMessage(conn));
    PQfinish(conn);
    exit(-1);
  }

  res = PQexec(conn,"select member_id,member_pw,member_name from tbl_member");
  if(PQresultStatus(res) != PGRES_TUPLES_OK){
    fprintf(stderr,"No data not found\n");
    PQclear(res);
    PQfinish(conn);
    exit(-1);
  }

  while(true){
    // bool check = false;
    int check = 0;

    printf("회원 ID : ");
    fgets(id,sizeof(id),stdin);
    id[strlen(id) - 1] = '\0';

    passwd(password);
    
    memset(member_id,0,sizeof(member_id));
    memset(member_name,0,sizeof(member_name));

    tuple = PQntuples(res);
    for(int i = 0;i < tuple; i++){
      if(strcmp(PQgetvalue(res,i,0),id) == 0){
        if(strcmp(PQgetvalue(res,i,1),password) == 0){
          snprintf(member_name,sizeof(member_name),PQgetvalue(res,i,2));
          snprintf(member_id,sizeof(member_id),PQgetvalue(res,i,0));
          printf("\nSysOne에 오신 것을 환영합니다.\n- 아무 키나 누르세요 -\n",member_name);
          check = 1;
          getch();
          system("clear");
          break;
        }
      }
    }
    if(check == 0){
      printf("\n잘못된 회원 정보입니다.\n- 아무 키나 누르세요 -\n");
      getch();
      system("clear");
      continue;
    }  

    while(check){ // 메뉴 화면
      int column;
      char choose[20] = {0,};
      char menu_name[50] = {0,};
      char title[50] = {0,};
      char query[200] = {0,};

      printf("사용자 : %s\n===== 메뉴 =====\n\n0. 로그아웃\n1. 공지/notice\n2. 게시판/bbs\n3. 토론방/agora\n\n번호 혹은 go alias > ",member_name);
      fgets(choose,sizeof(choose),stdin);
      choose[strlen(choose) - 1] = '\0';      
      
      if(strcmp(choose,"0") == 0)
      {
        printf("로그아웃 되었습니다.\n- 아무 키나 누르세요 -\n");
        getch();
        system("clear");
        break;
      }
      else
      {
        int number = atoi(choose);
        if(number == 0){ // go alias 입력
          PGresult* menu_name_res;
          if(strcmp(choose,"go notice") == 0)
          {
            go_notice(&menu_name_res,&conn,1);
          }
          else if(strcmp(choose,"go bbs") == 0)
          {
            go_bbs(&menu_name_res,&conn,member_name,member_id,2,&check);
          }
          else if(strcmp(choose,"go agora") == 0)
          {
            go_agora(&menu_name_res,&conn,3);
          }
          else{
            wrong_input();
            continue;       
          }
          PQclear(menu_name_res);
        }
        else if (number != 0){ // 번호 입력

          PGresult* menu_name_res;
          if(number == 1) // 메뉴1 : 공지
          {
            go_notice(&menu_name_res,&conn,number);
          }
          else if(number == 2) // 메뉴2 : 게시판
          {
            go_bbs(&menu_name_res,&conn,member_name,member_id,number,&check);
          }
          else if(number == 3) // 메뉴3 : 토론방
          {
            go_agora(&menu_name_res,&conn,number);
          }
          else{
            wrong_input();
            continue;  
          }
          PQclear(menu_name_res);
        }
      }  
    }
  }
  PQclear(res);
  PQfinish(conn);
  return 0;
}

void wrong_input(){
  printf("잘못된 선택입니다.\n- 아무 키나 누르세요 -\n");
  getch();
  system("clear");  
  return;
}

void go_bbs(PGresult** menu_name_res, PGconn** conn,char* member_name,char* member_id,int number,int* check){
  int curr_page = 0;
  int tuple = 0;
  int column = 0;
  char input[20] = {0,};
  char query[200] = {0,};

  memset(query,0,sizeof(query));
  snprintf(query,sizeof(query),"select menu_name from tbl_menu where menu_idx=%d",number);
  *menu_name_res = PQexec(*conn,query);

  if(PQresultStatus(*menu_name_res) != PGRES_TUPLES_OK){
    fprintf(stderr,"menu_name_res No data not found\n");
    PQclear(*menu_name_res);
    PQfinish(*conn);
    exit(-1);
  }

  while(true){ // 게시판
    system("clear");
    printf("사용자 : %s\n",member_name);
    printf("===== %s =====\n\n ",PQgetvalue(*menu_name_res,0,0));

    memset(query,0,sizeof(query));
    snprintf(query,sizeof(query),"select article_idx,member_id,member_name,title,write_date from tbl_article where menu_idx=%d and is_deleted=false order by article_idx desc offset (%d * 10) limit 10",number,curr_page);
    PGresult* select_article_res = PQexec(*conn,query); 

    tuple = PQntuples(select_article_res);
    column = PQnfields(select_article_res);
    for(int i = 0;i < tuple; i++){
      for(int j = 0; j < column; j++){
        printf("%s   ",PQgetvalue(select_article_res,i,j));
      }
      printf("\n");
    }
    PQclear(select_article_res);
    

    printf("\n=====\n\n");
    printf("0: 로그아웃, C (본문) : 작성, D (번호) : 삭제, M(메뉴로), N (다음 페이지), P(이전 페이지), go alias\n> ");
    fgets(input,sizeof(input),stdin);
    input[strlen(input) - 1] = '\0';

    char* first_input; 
    char* second_input;
    char* next_ptr;
    first_input = strtok_r(input," ",&next_ptr);
    second_input = strtok_r(NULL," ",&next_ptr);

    if(strstr(input,"0")){

      printf("로그아웃 되었습니다.\n- 아무 키나 누르세요 -\n");
      getch();
      system("clear");
      break;

    }else if(strstr(first_input,"C")){

      memset(query,0,sizeof(query));
      snprintf(query,sizeof(query),"insert into tbl_article(menu_idx,member_id,member_name,title,write_date) values(2,'%s','%s','%s',NOW())",member_id,member_name,second_input);
      PGresult* insert_res = PQexec(*conn,query);
      if(PQresultStatus(insert_res) != PGRES_COMMAND_OK){
        fprintf(stderr,"insert_res No data not foound\n");
        PQclear(insert_res);
        PQfinish(*conn);
        exit(-1);
      }
      PQclear(insert_res);

    }else if(strstr(first_input,"D")){

      memset(query,0,sizeof(query));
      snprintf(query,sizeof(query),"delete from tbl_article where article_idx=%d",atoi(second_input));
      PGresult* delete_res = PQexec(*conn,query);
      if(PQresultStatus(delete_res) != PGRES_COMMAND_OK){
        fprintf(stderr,"insert_res No data not foound\n");
        PQclear(delete_res);
        PQfinish(*conn);
        exit(-1);
      }
      PQclear(delete_res);

    }else if(strstr(first_input,"M")){
      system("clear");
      break;
    }
    else if(strstr(first_input,"N")){
      curr_page++;
    }else if(strstr(first_input,"P")){
      curr_page--;
    }else if(strstr(first_input,"go")){

      if(strcmp(second_input,"notice") == 0){
        memset(query,0,sizeof(query));
        snprintf(query,sizeof(query),"select menu_name from tbl_menu where menu_idx=%d",number);
        *menu_name_res = PQexec(*conn,query);
        if(PQresultStatus(*menu_name_res) != PGRES_TUPLES_OK){
          fprintf(stderr,"menu_name_res No data not found\n");
          PQclear(*menu_name_res);
          PQfinish(*conn);
          exit(-1);
        }
        system("clear");
        printf("===== %s =====\n아직 공지사항이 없습니다.\n- 아무 키나 누르세요 -\n",PQgetvalue(*menu_name_res,0,0));
        getch();
        system("clear");
        break;
      }else if(strcmp(second_input,"bbs") == 0){
        system("clear");
      }else if(strcmp(second_input,"agora") == 0){
        memset(query,0,sizeof(query));
        snprintf(query,sizeof(query),"select menu_name from tbl_menu where menu_idx=%d",number);
        *menu_name_res = PQexec(*conn,query);
        if(PQresultStatus(*menu_name_res) != PGRES_TUPLES_OK){
          fprintf(stderr,"menu_name_res No data not found\n");
          PQclear(*menu_name_res);
          PQfinish(*conn);
          exit(-1);
        }
        system("clear");
        printf("===== %s =====\n아직 토론방 기능이 없습니다.\n- 아무 키나 누르세요 -\n",PQgetvalue(*menu_name_res,0,0));
        getch();
        system("clear");
        break;
      }

    }
  }
  
  if(strstr(input,"0")) {
    *check = 0;
  }
}

int getch()
{
  int c;
  struct termios oldattr, newattr;

  tcgetattr(STDIN_FILENO, &oldattr);           
  newattr = oldattr;
  newattr.c_lflag &= ~(ICANON | ECHO);         
  newattr.c_cc[VMIN] = 1;                      
  newattr.c_cc[VTIME] = 0;                     
  tcsetattr(STDIN_FILENO, TCSANOW, &newattr);  
  c = getchar();                               
  tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);  
  return c;
}

void passwd(char* password){
  printf("패스워드 : ");

  struct termios term;
  tcgetattr(STDIN_FILENO, &term);

  term.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO,TCSANOW,&term);

  fgets(password,sizeof(password),stdin);
  password[strlen(password)-1] = '\0';

  term.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO,TCSANOW,&term);

  return;
}

void go_notice(PGresult** menu_name_res, PGconn** conn,int number){
  char query[200] = {0,};
  memset(query,0,sizeof(query));
  snprintf(query,sizeof(query),"select menu_name from tbl_menu where menu_idx=%d",number);
  printf("query: %s\n",query);
  *menu_name_res = PQexec(*conn,query);

  if(PQresultStatus(*menu_name_res) != PGRES_TUPLES_OK){
    fprintf(stderr,"menu_name_res No data not found\n");
    PQclear(*menu_name_res);
    PQfinish(*conn);
    exit(-1);
  }
  system("clear");
  printf("===== %s =====\n아직 공지사항이 없습니다.\n- 아무 키나 누르세요 -\n",PQgetvalue(*menu_name_res,0,0));
  getch();
  system("clear");

  return;
}

void go_agora(PGresult** menu_name_res, PGconn** conn,int number){
  char query[200] = {0,};
  memset(query,0,sizeof(query));
  snprintf(query,sizeof(query),"select menu_name from tbl_menu where menu_idx=%d",number);
  *menu_name_res = PQexec(*conn,query);

  if(PQresultStatus(*menu_name_res) != PGRES_TUPLES_OK){
    fprintf(stderr,"menu_name_res No data not found\n");
    PQclear(*menu_name_res);
    PQfinish(*conn);
    exit(-1);
  }
  system("clear");
  printf("===== %s =====\n아직 토론방 기능이 없습니다.\n- 아무 키나 누르세요 -\n",PQgetvalue(*menu_name_res,0,0));
  getch();
  system("clear");

  return;
}