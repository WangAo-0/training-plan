cd /home/oliver/cplus/training-plan
###
 # @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 # @Date: 2023-10-17 09:37:19
 # @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 # @LastEditTime: 2023-10-17 11:47:47
 # @FilePath: /training-plan/rebuild.sh
 # @Description: 
 # 
 # Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
### 
rm -rf build
cmake -G Ninja -S . -B ./build
cmake --build ./build
cd /home/oliver/cplus/training-plan/build/src
