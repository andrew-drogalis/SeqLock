// Andrew Drogalis Copyright (c) 2024, GNU 3.0 Licence
//
// Inspired from Erik Rigtorp
// Significant Modifications / Improvements
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#ifndef DRO_SEQLOCK
#define DRO_SEQLOCK

#include <atomic>     // for atomic, memory_order
#include <concepts>   // for concept, requires
#include <cstddef>    // for std::size_t
#include <new>        // for std::hardware_destructive_interference_size
#include <type_traits>// for std::is_default_constructible_v

namespace dro
{

#ifdef __cpp_lib_hardware_interference_size
static constexpr std::size_t cacheLineSize =
    std::hardware_destructive_interference_size;
#else
static constexpr std::size_t cacheLineSize = 64;
#endif

template <typename T>
concept Seq_Type =
    std::is_default_constructible_v<T> &&
    (std::is_move_assignable_v<T> || std::is_copy_assignable_v<T>);

template <typename T>
concept Seq_NoThrow =
    ((std::is_nothrow_copy_assignable_v<T> && std::is_copy_assignable_v<T>) ||
     (std::is_nothrow_move_assignable_v<T> && std::is_move_assignable_v<T>));

template <Seq_Type T> class alignas(cacheLineSize) Seqlock
{
  using size_type  = std::size_t;
  using value_type = T;

private:
  alignas(cacheLineSize) value_type value_;
  alignas(cacheLineSize) std::atomic<size_type> seq_ {0};

public:
  Seqlock() = default;

  [[nodiscard]] value_type load() const noexcept(Seq_NoThrow<T>)
  {
    value_type output;
    size_type seqStart {0};
    size_type seqEnd {1};
    while (seqStart != seqEnd || seqStart & 1)
    {
      seqStart = seq_.load(std::memory_order_acquire);
      std::atomic_thread_fence(std::memory_order_acq_rel);
      read_value(output);
      std::atomic_thread_fence(std::memory_order_acq_rel);
      seqEnd = seq_.load(std::memory_order_acquire);
    }
    return output;
  }

  void store(const value_type& input) noexcept(Seq_NoThrow<T>)
  {
    std::size_t seqStart = seq_.load(std::memory_order_relaxed);
    seq_.store(seqStart + 1, std::memory_order_release);
    std::atomic_signal_fence(std::memory_order_acq_rel);
    write_value(input);
    std::atomic_signal_fence(std::memory_order_acq_rel);
    seq_.store(seqStart + 2, std::memory_order_release);
  }

private:
  void read_value(value_type& output) const noexcept(Seq_NoThrow<T>)
    requires std::is_copy_assignable_v<T> && (! std::is_move_assignable_v<T>)
  {
    output = value_;
  }

  void read_value(value_type& output) const noexcept(Seq_NoThrow<T>)
    requires std::is_move_assignable_v<T>
  {
    output = std::move(value_);
  }

  void write_value(const value_type& input) noexcept(Seq_NoThrow<T>)
    requires std::is_copy_assignable_v<T> && (! std::is_move_assignable_v<T>)
  {
    value_ = input;
  }

  void write_value(const value_type& input) noexcept(Seq_NoThrow<T>)
    requires std::is_move_assignable_v<T>
  {
    value_ = std::move(input);
  }
};
}// namespace dro

#endif
