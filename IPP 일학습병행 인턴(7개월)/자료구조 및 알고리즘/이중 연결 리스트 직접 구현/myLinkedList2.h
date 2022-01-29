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
class Iter
{
private:
  Node<T>* current;
public:
  Iter(Node<T>* node = nullptr):current(node){}

  Iter<T>& operator ++ (){ // 전위 증가 => ++itor lvalue
    current = current->next;
    return *this;
  }
  Iter<T> operator++(int){ // 후위 증가 => itor++ rvalue
    Iter<T> original = Iter<T>(*this);
    current = current->next;
    return original;
  }
  T& operator * (){
    return current->value;
  }
  Node<T>* operator->(){
    return current;
  }
  bool operator == (const Iter<T>& ref){
    return current == ref.current;
  }
  bool operator != (const Iter<T>& ref){
    return current != ref.current;
  }

  // Iter<T>& operator = (Iter<T>& ref){  // (x) // & 레퍼런스는 lvalue인데 begin()의 반환값이 rvalue이므로 받을 수 없음 에러남
  //   current = ref.current;
  //   return *this;
  // }
  Iter<T>& operator = (const Iter<T>& ref){  // (o) // const 참조는 rvalue를 받을 수 있음 
    current = ref.current;
    return *this;
  }
  // Iter<T>& operator = (Iter<T> ref){  // (o) // rvalue를 매개변수로 받아옴
  //   current = ref.current;
  //   return *this;
  // }
};
template <typename T>
class List{
protected:
  Node<T> *head; 
public:
  List();
  virtual ~List();

  typedef Iter<T> iterator; // main에서 접근할 때 List<T>::iterator itor;이렇게 접근함

  Iter<T> begin(){
    return Iter<T>(head);  //임시 객체 생성 => rvalue임
  }
  Iter<T> end(){
    return Iter<T>(nullptr);
  }

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
  printf("소멸자 호출\n");
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