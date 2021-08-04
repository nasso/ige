#ifndef A6B8546A_B04A_4D35_A638_497082B9F07A
#define A6B8546A_B04A_4D35_A638_497082B9F07A

#include "ige/core/Types.hpp"
#include <concepts>
#include <memory>
#include <optional>

namespace ige::utility {

/**
 * @brief A container implementing a FIFO ring buffer.
 *
 * Elements are added to the "tail" of the RingBuffer and removed from the
 * "head". When an element is added, the RingBuffer is automatically resized to
 * fit the new element (if necessary).
 *
 * The RingBuffer's internal buffer defines three values:
 *
 *         head                    capacity
 *          v                         v
 *     |----###########---------------|
 *          |- size -->
 *
 * The "head" cursor points to the location where the next element will be
 * removed. The "size" represents how many elements follow the head. The
 * "capacity" indicates how many elements can be stored in the buffer before it
 * must be resized. An empty buffer has a size of 0, but its capacity can be
 * any value.
 *
 * Adding an element increases the size by 1. Removing an element decreases the
 * size by 1 and advances the head cursor. Elements are placed one after the
 * other, until the end of the buffer is reached, in which case new elements
 * will be placed at the beginning of it (if there's room).
 *
 *                       head      capacity
 *                        v           v
 *     |########----------############|
 *   ...------->          |- size ----...
 *
 * As the buffer grows, the size gets closer to the capacity. When the size
 * reaches the capacity, the buffer is resized to twice its current size.
 *
 *             head      capacity
 *              v           v
 *     |####################|
 *   ...------->|- size ----...
 *
 *     head                                  capacity
 *      v                                       v
 *     |####################--------------------|
 *      |------ size ------>----- size * 2 ---->
 *
 * When the buffer is resized, elements are moved to be tightly packed at the
 * beginning of the buffer.
 *
 * The RingBuffer class is not thread-safe (it can be externally synchronized).
 *
 * @tparam T The type of the elements in the RingBuffer.
 * @tparam Allocator An allocator that can be used to allocate memory for the
 *                      elements in the RingBuffer.
 */
template <std::movable T, class Allocator = std::allocator<T>>
class RingBuffer {
public:
    RingBuffer(const Allocator& = Allocator());

    RingBuffer(usize capacity, const Allocator& allocator = Allocator());

    RingBuffer(const RingBuffer&) requires std::copy_constructible<T>;

    RingBuffer(RingBuffer&&);

    ~RingBuffer();

    RingBuffer&
    operator=(const RingBuffer&) requires std::copy_constructible<T>;

    RingBuffer& operator=(RingBuffer&&);

    template <class... Args>
        requires std::constructible_from<T, Args...>
    auto emplace(Args&&...) -> T&;

    std::optional<T> pop();

    inline T* peek_mut();

    inline const T* peek() const;

    void clear();

    void reserve(usize new_capacity);

    inline bool empty() const;

    inline usize capacity() const;

    inline usize size() const;

    template <std::invocable<T&> F>
    void for_each_mut(F&& f);

    template <std::invocable<const T&> F>
    void for_each(F&& f) const;

private:
    inline void destroy_elements(usize start, usize count);

    void mark_as_moved();

    Allocator m_allocator;
    T* m_buffer = nullptr;
    usize m_head = 0;
    usize m_size = 0;
    usize m_capacity = 0;

    static constexpr usize DEFAULT_CAPACITY = 16;

    using AllocatorTraits = std::allocator_traits<Allocator>;
};

}

#include "RingBuffer.inl"

#endif /* A6B8546A_B04A_4D35_A638_497082B9F07A */
