#include<stdio.h>
#include<stdlib.h>
#include "/home/kdy/postgresql/include/libpq-fe.h"

int main(int argc, char* argv[]){
  int i,j;
  int tuple,column;
  PGconn* conn = PQconnectdb("user=dkyong dbname=dkyongdb");

  if(PQstatus(conn) == CONNECTION_BAD){
    fprintf(stderr,"%s\n",PQerrorMessage(conn));
    PQfinish(conn);
    return -1;
  }

  PGresult* resp = PQexec(conn, "select * from address");

  if(PQresultStatus(resp) != PGRES_TUPLES_OK){
    fprintf(stderr,"No data not found\n");
    PQclear(resp);
    PQfinish(conn);
    return -1;
  }
  

  tuple = PQntuples(resp); // 테이블의 행의 갯수를 반환
  column = PQnfields(resp);   // 테이블의 열의 갯수를 반환
  for(i = 0;i < tuple; i++){
    for(j = 0; j < column; j++){
      printf("%s\t\t",PQgetvalue(resp,i,j));
    }
    printf("\n");
  }

  printf("%s\n",PQfname(resp,1)); // phone_nbr => 컬럼명을 알려줌 인덱스는 0부터 시작함
  printf("%s\n",PQfname(resp,0));
  PQclear(resp);
  PQfinish(conn);
  return 0;
}