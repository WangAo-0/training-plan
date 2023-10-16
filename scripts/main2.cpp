/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-15 15:27:03
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-15 15:28:07
 * @FilePath: /bPlusProject/scripts/main2.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

struct BPTreeNode {
  int key;
  int value;
  std::mutex node_mutex;  // 1
  int version;            // 2
  BPTreeNode* parent;     // 3
  std::vector<BPTreeNode*> children;
};

void crab_up(BPTreeNode* node) {
  while (node) {
    node->node_mutex.lock();  // 获取节点锁
    node = node->parent;
  }
}

void release_up(BPTreeNode* node) {
  while (node) {
    node->node_mutex.unlock();  // 释放节点锁
    node = node->parent;
  }
}

void insert(BPTreeNode* root, int key, int value) {
  BPTreeNode* newNode = new BPTreeNode();
  newNode->key = key;
  newNode->value = value;
  newNode->version = 1;

  // 开始Latch Crabbing，向上遍历获取节点锁
  crab_up(root);

  // 此时已经获得了从根节点到叶子节点的所有节点的锁
  // 在这里执行插入操作

  // 释放从叶子节点到根节点的所有节点的锁
  release_up(root);
}

int main() {
  BPTreeNode* root = new BPTreeNode();
  root->key = 50;
  root->value = 500;
  root->version = 1;

  std::thread t1(insert, root, 30, 300);
  std::thread t2(insert, root, 70, 700);

  t1.join();
  t2.join();

  return 0;
}
