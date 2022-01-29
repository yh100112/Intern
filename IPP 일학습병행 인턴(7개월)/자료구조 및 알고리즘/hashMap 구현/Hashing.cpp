#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#define BK 10 //bucket : 주소록에서 ㄱ,ㄴ,ㄷ,ㄹ 각 페이지가 버킷이다.
#define SL 1 // slot : 버킷에 데이터가 저장되는 단위

int hashtable[BK][SL];

int hash(int key){
  return key % 10; // 10으로 나눈 나머지, 즉 1의 자릿수를 해시값으로 하겠다!
}

void AddKey(int key){
  int bucket;

  bucket = hash(key); // 입력된 키로부터 해시값(버킷)을 찾음
  if(hashtable[bucket][0] == 0){ // 이 버킷이 비어있을 때 값을 추가
    hashtable[bucket][0] = key;
  }
}

bool FindKey(int key){
  int bucket;

  bucket = hash(key); // 해시값(버킷)을 찾음
  return (hashtable[bucket][0] == key); // 버킷에 들어있는 값과 키값을 비교해 찾으려는 키값이 있는지 여부 반환
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