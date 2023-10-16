/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-15 17:17:52
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-15 17:18:54
 * @FilePath: /bPlusProject/scripts/main5.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

std::shared_mutex rw_mutex;  // 读写锁
int shared_data = 0;

void ReadOperation(std::string name) {
  std::shared_lock<std::shared_mutex> lock(rw_mutex);  // 获取读取锁
  std::cout << "enter1" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << name << "Read operation: " << shared_data << std::endl;
}

void ReadOperation2(std::string name) {
  std::shared_lock<std::shared_mutex> lock(rw_mutex);  // 获取读取锁
  std::cout << "enter2" << std::endl;
  std::cout << name << "Read operation: " << shared_data << std::endl;
}

void WriteOperation3(std::string name) {
  std::unique_lock<std::shared_mutex> lock(rw_mutex);  // 获取写锁
  std::cout << name << " enter" << std::endl;
  shared_data = 3;
}

void WriteOperation4(std::string name) {
  std::unique_lock<std::shared_mutex> lock(rw_mutex);  // 获取写锁
  std::cout << name << " enter" << std::endl;
  shared_data = 4;
}

int main() {
  // 默认是读优先，即只要有读的计数器不为0，则写一直写不了；
  std::thread reader1(ReadOperation, "reader1");
  std::thread writer3(WriteOperation3, "write3");
  std::thread reader2(ReadOperation2, "reader2");
  std::thread writer4(WriteOperation4, "write4");

  reader1.join();
  writer3.join();
  reader2.join();
  writer4.join();
  std::cout << "final " << shared_data << std::endl;
  return 0;
}