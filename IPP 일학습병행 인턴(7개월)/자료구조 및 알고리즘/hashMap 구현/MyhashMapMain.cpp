#include "MyhashMap.h"
using namespace std;

void hashTest(HashMap& hash, int key){
  printf("------------------------\n");
  HashSlot* curr = hash.find(15);
  if(curr == NULL){
    printf("찾으려는 key값이 없습니다.\n");
  }else{
    printf("key : %d / value : %d\n",curr->key,curr->value);
  }
  printf("------------------------\n");
}

int main(){
  HashMap hash;
  hash.add(1,10);
  hash.add(2,20);
  hash.add(3,30);
  hash.add(8,45);
  hash.add(15,999);

  hashTest(hash,15);
  hash.remove(15);
  hashTest(hash,15);
}