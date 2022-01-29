#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#define BK 10
#define SL 1
int hashtable[BK][SL];

int hash(int key){
  return key % BK;
}

int hash2(int key){
  return (key/10 + key%10) % 10;
}

void AddKey(int key){
  int bucket;
  bucket = hash(key);
  if(hashtable[bucket][0] != 0){
    bucket = hash2(key);
  }
  if(hashtable[bucket][0] == 0){
    hashtable[bucket][0] = key;
  }
}

bool FindKey(int key){
  int bucket;
  bucket = hash(key);
  if(hashtable[bucket][0] == key){
    return true;
  }
  bucket = hash2(key);
  if(hashtable[bucket][0] == key){
    return true;
  }
  return false;
}

int main(){
  int i,key;

  memset(hashtable,0,sizeof(hashtable)); // 해시 테이블 0으로 초기화
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