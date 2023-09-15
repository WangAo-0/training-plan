/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-08 10:17:04
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-08 19:13:52
 * @FilePath: /bPlusProject/test/test_case2.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-08 10:17:04
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-08 17:12:00
 * @FilePath: /bPlusProject/test/test_ase2.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include <cstdint>
#include <vector>

#include "BPlusFunc.h"
#include "gtest/gtest.h"
class BPlusTreeTest : public testing::Test {
 protected:
  BPlusTree<int>* bp;
  UserOperation<int>* user;
  void SetUp() override { bp->setDegree(3); }
};

TEST_F(BPlusTreeTest, TenBillionInsert) {
  for (int i = 1; i <= 10000000; i++) {
    bp->insert(i, 2 * i);
  }
}

TEST_F(BPlusTreeTest, TenBillionBinarySearch) {
  std::vector<uint64_t> vec;
  for (int i = 1; i <= 10000000; i++) {
    bp->optimizedSearch(i, vec);
    EXPECT_EQ(vec[0], 2 * i);
    vec.clear();
  }
}

TEST_F(BPlusTreeTest, TenBillionNormalSearch) {
  std::vector<uint64_t> vec;
  for (int i = 1; i <= 10000000; i++) {
    bp->search(i, vec);
    EXPECT_EQ(vec[0], 2 * i);
    vec.clear();
  }
}

TEST_F(BPlusTreeTest, TenBillionDelete) {
  std::vector<uint64_t> vec;
  for (int i = 1; i <= 10000000; i++) {
    bp->remove(vec, i);
    EXPECT_EQ(vec[0], 2 * i);
    vec.clear();
  }
}