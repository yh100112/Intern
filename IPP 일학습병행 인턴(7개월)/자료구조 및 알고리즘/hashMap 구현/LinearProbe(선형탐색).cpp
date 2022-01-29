#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

#define BK 10
#define SL 1
int hashtable[BK][SL];

int hash(int key){
  return key % 10;
}

void AddKey(int key){
  int bucket;
  bucket = hash(key);
  while(hashtable[bucket][0] != 0){
    bucket = (bucket + 1) % BK; 
  }
  hashtable[bucket][0] = key;
}

bool FindKey(int key){
  int bucket;
  bucket = hash(key);
  while(hashtable[bucket][0] != 0){
    if(hashtable[bucket][0] == key) return true;
    bucket = (bucket + 1) % BK;
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

/*
		선형 탐색의 문제점
		-삭제에 무척 취약한 알고리즘이다.
			-> LinearProbe.cpp을 보면 FindKey가 빈칸을 찾을 때까지 검색을 하도록 되어 있어 삭제할 때 빈칸으로 만들면 안된다.
			-> 빈칸으로 만들어버리면 빈칸까지만 반복을 돌고 뒤에 있는 값은 무시하기 때문이다.
			-> 그렇기 때문에 값을 삭제하면 0이 아닌 특정 값(예를 들어 -1)을 넣어줘서 삭제된 칸임을 명시해야 한다.
*/