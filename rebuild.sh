cd /home/oliver/Developer/bPlusProject
rm -rf build
cmake -G Ninja -S . -B ./build
cmake --build ./build
cd /home/oliver/Developer/bPlusProject/build/src
