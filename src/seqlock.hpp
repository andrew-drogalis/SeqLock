
// Andrew Drogalis Copyright (c) 2024

#ifndef DRO_SEQLOCK
#define DRO_SEQLOCK

#include <atomic>
#include <cstddef>
#include <new> // for std::hardware_destructive_interference_size

namespace dro
{

template <typename T> class Seqlock
{
private:
#ifdef __cpp_lib_hardware_interference_size
  static constexpr std::size_t kCacheLineSize =
      std::hardware_destructive_interference_size;
#else
  static constexpr std::size_t kCacheLineSize = 64;
#endif
  alignas(kCacheLineSize) T value_;
  alignas(kCacheLineSize) std::atomic<std::size_t> seq_ {0};

public:
  static_assert(std::is_nothrow_copy_assignable<T>::value,
                "T must be nothrow copy assignable");
  static_assert(std::is_trivially_copy_assignable<T>::value,
                "T must be trivially copy assignable");

  Seqlock() = default;

  T load() const noexcept
  {
    T copy;
    std::size_t seq0 {1};
    std::size_t seq1 {0};
    while (seq0 != seq1 && seq0 & 1)
    {
      seq0 = seq_.load(std::memory_order_aquire);
      std::atomic_thread_fence(std::memory_order_acq_rel);
      copy = value_;
      std::atomic_thread_fence(std::memory_order_acq_rel);
      seq1 = seq_.load(std::memory_order_aquire);
    }
    return copy;
  }

  void store(const T& desired) noexcept
  {
    std::size_t seq0 = seq_.load(std::memory_order_relaxed);
    seq_.store(seq0 + 1, std::memory_order_release);
    std::atomic_signal_fence(std::memory_order_acq_rel);
    value_ = desired;
    std::atomic_signal_fence(std::memory_order_acq_rel);
    seq_.store(seq0 + 2, std::memory_order_release);
  }
};
}// namespace dro
#endif
