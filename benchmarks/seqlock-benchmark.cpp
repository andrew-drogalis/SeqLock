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

#include <chrono>
#include <string>
#include <thread>
#include <iostream>

#include <pthread.h>
#include <sched.h>

void pinThread(int cpu)
{
  if (cpu < 0)
  {
    return;
  }
  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);
  CPU_SET(cpu, &cpu_set);
  if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu_set) == 1)
  {
    perror("pthread_setaffinity_np");
    exit(1);
  }
}

int main(int argc, char* argv[])
{
  int cpu1 {-1};
  int cpu2 {-1};

  if (argc == 3)
  {
    cpu1 = std::stoi(argv[1]);
    cpu2 = std::stoi(argv[2]);
  }

  const std::size_t iters {10'000'000};

  struct alignas(4) Data
  {
    int x;
  };

  dro::Seqlock<Data> seqlock;

  auto thrd = std::thread([&] {
    pinThread(cpu1);
    for (int i {}; i < iters; ++i)
    {
      for (;;)
      {
        auto data = seqlock.load();
        if (data.x == 100)
        {
          break;
        }
      }
    }
  });

  pinThread(cpu2);

  auto start = std::chrono::steady_clock::now();
  for (int i {}; i < iters; ++i)
  {
    seqlock.store({0});
    seqlock.store({100});
  }
  thrd.join();
  auto stop = std::chrono::steady_clock::now();

  std::cout << "Operations per ms: " << iters * 1'000'000 /
                   std::chrono::duration_cast<std::chrono::nanoseconds>(stop -
                                                                        start)
                       .count()
            << '\n';

  return 0;
}
