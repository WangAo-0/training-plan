#include <cstdint>
#include <iostream>
#include <ostream>

#include "Bplus.h"

void create_bplus_tree_man(BPlusTree<int> *&intTree) {
  if (intTree != nullptr) {
    delete intTree;
  }
  intTree = new BPlusTree<int>();
  intTree->setDegree();
  int value;
  uint64_t key;
  while (true) {
    std::cout << "please enter key and value ( exit by 0 0) :";
    std::cin >> key >> value;
    if (key == 0 && value == 0) {
      break;
    }
    intTree->insert(key, value);
  }
  intTree->print();
}

int main() {
  BPlusTree<int> *intTree = nullptr;
  std::vector<uint64_t> vec;
  while (true) {
    std::cout << "-------------------------------" << std::endl;
    std::cout << "Welcome to use B+ tree!" << std::endl;
    std::cout << "1. create a B+ tree(int)" << std::endl;
    std::cout << "2. test >create a B+ tree(int)" << std::endl;
    std::cout << "3. create a B+ tree(string)" << std::endl;
    std::cout << "4. test >create a B+ tree(string)" << std::endl;
    std::cout << "5. show the B+ tree" << std::endl;
    std::cout << "6. delete B+ tree Node by key" << std::endl;
    std::cout << "7. exit." << std::endl;
    std::cout << "8. clear screen" << std::endl;
    std::cout << "9. searchRange , enter start , end " << std::endl;
    std::cout << "10. search , enter key" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    int funcNum = -1;
    std::cin >> funcNum;

    switch (funcNum) {
      case 1:
        create_bplus_tree_man(intTree);
        break;
      case 2:
        if (intTree != nullptr) {
          delete intTree;
        }
        intTree = new BPlusTree<int>();
        intTree->setDegree();
        // createTestInt2(intTree);
        createTestInt1(intTree);
        intTree->print();
        break;
      case 3:
        std::cout << "sorry, the func is not avaliable." << std::endl;
        break;
      case 4:
        std::cout << "sorry, the func is not avaliable." << std::endl;
        break;
      case 5:
        if (checkNotCreate(intTree)) {
          break;
        }
        intTree->print();
        break;
      case 6:
        if (checkNotCreate(intTree)) {
          break;
        }
        vec.clear();
        std::cout << "please enter the key that you want to delete :";
        int key6;
        std::cin >> key6;
        intTree->remove(vec, key6);
        if (!(intTree->start->next->keys.empty())) {
          intTree->print();
        }
        std::cout << std::endl;
        break;
      case 7:
        exit(0);
        break;
      case 8:
        system("clear");
        break;
      case 9:
        if (checkNotCreate(intTree)) {
          break;
        }
        int start, end;
        std::cin >> start >> end;
        intTree->searchRange(start, end);
        std::cout << std::endl;
        break;
      case 10:
        if (checkNotCreate(intTree)) {
          break;
        }
        std::cout << "please enter key :";
        int key2;
        std::cin >> key2;
        intTree->optimizedSearch(key2);
        std::cout << std::endl;
        break;
      default:
        break;
    }
  }
  return 0;
}

bool checkNotCreate(BPlusTree<int> *root) {
  if (root == nullptr) {
    std::cout << "please retry again after creating a BPlusTree!" << std::endl;
    return true;
  }
  return false;
}

void createTestInt2(BPlusTree<int> *intTree) {
  for (int i = 1; i < 8; ++i) {
    intTree->insert(i, i * 2);
  }
}

void createTestInt1(BPlusTree<int> *intTree) {
  intTree->insert(5, 100);
  intTree->insert(3, 200);
  intTree->insert(7, 300);
  intTree->insert(4, 400);
  intTree->insert(8, 300);
  intTree->insert(9, 400);
  intTree->insert(6, 400);
  intTree->insert(6, 400);
  intTree->insert(7, 400);
  intTree->insert(5, 100);
  intTree->insert(3, 200);
  intTree->insert(7, 300);
  intTree->insert(4, 400);
  intTree->insert(8, 300);
  intTree->insert(9, 400);
  intTree->insert(10, 400);
  intTree->insert(6, 400);
  intTree->insert(6, 400);
  intTree->insert(7, 400);
  intTree->insert(5, 100);
  intTree->insert(5, 100);
  intTree->insert(3, 200);
  intTree->insert(7, 300);
  intTree->insert(4, 400);
  intTree->insert(8, 300);
  intTree->insert(9, 400);
  intTree->insert(6, 400);
  intTree->insert(6, 400);
  intTree->insert(7, 400);
  intTree->insert(5, 100);
  intTree->insert(3, 200);
  intTree->insert(7, 300);
  intTree->insert(4, 400);
  intTree->insert(8, 300);
  intTree->insert(9, 400);
  intTree->insert(10, 400);
  intTree->insert(6, 400);
  intTree->insert(6, 400);
  intTree->insert(7, 400);
  intTree->insert(5, 100);
}
