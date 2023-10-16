/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-01 14:43:44
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-14 13:45:54
 * @FilePath: /bPlusProject/src/Main.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-01 14:43:44
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-09 19:32:12
 * @FilePath: /bPlusProject/src/Main.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iostream>
#include <ostream>
#include <thread>
#include <vector>

#include "BPlusFunc.h"

void writeWorker(BPlusTree<int>* intTree, int i) {
  intTree->insert(i, 2 * i, i);
  intTree->print();
}

void deleteWorker(BPlusTree<int>* intTree, UserOperation<int>* user, int i) {
  std::vector<uint64_t> result;
  intTree->remove(result, i);
  user->printVector(result);
}

void mutiTest(BPlusTree<int>*& intTree, UserOperation<int>* op) {
  if (intTree != nullptr) {
    delete intTree;
  }
  intTree = new BPlusTree<int>();

  intTree->setDegree(op->putADegree());
  op->testInsertIntAuto(intTree);
  std::vector<std::thread> threads;

  // for (int i = 0; i < 1; i++) {
  //   threads.push_back(std::thread(deleteWorker, intTree, op, i));
  // }
  // for (auto& item : threads) {
  //   item.join();
  // }
  intTree->print();

  std::thread del(deleteWorker, intTree, op, 3);
  del.join();
  intTree->print();
}

int main() {
  BPlusTree<int>* intTree = nullptr;
  std::vector<uint64_t> vec;
  UserOperation<int>* op = nullptr;
  while (true) {
    std::cout << std::endl << "-------------------------------" << std::endl;
    std::cout << "Welcome to use B+ tree!" << std::endl;
    std::cout << "1. create a B+ tree(int)" << std::endl;
    std::cout << "2. test >create a B+ tree(int)" << std::endl;
    std::cout << "3. create a B+ tree(muti)" << std::endl;
    std::cout << "4. test >create a B+ tree(string)" << std::endl;
    std::cout << "5. show the B+ tree" << std::endl;
    std::cout << "6. delete B+ tree Node by key" << std::endl;
    std::cout << "7. exit." << std::endl;
    std::cout << "8. clear screen" << std::endl;
    std::cout << "9. searchRange , enter start , end " << std::endl;
    std::cout << "10. search , enter key" << std::endl;
    std::cout << "11. test: insert" << std::endl;
    std::cout << "12. test: delete" << std::endl;
    std::cout << "13. insert A data" << std::endl;
    std::cout << "14. serialization" << std::endl;
    std::cout << "15. deserialization" << std::endl;
    std::cout << "16. multithread" << std::endl;

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
        op->createBPlusTreeMuti(intTree);
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
        delete intTree;
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
      case 14:
        op->serializationTest(intTree);
        break;
      case 15:
        op->deserializationTest(intTree);
        break;
      case 16:
        mutiTest(intTree, op);
        // op->multiThreadTest(intTree);
        break;
      default:
        break;
    }
  }
  return 0;
}
