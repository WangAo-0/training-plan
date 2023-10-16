/***
 * @Author: wangao23 oliverdebox@163.com
 * @Date: 2023-09-15 15:02:30
 * @LastEditors: wangao23 oliverdebox@163.com
 * @LastEditTime: 2023-09-15 15:03:14
 * @FilePath: /bPlusProject/scripts/main.cpp
 * @Description:
 * @
 * @Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <ostream>
#include <thread>

class Resource {
 public:
  void Read() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!canRead_) {
      cv_.wait(lock);
    }
    // 进行读操作
  }

  void Write() {
    std::unique_lock<std::mutex> lock(mutex_);
    canRead_ = false;
    // 进行写操作
    canRead_ = true;
    cv_.notify_all();
  }

 private:
  std::mutex mutex_;
  std::condition_variable cv_;
  bool canRead_ = true;
};

int main() {
  Resource resource;

  // 启动多个读线程
  std::thread reader1([&]() {
    resource.Read();
    // 进行读操作
    std::cout << "reader1" << std::endl;
  });

  std::thread reader2([&]() {
    resource.Read();
    // 进行读操作
    std::cout << "reader2" << std::endl;
  });

  // 启动写线程
  std::thread writer([&]() {
    resource.Write();
    // 进行写操作
    std::cout << "writer" << std::endl;
  });

  reader1.join();
  reader2.join();
  writer.join();

  return 0;
}
