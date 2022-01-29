#include "MyBST.h"

int main(){
  TreeMap tree;
  tree.add(9,1);
  tree.add(15,1);
  tree.add(30,1);
  tree.add(11,1);
  tree.add(5,1);
  tree.add(13,1);
  tree.add(77,1);
  tree.add(98,1);
  tree.printAll();

  tree.remove(15);
  printf("8 삭제 후");
  tree.printAll();

  
}