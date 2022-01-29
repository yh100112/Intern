#include<stdio.h>
#include<stdlib.h>
#include<memory.h>

struct HashSlot{
  int key;
  int value;
  HashSlot* next;
};

class HashMap{
private:
  HashSlot* buckets; // 생성자에서 buckets = new HashSlot[bucketSize];로 생성
  int bucketSize;    // 생성자에서 받은 bucketSize 대입
public:
  HashMap(int bucket = 7);
  virtual ~HashMap();

  HashSlot* add(int key, int value);
  void remove(int key);
  HashSlot* find(int key);

  inline int hashFunc(int key, int bucketSize){
    return key % bucketSize;
  }
  inline bool compareFunc(int key1,int key2){
    return (key1 == key2);
  }
};

HashMap::HashMap(int bucket){
  bucketSize = bucket;
  buckets = new HashSlot[bucketSize]; // 버킷 생성
  memset(buckets,0,sizeof(HashSlot) * bucketSize);
  // printf("bucket 생성\n");
};

HashMap::~HashMap(){
  HashSlot* curr = buckets[0].next;
  HashSlot* Right;
  int cnt = 1;

  while(cnt < bucketSize){
    while(curr != NULL){
      Right = curr->next;
      delete curr;
      curr = Right;
    }
    curr = buckets[cnt].next;
    ++cnt;
  }
  delete[] buckets;
  // printf("bucket 삭제 완료\n");
};

HashSlot* HashMap::add(int key, int value){
  HashSlot* curr = find(key);
  if(curr != NULL){
    printf("동일한 key값이 이미 존재합니다.\n");
    return curr; 
  } // key가 이미 존재하면 추가를 하지 않고 현재 key가 있는 주소를 반환

  int hash = hashFunc(key,bucketSize);
  HashSlot* newslot = new HashSlot;
  memset(newslot,0,sizeof(HashSlot));

  newslot->key = key;
  newslot->value = value;
  newslot->next = buckets[hash].next; // bucket의 맨 앞에 추가
  buckets[hash].next = newslot; // bucket의 next를 새로 추가된 hashslot으로 만듬 (즉, 새로 추가된 것은 맨 앞에 붙으니까 그걸 가리키게 만듬)
  
  return newslot;
};

HashSlot* HashMap::find(int key){
  int hash = hashFunc(key,bucketSize);
  HashSlot* curr = buckets[hash].next; // hash값에 해당하는 bucket에 저장되어 있는 단순 연결 리스트의 시작 노드 주소이다.

  while((curr != NULL) && (compareFunc(key,curr->key) == false)){ // key를 찾았으면 탈출
    curr = curr->next;
  }

  return curr;
};

void HashMap::remove(int key){
  int hash = hashFunc(key,bucketSize);
  HashSlot* curr = buckets[hash].next;// hash값에 해당하는 bucket에 저장되어 있는 단순 연결 리스트의 시작 노드 주소이다. bucket이 리스트의 head노드임!

  HashSlot* head = buckets + hash; 
  while(curr && !compareFunc(key,curr->key)){ //다음 노드가 null이 아니고, key를 찾지 못했으면 반복
    head = curr;
    curr = curr->next;
  }
  //삭제할 key가 없다면 curr =>null, 삭제할 key가 있다면 curr => key가 있는 노드의 주소

  if(curr != NULL){
    head->next = curr->next; // 단순 연결 리스트이므로 삭제할 노드가 가리키는 곳을 삭제할 노드의 이전 노드에 연결하면 됨
    delete curr;
    printf("key : %d  => 삭제 완료\n",key);
  }else if(curr == NULL){
    printf("삭제하려는 키가 존재하지 않습니다.\n");
  }
};




