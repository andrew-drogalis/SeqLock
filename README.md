# Seqlock

## About the Project

## Benchmarks

These benchmarks were taken on a (4) core Intel(R) Core(TM) i5-9300H CPU @ 2.40GHz with isolcpus on cores 2 and 3. The linux kernel is v6.10.11-200.fc40.x86_64 and compiled with gcc version 14.2.1.

| Operations per ms |
| ----------------- |
| 149,457           |

## Background Information

<img src="https://raw.githubusercontent.com/drogalis/Seqlock/main/assets/seqlock1.jpg" alt="Seqlock" style="width: 500px; padding-top: 10px;">

[Source](http://blog.kongfy.com/2017/04/%E5%B9%B6%E5%8F%91%E7%BC%96%E7%A8%8B%E7%89%9B%E5%88%80%E5%B0%8F%E8%AF%95%EF%BC%9Aseqlock/)

## Installing

To build and install the shared library, run the commands below.

```
    $ mkdir build && cd build
    $ cmake .. -DCMAKE_BUILD_TYPE=Release
    $ sudo make install
```

## Sources

Inspiration came from these sources. I recommend checking them out:

- [Rigtorp SPSCQueue](https://github.com/rigtorp/SPSCQueue)
- [Boost SPSC Queue](https://www.boost.org/doc/libs/1_60_0/boost/lockfree/spsc_queue.hpp)
