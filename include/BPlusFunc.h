#include <cstdint>

#include "BPlus.h"
template <typename KeyType>
class UserOperation {
 public:
  void createBPlusTreeMan(BPlusTree<KeyType> *&tree) {
    if (tree != nullptr) {
      delete tree;
      tree = nullptr;
    }
    tree = new BPlusTree<KeyType>();
    tree->setDegree();
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
    tree->setDegree();
    testInsertIntAuto(tree);
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
    tree->searchRange(start, end);
    std::cout << std::endl;
  }

  void optimisedSearch(BPlusTree<int> *tree) {
    if (checkNotCreate(tree)) {
      return;
    }
    std::cout << "please enter key :";
    KeyType key;
    std::cin >> key;
    tree->optimizedSearch(key);
    std::cout << std::endl;
  }

  void insertTenMillionAuto(BPlusTree<KeyType> *&tree) {
    if (tree != nullptr) {
      delete tree;
    }
    tree = new BPlusTree<KeyType>();
    tree->setDegree();
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
    for (int i = 1; i < 8; ++i) {
      intTree->insert(i, i * 2);
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
    intTree->insert(5, 100);
  }

  int getRand(int min, int max) { return (rand() % (max - min + 1)) + min; }
};
