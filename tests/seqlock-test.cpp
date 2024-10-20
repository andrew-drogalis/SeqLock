// Andrew Drogalis Copyright (c) 2024, GNU 3.0 Licence
//
// Inspired from Erik Rigtorp
// Significant Modifications / Improvements
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include <atomic>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "dro/seqlock.hpp"

int main(int argc, char* argv[])
{
  // Basic test
  {
    dro::Seqlock<int> seqlock;
    int result {};
    seqlock.store(1);
    seqlock.load(result);
    assert(result == 1);
    seqlock.store(2);
    seqlock.load(result);
    assert(result == 2);
  }

  // Copyable Only test
  {
    struct Test
    {
      int x_;
      int y_;
      Test() = default;
      Test(int x, int y) : x_(x), y_(y) {}
      ~Test()                      = default;
      Test(const Test&)            = default;
      Test& operator=(const Test&) = default;
      Test(Test&&)                 = delete;
      Test& operator=(Test&&)      = delete;
    };
    dro::Seqlock<Test> seqlock;
    Test val {1, 2};
    seqlock.store(val);
    Test result {};
    seqlock.load(result);
    assert(result.x_ == 1);
    assert(result.y_ == 2);
    // R-value
    seqlock.store(Test(3, 4));
    seqlock.load(result);
    assert(result.x_ == 3);
    assert(result.y_ == 4);
  }

  // Fuzz test
  {
    struct Data
    {
      std::size_t x, y, z;
    };

    dro::Seqlock<Data> seqlock;
    std::atomic<std::size_t> ready(0);
    std::vector<std::thread> threads;
    const int size {100};

    for (int i = {}; i < size; ++i)
    {
      threads.emplace_back([&seqlock, &ready]() {
        while (ready == 0) {}
        for (std::size_t i {}; i < 10'000'000; ++i)
        {
          Data copy {};
          seqlock.load(copy);
          if (copy.x + 100 != copy.y || copy.z != copy.x + copy.y)
          {
            throw std::runtime_error("Read tearing occurred");
          }
        }
        ready--;
      });
    }

    std::size_t counter = 0;
    while (true)
    {
      Data data = {counter++, data.x + 100, data.y + data.x};
      seqlock.store(data);
      if (counter == 1)
      {
        ready += threads.size();
      }
      if (ready == 0)
      {
        break;
      }
    }

    for (auto& thrd : threads) { thrd.join(); }
  }
  std::cout << "Test Completed!\n";

  return 0;
}
