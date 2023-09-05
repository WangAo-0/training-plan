#include "Bplus.h"
template <typename Key>
void create_bplus_tree_man(BPlusTree<int> *&intTree) {
  if (intTree != nullptr) {
    delete intTree;
    intTree = nullptr;
  }
  intTree = new BPlusTree<int>();
  intTree->setDegree();
  int key = 0;
  uint64_t value = 0;
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

void create_bplus_tree_auto(BPlusTree<int> *&intTree) {
  if (intTree != nullptr) {
    delete intTree;
  }
  intTree = new BPlusTree<int>();
  intTree->setDegree();
  createTestInt1(intTree);
  intTree->print();
}

void printTree(BPlusTree<int> *intTree) {
  if (checkNotCreate(intTree)) {
    return;
  }
  intTree->print();
}

void deleteNode(BPlusTree<int> *&intTree, std::vector<uint64_t> vec) {
  if (checkNotCreate(intTree)) {
    return;
  }
  vec.clear();
  std::cout << "please enter the key that you want to delete :";
  int key = 0;
  std::cin >> key;
  intTree->remove(vec, key);
  if (intTree->start->next != nullptr) {
    intTree->print();
  } else {
    delete intTree;
  }
  std::cout << std::endl;
}

void searchRange(BPlusTree<int> *intTree) {
  if (checkNotCreate(intTree)) {
    return;
  }
  int start, end;
  std::cin >> start >> end;
  intTree->searchRange(start, end);
  std::cout << std::endl;
}
void optimisedSearch(BPlusTree<int> *intTree) {
  if (checkNotCreate(intTree)) {
    return;
  }
  std::cout << "please enter key :";
  int key2;
  std::cin >> key2;
  intTree->optimizedSearch(key2);
  std::cout << std::endl;
}
void insert_one_billion(BPlusTree<int> *&intTree) {
  if (intTree != nullptr) {
    delete intTree;
  }
  intTree = new BPlusTree<int>();
  intTree->setDegree();
  int num = 0;
  time_t start = 0;
  time_t end = 0;
  std::cout << "please enter nums:";
  std::cin >> num;

  start = time(NULL);
  for (int i = 0; i < num; i++) {
    intTree->insert(i, i * 2);
  }
  end = time(NULL);
  std::cout << std::endl;
  std::cout << "runtime :" << end - start << std::endl;
  std::cout << "done" << std::endl;
}

void deleteTree(BPlusTree<int> *&intTree) {
  if (intTree != nullptr) {
    int num = 0;
    time_t start = 0;
    time_t end = 0;
    start = time(NULL);
    delete intTree;
    intTree = nullptr;
    end = time(NULL);
    std::cout << std::endl;
    std::cout << "runtime :" << end - start << std::endl;
    std::cout << "detleted successfully" << std::endl;
  }
}

void insertNode(BPlusTree<int> *&intTree) {
  int value;
  uint64_t key;
  while (true) {
    std::cout << "please enter key and value ( exit by 0 0) :";
    std::cin >> key >> value;
    if (key == 0 && value == 0) {
      break;
    }
    intTree->insert(key, value);
    intTree->print();
  }
}

bool checkNotCreate(BPlusTree<int> *initTree) {
  if (initTree == nullptr) {
    std::cout << "please retry again after creating a BPlusTree!" << std::endl;
    return true;
  }
  return false;
}

void createTestInt2(BPlusTree<int> *&intTree) {
  for (int i = 1; i < 8; ++i) {
    intTree->insert(i, i * 2);
  }
}

void createTestInt1(BPlusTree<int> *&intTree) {
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
