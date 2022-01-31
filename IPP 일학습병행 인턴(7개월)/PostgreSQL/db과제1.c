#include<stdio.h>
#include<stdlib.h>
#include "/home/kdy/postgresql/include/libpq-fe.h"

void show(PGresult* resp);
void check(PGresult* resp,PGconn* conn);

int main(){
  PGconn* conn = PQconnectdb("user=dkyong dbname=dkyongdb");

  if(PQstatus(conn) == CONNECTION_BAD){
    fprintf(stderr,"%s\n",PQerrorMessage(conn));
    PQfinish(conn);
    exit(-1);
  }
  PGresult* resp = PQexec(conn,"select * from tbl_member");
  PGresult* resp2 = PQexec(conn,"select * from tbl_article");
  check(resp,conn);
  show(resp);
  
  check(resp2,conn);
  show(resp2);

  PQclear(resp);
  PQclear(resp2);
  PQfinish(conn);
}

void check(PGresult* resp, PGconn* conn){
  if(PQresultStatus(resp) != PGRES_TUPLES_OK){
    fprintf(stderr,"No data\n");
    PQclear(resp);
    PQfinish(conn);
    exit(-1);
  }
  return;
}

void show(PGresult* resp){
  int tuple,column,i,j;
  tuple = PQntuples(resp); // 튜플 ( 행 ) 갯수
  column = PQnfields(resp);

  for(int k=0;k < column;k++){
    printf("%s\t",PQfname(resp,k)); //필드명(속성명)들 출력
  }
  printf("\n");

  //테이블 값들 출력
  for(i = 0;i < tuple; i++){
    for(j = 0; j < column; j++){
      printf("%s\t",PQgetvalue(resp,i,j));
    }
    printf("\n");
  }    

  printf("\n");
  return;
}