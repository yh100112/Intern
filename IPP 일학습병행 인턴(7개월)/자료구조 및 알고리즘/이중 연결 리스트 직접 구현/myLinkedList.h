#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;

template <typename T>
struct Node
{
  T value;
  Node *next;
  Node *prev;
};

template <typename T>
class List
{
protected:
  Node<T> *head; 
public:
  List();
  virtual ~List();

  Node<T> *push(T val); // 맨 앞에 값 추가
  Node<T> *push_back(T val); // 맨 뒤에 값 추가
  Node<T> *insert_to_front(Node<T>* Target, T insert_val); // 특정노드 앞에 값 추가
  Node<T> *insert_to_back(Node<T>* Target, T insert_val); // 특정노드 뒤에 값 추가
  bool delete_node(T val); // 특정 값을 가진 노드 삭제
  Node<T> *search(T val); // 특정 값을 가진 노드 선형검색
  void print_all(); // 전체 노드의 값 순회하면서 출력
};

template <typename T>
void List<T>::print_all(){
  Node<T>* Now;
  int Count = 0;

  for(Now = head->next; Now; Now = Now->next){
    cout << Now->value << "\t";
    Count++;
  }
  printf("\n");
  printf("노드 갯수 : %d\n",Count);
}

template <typename T>
List<T>::List(){
  head = new Node<T>;
  head->prev = NULL;
  head->next = NULL;
  printf("head노드 생성\n");
}

template <typename T>
List<T>::~List(){
  Node<T>* Now;
  Node<T>* Right;

  Now = head;
  while(true){
    cout << Now->value <<" 삭제" << endl;
    Right = Now->next;
    delete Now;
    Now = Right;
    if(Now == NULL){
      break;
    }
  }
}


template<typename T>
Node<T>* List<T>::push(T val){
  return insert_to_back(head,val);
}

template<typename T>
Node<T>* List<T>::push_back(T val){
  Node<T>* tail;
  for(tail = head;tail->next;tail = tail->next){}
  return insert_to_back(tail,val);
}

template<typename T>
Node<T>* List<T>::insert_to_front(Node<T>* Target, T insert_val){
  Node<T>* Left;
  Left = Target->prev;
  return insert_to_back(Left,insert_val);
}

template<typename T>
Node<T>* List<T>::insert_to_back(Node<T>* Target, T insert_val){
  Node<T>* New;
  Node<T>* Right;
  New = new Node<T>;
  New->value = insert_val;

  Right = Target->next;
  New->prev = Target;
  Target->next = New;
  New->next = Right;

  if(Right != NULL){
    Right->prev = New;
  }

  return New;
}

template<typename T>
bool List<T>::delete_node(T val){
  Node<T>* Now;
  Node<T>* Left;
  Node<T>* Right;
  Now = new Node<T>;

  for(Now = head->next;Now;Now = Now->next){
    if(Now->value == val){
      break;
    }
  }

  if(Now == NULL || Now == head){
    return false;
  }

  Left = Now->prev;
  Right = Now->next;

  Left->next = Right;
  if(Right != NULL){
    Right->prev = Left;
  }
  
  delete Now;
  return true;
}

template <typename T>
Node<T>* List<T>::search(T val){
  Node<T>* Now;
  for(Now = head->next;Now;Now = Now->next){
    if(Now->value == val){
      break;
    }
  }

  cout << "찾은 값 : " << Now->value << endl;

  return Now;

};