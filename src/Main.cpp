#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <ostream>
#include <vector>

#include "BPlusFunc.h"

int main() {
  BPlusTree<int>* intTree = nullptr;
  std::vector<uint64_t> vec;
  UserOperation<int>* op = nullptr;
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
    std::cout << "11. test: insert" << std::endl;
    std::cout << "12. test: delete" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    int funcNum = -1;
    std::cin >> funcNum;

    switch (funcNum) {
      case 1:
        op->createBPlusTreeMan(intTree);
        break;
      case 2:
        op->createBPlustTreeAuto(intTree);
        break;
      case 3:
        std::cout << "sorry, the func is not avaliable." << std::endl;
        break;
      case 4:
        std::cout << "sorry, the func is not avaliable." << std::endl;
        break;
      case 5:
        op->printTree(intTree);
        break;
      case 6:
        op->deleteNode(intTree, vec);
        break;
      case 7:
        exit(0);
        break;
      case 8:
        system("clear");
        break;
      case 9:
        op->searchRange(intTree);
        break;
      case 10:
        op->optimisedSearch(intTree);
        break;
      case 11:
        op->insertTenMillionAuto(intTree);
        break;
      case 12:
        op->deleteTree(intTree);
        break;
      case 13:
        op->insertIntNode(intTree);
        break;
      default:
        break;
    }
  }
  return 0;
}
