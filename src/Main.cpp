#include "Bplus.h"
#include <iostream>
#include <ostream>

int main() {

  BPlusTree<int> *intTree = new BPlusTree<int>();
  std::vector<uint64_t> vec;
  std::vector<uint64_t> searchVec;
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
    int funcNum;
    std::cin >> funcNum;

    switch (funcNum) {
    case 1:
      std::cout << "sorry, the func is not avaliable." << std::endl;
      break;
    case 2:
      if (intTree->start != nullptr) {
        // 先删除该树
        delete (intTree);
      }
      intTree = new BPlusTree<int>();
      intTree->setDegree();
      createTestInt(intTree);
      // createTest2(intTree);
      intTree->print();
      break;
    case 3:
      std::cout << "sorry, the func is not avaliable." << std::endl;
      break;
    case 4:
      std::cout << "sorry, the func is not avaliable." << std::endl;
      break;
    case 5:
      intTree->print();
      break;
    case 6:
      if (checkNotCreate(intTree)) {
        break;
      }
      vec.clear();
      std::cout << "please enter the key that you want to delete" << std::endl;
      int key;
      std::cin >> key;
      intTree->remove(vec, key);
      intTree->print();
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
      searchVec.clear();
      searchVec = intTree->searchRange(start, end);
      for (auto item : searchVec) {
        std::cout << item << " ";
      }
      std::cout << std::endl;
      break;
    case 10:
      if (checkNotCreate(intTree)) {
        break;
      }
      int key2;
      std::cin >> key2;
      intTree->optimizedSearch(key2);
      break;
    default:
      break;
    }
  }
  return 0;
}

bool checkNotCreate(BPlusTree<int> *root) {
  if (root->start == nullptr) {
    std::cout << "please retry again after creating a BPlusTree!" << std::endl;
    return true;
  }
  return false;
}
void createTestInt2(BPlusTree<int> *intTree) {
  for (int i = 0; i < 100; ++i) {
    intTree->insert(i, i * 2);
  }
}

void createTestInt(BPlusTree<int> *intTree) {
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
  // intTree->insert(7, 300);
  // intTree->insert(4, 400);
  // intTree->insert(8, 300);
  // intTree->insert(9, 400);
  // intTree->insert(10, 400);
  // intTree->insert(6, 400);
  // intTree->insert(6, 400);
  // intTree->insert(7, 400);
  // intTree->insert(5, 100);
  // intTree->insert(3, 200);
  // intTree->insert(7, 300);
  // intTree->insert(4, 400);
  // intTree->insert(8, 300);
  // intTree->insert(9, 400);
  // intTree->insert(6, 400);
  // intTree->insert(6, 400);
  // intTree->insert(7, 400);
  // intTree->insert(5, 100);
  // intTree->insert(3, 200);
  // intTree->insert(7, 300);
  // intTree->insert(4, 400);
  // intTree->insert(8, 300);
  // intTree->insert(9, 400);
  // intTree->insert(10, 400);
  // intTree->insert(6, 400);
  // intTree->insert(6, 400);
  // intTree->insert(7, 400);
  // intTree->insert(5, 100);
  // intTree->insert(3, 200);
  // intTree->insert(7, 300);
  // intTree->insert(4, 400);
  // intTree->insert(8, 300);
  // intTree->insert(9, 400);
  // intTree->insert(6, 400);
  // intTree->insert(6, 400);
  // intTree->insert(7, 400);
  // intTree->insert(5, 100);
  // intTree->insert(3, 200);
  // intTree->insert(7, 300);
  // intTree->insert(4, 400);
  // intTree->insert(8, 300);
  // intTree->insert(9, 400);
}
