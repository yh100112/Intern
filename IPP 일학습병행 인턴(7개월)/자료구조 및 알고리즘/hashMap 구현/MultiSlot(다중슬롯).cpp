#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#define BK 10
#define SL 3
int hashtable[BK][SL];

int hash(int key){
  return key % 10;  
}

void AddKey(int key){
  int i,bucket;
  bucket = hash(key);
  for(i=0;i<SL;i++){
    if(hashtable[bucket][i] == 0){
      hashtable[bucket][i] = key;
      break;
    }
  }
}

bool FindKey(int key){
  int i,bucket;

  bucket = hash(key);
  for(i=0;i<SL;i++){
    if(hashtable[bucket][i] == key){
      return true;
    }
  }
  return false;
}

int main(){
  int i,key;

  memset(hashtable,0,sizeof(hashtable));
  for(i=0;i<5;i++){
    printf("%d번째 값을 입력하세요 : ",i+1);
    scanf("%d",&key);
    AddKey(key);    
  }
  printf("검색할 키를 입력하세요 : ");
  scanf("%d",&key);
  if(FindKey(key)){
    puts("검색되었습니다.");
  }else{
    puts("입력하신 값이 없습니다.");
  }  
}