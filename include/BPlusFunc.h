#include <cstdint>
#include <iostream>
#include <ostream>
#include <thread>
#include <vector>

#include "BPlus.h"

template <typename KeyType>
class UserOperation {
 public:
  int putADegree() {
    std::cout << "please enter a number that is greater than 2 : ";
    int num;
    std::cin >> num;
    while (num < 3) {
      std::cout << "please enter a number that is greater than 2: ";
      std::cin >> num;
      return num;
    }
    return num;
  }

  void createBPlusTreeMan(BPlusTree<KeyType> *&tree) {
    if (tree != nullptr) {
      delete tree;
      tree = nullptr;
    }
    tree = new BPlusTree<KeyType>();
    tree->setDegree(putADegree());
    KeyType key = 0;
    uint64_t value = 0;
    while (true) {
      std::cout << "please enter key and value ( exit by 0 0) :";
      std::cin >> key >> value;
      if (key == 0 && value == 0) {
        break;
      }
      tree->insert(key, value);
    }
    tree->print();
  }

  void createBPlustTreeAuto(BPlusTree<KeyType> *&tree) {
    if (tree != nullptr) {
      delete tree;
    }
    tree = new BPlusTree<int>();
    tree->setDegree(putADegree());
    testInsertIntAuto(tree);
    tree->print();
  }

  void createBPlusTreeMuti(BPlusTree<KeyType> *&tree) {
    if (tree != nullptr) {
      delete tree;
    }
    tree = new BPlusTree<int>();
    tree->setDegree(putADegree());
    testInsertIntNoOrder(tree);
    tree->print();
  }

  void printTree(BPlusTree<KeyType> *tree) {
    if (checkNotCreate(tree)) {
      return;
    }
    tree->print();
  }

  void deleteNode(BPlusTree<KeyType> *&tree, std::vector<uint64_t> vec) {
    if (checkNotCreate(tree)) {
      return;
    }
    vec.clear();
    std::cout << "please enter the key that you want to delete :";
    KeyType key = 0;
    std::cin >> key;
    tree->remove(vec, key);
    if (tree->start->next != nullptr) {
      tree->print();
    } else {
      delete tree;
    }
    std::cout << std::endl;
  }

  void searchRange(BPlusTree<KeyType> *tree) {
    if (checkNotCreate(tree)) {
      return;
    }
    KeyType start, end;
    std::cin >> start >> end;
    std::map<KeyType, std::vector<uint64_t>> res;
    tree->searchRange(res, start, end);
    auto iter = res.begin();
    std::cout << "范围查找结果：" << std::endl;
    for (auto iter = res.begin(); iter != res.end(); iter++) {
      std::cout << iter->first << ":";
      for (auto value : iter->second) {
        std::cout << value << " ";
      }
      std::cout << std::endl;
    }
  }

  void optimisedSearch(BPlusTree<int> *tree) {
    if (checkNotCreate(tree)) {
      return;
    }
    std::cout << "please enter key :";
    KeyType key;
    std::cin >> key;
    std::vector<uint64_t> res;
    tree->optimizedSearch(key, res);
    if (res.empty()) {
      std::cout << "can't find " << key << std::endl;
    } else {
      printVector(res);
    }
  }

  void printVector(std::vector<uint64_t> &res) {
    if (res.empty()) {
      return;
    }
    std::cout << std::endl;
    std::cout << "查询到的数据为:";
    for (auto item : res) {
      std::cout << item << " ";
    }
    std::cout << std::endl;
  }

  void insertTenMillionAuto(BPlusTree<KeyType> *&tree) {
    if (tree != nullptr) {
      delete tree;
    }
    tree = new BPlusTree<KeyType>();
    tree->setDegree(putADegree());
    int num = 0;
    time_t start = 0;
    time_t end = 0;
    std::cout << "please enter nums:";
    std::cin >> num;

    start = time(NULL);
    for (int i = 0; i < num; i++) {
      tree->insert(i, i * 2);
    }
    end = time(NULL);
    std::cout << std::endl;
    std::cout << "runtime :" << end - start << std::endl;
    std::cout << "done" << std::endl;
  }

  void deleteTree(BPlusTree<KeyType> *&tree) {
    if (tree != nullptr) {
      int num = 0;
      time_t start = 0;
      time_t end = 0;
      start = time(NULL);
      delete tree;
      tree = nullptr;
      end = time(NULL);
      std::cout << std::endl;
      std::cout << "runtime :" << end - start << std::endl;
      std::cout << "detleted successfully" << std::endl;
    }
  }

  void insertIntNode(BPlusTree<KeyType> *&tree) {
    KeyType key;
    uint64_t value;
    while (true) {
      std::cout << "please enter key and value ( exit by 0 0) :";
      std::cin >> key >> value;
      if (key == 0 && value == 0) {
        break;
      }
      tree->insert(key, value);
      tree->print();
    }
  }

  bool checkNotCreate(BPlusTree<KeyType> *tree) {
    if (tree == nullptr) {
      std::cout << "please retry again after creating a BPlusTree!"
                << std::endl;
      return true;
    }
    return false;
  }

  void testInsertIntAuto(BPlusTree<int> *&intTree) {
    for (int i = 1; i < 10; ++i) {
      intTree->insert(i, i * 2, i);
    }
  }

  void testInsertIntNoOrder(BPlusTree<int> *&intTree) {
    intTree->insert(5, 100);
    intTree->insert(3, 200);

    intTree->insert(7, 300);

    intTree->insert(4, 400);

    intTree->insert(8, 300);

    intTree->insert(9, 400);

    intTree->insert(6, 400);

    intTree->insert(6, 400);

    intTree->insert(7, 400);
    intTree->print();
    //   intTree->insert(5, 100);
    //  intTree->insert(7, 100);

    //   intTree->insert(3, 200);
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
    intTree->insert(5, 100);
  }

  int getRand(int min, int max) { return (rand() % (max - min + 1)) + min; }

  void serializationTest(BPlusTree<KeyType> *&tree) {
    if (tree == nullptr) {
      std::cout << "please create a b+ tree" << std::endl;
      return;
    }
    tree->serialiaztionTree();
  }

  void deserializationTest(BPlusTree<KeyType> *&tree) {
    if (tree != nullptr) {
      delete tree;
      tree = nullptr;
    }
    tree = new BPlusTree<KeyType>();
    tree->deserializationTree();
  }

  void readerTest(BPlusTree<KeyType> *&tree) {
    std::vector<uint64_t> res;
    tree->search(13, res);
    if (res.empty()) {
      std::cout << "readerA dont find ." << std::endl;
    } else {
      std::cout << "readerA" << res[0] << std::endl;
    }
  }

  void multiThreadTest(BPlusTree<KeyType> *tree) {
    // // 创建多个线程
    // // 多线程策略：latch crabbing ;
    // //
    // 适用于读多写少：可以同时读，但不可以同时写，谁先写，另外一个等待，不竞争
    // std::thread writer0([tree]() { tree->insert(9, 3, 0); });
    // // 这里不会等待，直接出结果

    // std::thread writer1([tree]() { tree->insert(13, 4, 1); });
    // //    tree->print();

    // std::thread writer2([tree]() { tree->insert(12, 5, 2); });
    // //    tree->print();

    // std::thread writer3([tree]() { tree->insert(14, 6, 3); });
    // // tree->print();

    // std::thread writer4([tree]() { tree->insert(15, 7, 4); });
    // //    tree->print();

    // std::thread writer5([tree]() { tree->insert(16, 7, 5); });
    // //   tree->print();

    // std::thread writer6([tree]() { tree->insert(17, 7); });
    // //  tree->print();

    // std::thread writer7([tree]() { tree->insert(18, 7); });
    // //  tree->print();
    // std::thread writer8([tree]() { tree->insert(19, 7); });
    // //   tree->print();
    // writer0.join();
    // writer1.join();
    // writer2.join();
    // writer3.join();
    // writer4.join();
    // writer5.join();
    // writer6.join();
    // writer7.join();
    // writer8.join();

    // // std::thread readerA([tree]() {
    // //   std::vector<uint64_t> res;
    // //   tree->search(13, res);
    // //   if (res.empty()) {
    // //     std::cout << "readerA dont find ." << std::endl;
    // //   } else {
    // //     std::cout << "readerA" << res[0] << std::endl;
    // //   }
    // // });
  }
};
