# NokiaTest
## Building with minGW
1. install cmake
2. install mingw
3. add the mingw-w64 programs to the system PATH variable .
4. execute in command line
```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw64-make
```
5. run example
```bash
NokiaTest.exe ../tests/default.csv
```
