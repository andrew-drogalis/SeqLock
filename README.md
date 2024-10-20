# Seqlock

A [sequence lock](https://en.wikipedia.org/wiki/Seqlock) for passing message from a single writer thread to multiple reader threads. The readers are non-blocking, so the writer can always store.
Avoids the problem of [writer starvation](https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock).

## Table of Contents

- [Usage](#Usage)
- [Benchmarks](#Benchmarks)
- [Installing](#Installing)
- [Sources](#Sources)

## Implementation

The seqlock is based on an atomic sequence that is increamented during store operations. At the beginning of the store, the sequence is increamented by one to an odd value.
After the store is completed, the sequence is increamented by two to an even value.

The reader confirms that the sequence value is the same as when it started, and it
confirms that the value is an even sequence. An atomic thread fence is used to prevent tearing on the read and write operations.

<img src="https://raw.githubusercontent.com/drogalis/Seqlock/main/assets/seqlock1.jpg" alt="Seqlock" style="..spadding-top: 10px;">

[Source](http://blog.kongfy.com/2017/04/%E5%B9%B6%E5%8F%91%E7%BC%96%E7%A8%8B%E7%89%9B%E5%88%80%E5%B0%8F%E8%AF%95%EF%BC%9Aseqlock/)

## Usage

The full list of template arguments are as follows:

Type: Must be default constructible and must be copy assignable.

**Methods**

- `void value_type load(value_type& output) const noexcept(Seq_NoThrow<T>);`

  Load stored value from multiple reader threads.

- `void store(const value_type& input) noexcept(Seq_NoThrow<T>);`

  Store a value from a single writer thread.

## Benchmarks

These benchmarks were taken on a (4) core Intel(R) Core(TM) i5-9300H CPU @ 2.40GHz with isolcpus on cores 2 and 3.
The linux kernel is v6.10.11-200.fc40.x86_64 and compiled with gcc version 14.2.1.

Most important aspects of benchmarking:

- Have at least one core isolated with isolcpus enabled in Grub.
- Compile with -DCMAKE_BUILD_TYPE=Release
- Pass isolated cores ID number as an executable argument i.e. ./Seqlock-Benchmark 2 3

| Operations per ms |
| ----------------- |
| 149,457           |

## Installing

To build and install the shared library, run the commands below.

```
    $ mkdir build && cd build
    $ cmake ..
    $ make
    $ sudo make install
```

## Sources

Inspiration came from these sources. I recommend checking them out:

- [Rigtorp Seqlock](https://github.com/rigtorp/Seqlock)

## License

This software is distributed under the GNU license. Please read [LICENSE](https://github.com/drogalis/Seqlock/blob/main/LICENSE) for information on the software availability and distribution.
