/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-22 11:24:46
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-22 11:35:49
 * @FilePath: /bPlusProject/test/test_case3_multi.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include <random>
#include <vector>

#include "BPlusFunc.h"
#include "gtest/gtest.h"

// BPlusTree<int>* intTree;

class BPlusTreeMultiTest : public testing::Test {
 protected:
  BPlusTree<int>* intTree;
  UserOperation<int>* user;
  void SetUp() override {
    intTree->setDegree(3);
    user->testInsertIntNoOrder(intTree);
  }
};

void writeWorker(BPlusTree<int>* intTree, int i) {
  intTree->insert(i, 2 * i, i);
}

int randomNum() {
  std::random_device rd;
  std::mt19937 generator(rd());

  // 创建随机数分布，指定范围为0到10
  std::uniform_int_distribution<int> distribution(0, 10);

  // 生成随机数
  return distribution(generator);
}

void deleteWorker(BPlusTree<int>* intTree, UserOperation<int>* user, int i) {
  std::vector<uint64_t> result;
  intTree->remove(result, i);
  user->printVector(result);
}

TEST_F(BPlusTreeMultiTest, TenThreadsInsert) {
  intTree->print();
  std::vector<std::thread> threads;

  for (int i = 0; i < 4; i++) {
    threads.push_back(std::thread(deleteWorker, intTree, user, randomNum()));
    // threads.push_back(std::thread(writeWorker, intTree, randomNum()));
  }
  for (auto& item : threads) {
    item.join();
  }

  intTree->print();
}