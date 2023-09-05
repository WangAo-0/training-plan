#include "Bplus.h"
#include "gtest/gtest.h"

TEST(_test_insert_10000000, create_insert) {
  BPlusTree<int>* intTree = new BPlusTree<int>;
  insert_one_billion(intTree);
}

TEST(_test_delete_10000000, delete_insert) {
  BPlusTree<int>* intTree = new BPlusTree<int>;
  insert_one_billion(intTree);
  deleteTree(intTree);
}
