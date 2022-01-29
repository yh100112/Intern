#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;

template<typename T>
struct Node
{
  T value;
  Node* next;
  Node* prev;
};

template <typename T>
class List{
protected:
  Node<T> *head;
public:
  List();
  virtual ~List();

  Node<T> *push(T val);
  Node<T> *push_back(T val);
  Node<T> *insert_to_front(Node<T>* Target, T insert_val);
  Node<T> *insert_to_back(Node<T>* Target, T insert_val);
  bool delete_node(T val);
  Node<T> *search(T val);
  void print_all();
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

template<typename T>
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
  printf("소멸자 호출\n");
}
