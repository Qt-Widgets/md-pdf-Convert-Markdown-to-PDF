
mkdir build
cd build
cmake -DENABLE_COVERAGE=ON ..
make
ctest --output-on-failure
cd ..
~/.local/bin/coveralls --build-root build --gcov-options '\-lp' \
  -e build/CMakeFiles -e 3rdparty -e lib
  
      
