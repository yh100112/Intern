#include<stdio.h>
#include<stdlib.h>

struct Node
{
  int value;
  Node* prev;
  Node* next;
};

struct Node* head;

//생성자 : head노드 초기화
void InitList()
{
  head = (struct Node*)malloc(sizeof(Node));
  head->prev = NULL;
  head->next = NULL;
}

//특정 노드 뒤에 값 추가
Node* InsertNodeRight(Node* Target, Node* addNode){
  Node* New;
  Node* Right; // 타겟의 기존의 오른쪽에 있는 노드

  New = (Node*)malloc(sizeof(Node));
  *New = *addNode;

  Right = Target->next; // 타겟의 오른쪽 노드의 주소
  New->next = Right; // 새로운 노드의 next = 타겟의 기존의 오른쪽에 있는 노드
  New->prev = Target; // 새로운 노드의 prev = 타겟
  Target->next = New; // 타겟의 next = 새로운 노드

  if( Right != NULL){
    printf("11\n");
    Right->prev = New; // 타겟의 기존의 오른쪽에 있던 노드의 prev = 새로운 노드
  }

  return New;
}

//특정 노드 앞에 값 추가
struct Node* InsertNodeLeft(Node* Target, Node* addNode){
  Node* Left;

  Left = Target->prev;
  if(Left){
    return InsertNodeRight(Left,addNode);
  }  
}

//제일 끝에 노드 추가
void AppendNode(Node* addNode){
  Node* tail;

  for(tail = head; tail->next; tail = tail->next){;}
  InsertNodeRight(tail,addNode);
}

//맨 앞에 노드 추가
void push(Node* addNode){
  InsertNodeRight(head,addNode);
}


//특정 노드 삭제
bool deleteNode(Node* Target){
  Node* Left;
  Node* Right;

  if(Target == NULL || Target == head){
    return false;
  }

  Right = Target->next;
  Left = Target->prev;
  Left->next = Right;
  if(Right){
    Right->prev = Left;
  }
  free(Target);

  return true;
}

//idx번째 노드를 찾는다.
Node* FindNodeByIndex(int idx){
  Node* Now;
  int index = 0;

  for(Now = head->next;Now;Now = Now->next){
    if(index == idx){
      return Now;
    }
    index++;
  }
  return NULL;
}

//노드의 개수를 조사한다.
int GetListCount(){
  Node* Now;
  int Count = 0;

  for(Now = head->next; Now; Now = Now->next){
    Count++;
  }
  return Count;
}

//모든 노드 삭제
void UnInitList(){
  while(deleteNode(head->next)){}
  free(head);
  head = NULL;
}


