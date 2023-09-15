/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-05 16:36:47
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-08 17:09:24
 * @FilePath: /bPlusProject/test/test_case1.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include "BPlusFunc.h"
#include "gtest/gtest.h"

TEST(_search_key, avg_time) {
  BPlusTree<int>* intTree = new BPlusTree<int>;
  UserOperation<int>* op = new UserOperation<int>();
  op->insertTenMillionAuto(intTree);
  srand(time(0));

  time_t start = 0;
  time_t end = 0;
  std::cout << "查询测试开始！" << std::endl;
  start = time(NULL);
  for (int i = 0; i < 10000000; i++) {
    int r = op->getRand(1, 10000000);
    std::vector<uint64_t> res;
    intTree->optimizedSearch(r, res);
    EXPECT_EQ(res[0], r * 2);
  }
  end = time(NULL);
  std::cout << "查询耗时 :" << end - start << std::endl;
  op->deleteTree(intTree);
}

TEST(DISABLED__test_insert_10000000, create_insert) {
  BPlusTree<int>* intTree = new BPlusTree<int>;
  UserOperation<int>* op = new UserOperation<int>();
  op->insertTenMillionAuto(intTree);
}

TEST(DISABLED__test_delete_10000000, delete_insert) {
  BPlusTree<int>* intTree = new BPlusTree<int>;
  UserOperation<int>* op = new UserOperation<int>();
  op->insertTenMillionAuto(intTree);
  op->deleteTree(intTree);
}

// 左闭右闭区间
