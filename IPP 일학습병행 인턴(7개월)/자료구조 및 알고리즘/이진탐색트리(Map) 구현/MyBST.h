#include<memory.h>
#include<stdio.h>
#include<stdlib.h>

struct TreeSlot
{
  int key;
  int value;
  TreeSlot* left;
  TreeSlot* right;
};

class TreeMap{
private:
  void printAllInner(TreeSlot* slot);
  void deleteAllInner(TreeSlot* slot);
  TreeSlot base;
public:
  TreeMap();
  virtual ~TreeMap();

  TreeSlot* add(int key, int val);
  void remove(int key);
  TreeSlot* find(int key);
  
  inline void deleteAll(){
    deleteAllInner(base.left);
  }

  inline void printAll(){
    printf("tree : ");
    printAllInner(base.left);
    printf("\n");
  }

};

void TreeMap::deleteAllInner(TreeSlot* slot){
  if(slot == NULL) return;
  TreeSlot* left = slot->left;
  TreeSlot* right = slot->right;
  
  printf("delete %d\n",slot->key);
  delete slot;
  deleteAllInner(left);
  deleteAllInner(right);
}

void TreeMap::printAllInner(TreeSlot* slot){
  if(slot == NULL) return;

  printf("%d ",slot->key);
  printAllInner(slot->left);
  printAllInner(slot->right);
}

TreeMap::TreeMap(){
  base = {0,};
  // printf("base 생성\n");
}

TreeMap::~TreeMap(){
  deleteAll();
}



TreeSlot* TreeMap::add(int key, int val){
  TreeSlot* curr = base.left;
  TreeSlot* parent = base.left;

  while(curr != NULL){
    if(key == curr->key) return curr; // 이미 동일한 key가 존재하면 그 key가 있는 노드의 위치를 반환하고 종료
    parent = curr;
    if(key < curr->key){
      curr = curr->left;
    }else if(key > curr->key){
      curr = curr->right;
    }
  }
  TreeSlot* New = new TreeSlot; 
  memset(New,0,sizeof(TreeSlot)); 

  if(parent == NULL){
    New->key = key;
    New->value = val;
    base.left = New;
    return New;
  }

  if(key < parent->key){
    New->key = key;
    New->value = val;
    parent->left = New;
  }
  else if(key > parent->key){
    New->key = key;
    New->value = val;
    parent->right = New;
  }
  return New;
}

void TreeMap::remove(int key){
  TreeSlot* curr =  base.left;
  TreeSlot* parent = base.left;

  while(curr != NULL){ // 삭제할 key 찾음. parent에는 삭제할 key의 부모노드 저장
    if(key ==  curr->key) break;
    parent = curr;
    if(key < curr->key){
      curr = curr->left;
    }else if(key > curr->key){
      curr = curr->right;
    }
  }
  if(curr == base.left){ // 루트 노드(base.left)를 삭제할 때
    if(curr->left == NULL && curr->right == NULL){delete curr;} // 삭제할 노드의 자식이 0개일때
    else if(curr->left == NULL || curr->right == NULL){// 자식이 1개일 때
      if(curr->left == NULL){
        base.left = curr->right;
      }else if(curr->left != NULL){
        base.left = curr->left;
      }
      delete curr;
    } 
    else if(curr->left != NULL && curr->right != NULL){ // 자식이 2개일 때
      if(curr->right->left == NULL){
        base.left = curr->right;
        base.left->left = curr->left;
        delete curr;
      }else if(curr->right->left != NULL){
        TreeSlot* minimal = curr->right;
        TreeSlot* minimalParent = curr;
        while(minimal->left){
          minimalParent = minimal;
          minimal = minimal->left;
        }
        curr->key = minimal->key;
        curr->value = minimal->value;
        minimalParent->left = NULL;
      }
    }
    return;
  }

  TreeSlot* isLeftChild = parent->left;
  if(curr->left == NULL && curr->right == NULL){ // 삭제할 노드의 자식이 0개일때
    if(curr == isLeftChild){ // 삭제할 노드가 부모의 왼쪽 자식이면
      parent->left = NULL;
    }else if(curr != isLeftChild){
      parent->right = NULL;
    }
    delete curr;

  }else if(curr->left == NULL || curr->right == NULL){ // 삭제할 노드의 자식이 1개일 때
    if(curr == isLeftChild){ // 삭제할 노드가 부모의 왼쪽 자식이면
    
      if(curr->left == NULL){ // 삭제할 노드의 자식이 오른쪽 자식이면
        parent->left = curr->right;
      }else if(curr -> left != NULL){ // 삭제할 노드의 자식이 왼쪽 자식이면
        parent->left = curr->left;
      }
    
    }else if(curr != isLeftChild){ // 삭제할 노드가 부모의 오른쪽 자식이면

      if(curr->left == NULL){ // 삭제할 노드의 자식이 오른쪽 자식이면
        parent->right = curr->right;
      }else if(curr -> left != NULL){ // 삭제할 노드의 자식이 왼쪽 자식이면
        parent->right = curr->left;
      }
    
    }
    delete curr;
  }else if(curr->left != NULL && curr->right != NULL){ // 삭제할 노드의 자식이 2개일 때
    if(curr->right->left == NULL){ // 삭제할 노드의 오른쪽 자식의 왼쪽 자식이 NULL이면 ( 오른쪽 자식이 제일 작은 값임 )
      if(curr == isLeftChild){ // 삭제할 노드가 부모의 왼쪽 자식이면
        parent->left = curr->right; // 부모의 왼쪽에 삭제할 노드의 오른쪽 자식 연결
        parent->left->left = curr->left; // 부모의 왼쪽에 새롭게 연결된 것의 왼쪽에 삭제할 노드의 왼쪽 자식 연결 
      }else if(curr != isLeftChild){ // 삭제할 노드가 부모의 오른쪽 자식이면
        parent->right = curr->right;
        parent->right->left = curr->left;
      }
      delete curr;
    
    }else if(curr->right->left != NULL){ // 삭제할 노드의 오른쪽 자식의 왼쪽 자식이 존재할 때 (최소 자식을 찾아야 함)
      TreeSlot* minimal = curr->right;
      TreeSlot* minimalParent = curr;
      while(minimal->left){ // 삭제할 노드의 오른쪽 자식의 왼쪽 자식이 없을 때까지 반복
        minimalParent = minimal;
        minimal = minimal->left;
      }
      //삭제할 노드의 오른쪽 부분의 최소값이 minimal에 저장됨. 그 최소값을 삭제할 노드 curr로 복사하고 minimal을 없앰
      curr->key = minimal->key;
      curr->value = minimal->value;
      minimalParent->left = NULL;
      delete minimal;
    }
  }

}

TreeSlot* TreeMap::find(int key){
  TreeSlot* curr = base.left;

  while(curr != NULL){
    if(key < curr->key){
      curr = curr->left;
    }else if(key > curr->key){
      curr = curr->right;
    }else if(key == curr->key){
      break;
    }
  }

  return curr;
}
