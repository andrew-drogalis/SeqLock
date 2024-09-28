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

#include <thread>

int main(int argc, char* argv[])
{

  struct Data
  {
    int x;
  };

  dro::Seqlock<Data> seqlock;
  seqlock.store({0});

  auto thrd = std::thread([&] {
    for (;;)
    {
      auto data = seqlock.load();
      if (data.x == 100)
      {
        return;
      }
    }
  });

  seqlock.store({100});
  thrd.join();

  return 0;
}
