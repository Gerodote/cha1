# cha1

## how to run it ?
```
git clone https://github.com/Gerodote/denisov_lab1
cd denisov_lab1
cmake -S . -B ./build -Dcha1_BUILD_EXECUTABLE=1 -Dcha1_ENABLE_VCPKG=1 -Dcha1_ENABLE_LTO=1 -DCMAKE_BUILD_TYPE="Release" -DCMAKE_CXX_FLAGS="-march=native"
cmake --build ./build --target all --parallel 4 
./build/bin/Release/cha1
```
