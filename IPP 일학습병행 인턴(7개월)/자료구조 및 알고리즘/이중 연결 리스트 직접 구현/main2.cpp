#include "myLinkedList2.h"
#include<string>

int main(){
  class List<int> a;
  a.push(2);
  a.push(3);
  a.push(4);
  a.push(5);

  List<int>::iterator itor; // lvalue
  itor = a.begin(); // a.begin()은 rvalue를 반환함
  ++itor;
  cout << itor->value << endl;
  // cout << itor.operator->()->value << endl;

  
  for(itor = a.begin(); itor != a.end(); itor++){ 
    // cout << itor.operator*() << " ";
    cout << *itor << " "; // * 자체가 뒤에다 놓고 연산하는 기능이 없음. 그리고 곱하기와 구별하는 법은 뒤에 매개변수의 유무에 따라 나뉨
  }
  printf("\n");
  if(itor == nullptr){
    printf("true\n");
  }

  // if(itor.operator==(nullptr)){
  //   printf("ttrrrrr\n");
  // }
  
}