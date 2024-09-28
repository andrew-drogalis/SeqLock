// Andrew Drogalis Copyright (c) 2024, GNU 3.0 Licence
//
// Inspired from Erik Rigtorp
// Significant Modifications / Improvements
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "dro/seqlock.hpp"

#include <atomic>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

int main(int argc, char* argv[])
{
  // Basic test
  {
    dro::Seqlock<int> seqlock;
    seqlock.store(1);
    assert(seqlock.load() == 1);
    seqlock.store(2);
    assert(seqlock.load() == 2);
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

    for (int i = 0; i < 100; ++i)
    {
      threads.emplace_back([&seqlock, &ready]() {
        while (ready == 0) {}
        for (std::size_t i {}; i < 10'000'000; ++i)
        {
          auto data = seqlock.load();
          if (data.x + 100 != data.y || data.z != data.x + data.y)
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
      Data data = {counter++, data.x + 100, data.y + data.z};
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

    std::cout << "Test Completed!\n";
  }

  return 0;
}
