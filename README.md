# Lock-Free Benchmark

A C++20 project comparing the performance of a mutex-based concurrent queue
vs. a lock-free queue using std::atomic and CAS operations.

## Building
```bash
mkdir build && cd build
cmake ..
make
./benchmark
```

## Tests
```bash
cd build
ctest --output-on-failure
```
