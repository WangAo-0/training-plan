/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-15 16:54:59
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-15 16:56:25
 * @FilePath: /bPlusProject/scripts/main3.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include <iostream>
#include <map>
#include <mutex>
#include <shared_mutex>

// 简化的 B+ 树节点结构
struct BPlusTreeNode {
  int key;
  int value;
};

class BPlusTree {
 public:
  // 构造函数
  BPlusTree() {}

  // 插入操作，写入锁
  void Insert(int key, int value) {
    std::unique_lock<std::shared_mutex> lock(read__write_mutex_);

    // 在此执行插入操作
    // ...

    // 操作完成后释放锁
  }

  // 查找操作，读取锁
  int Find(int key) {
    std::shared_lock<std::shared_mutex> lock(read__write_mutex_);

    // 在此执行查找操作
    // ...

    return 0;  // 返回查找结果
  }

 private:
  // B+ 树节点存储
  std::map<int, BPlusTreeNode> nodes_;

  // 读取锁
  mutable std::shared_mutex read__write_mutex_;
};

int main() {
  BPlusTree bptree;

  // 示例插入操作
  bptree.Insert(1, 100);
  bptree.Insert(2, 200);

  // 示例查找操作
  int result = bptree.Find(1);
  std::cout << "Found result: " << result << std::endl;

  return 0;
}
