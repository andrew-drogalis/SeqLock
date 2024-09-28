# Seqlock

## Benchmarks

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
