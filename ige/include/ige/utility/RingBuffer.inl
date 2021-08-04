#include "RingBuffer.hpp"
#include "ige/core/Types.hpp"
#include "ige/utility/CallOnExit.hpp"
#include <algorithm>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>

namespace ige::utility {

/**
 * @brief Construct an empty RingBuffer.
 *
 * Allocation is delayed until the first write.
 *
 * @param allocator Allocator to use for memory allocation.
 */
template <std::movable T, class Allocator>
RingBuffer<T, Allocator>::RingBuffer(const Allocator& allocator)
    : m_allocator(allocator)
{
}

/**
 * @brief Construct a RingBuffer with the given capacity.
 *
 * @param capacity The capacity of the RingBuffer.
 * @param allocator Allocator to use for memory allocation.
 */
template <std::movable T, class Allocator>
RingBuffer<T, Allocator>::RingBuffer(usize capacity, const Allocator& allocator)
    : RingBuffer(allocator)
{
    reserve(capacity);
}

/**
 * @brief Construct a RingBuffer copying the data from the given RingBuffer.
 *
 * @param other The RingBuffer to copy.
 */
template <std::movable T, class Allocator>
RingBuffer<T, Allocator>::RingBuffer(
    const RingBuffer& other) requires std::copy_constructible<T>
    : RingBuffer(AllocatorTraits::select_on_container_copy_construction(
          other.m_allocator))
{
    *this = other;
}

/**
 * @brief Construct a RingBuffer moving the data from the given RingBuffer.
 *
 * @param other The RingBuffer to move.
 */
template <std::movable T, class Allocator>
RingBuffer<T, Allocator>::RingBuffer(RingBuffer&& other)
    : m_allocator(std::move(other.m_allocator))
    , m_buffer(other.m_buffer)
    , m_head(other.m_head)
    , m_size(other.m_size)
    , m_capacity(other.m_capacity)
{
    other.mark_as_moved();
}

/**
 * @brief Destroy the RingBuffer.
 */
template <std::movable T, class Allocator>
RingBuffer<T, Allocator>::~RingBuffer()
{
    destroy_elements(m_head, m_size);

    AllocatorTraits::deallocate(m_allocator, m_buffer, m_capacity);
}

/**
 * @brief Overwrite all elements with a copy of the given RingBuffer's data.
 *
 * @param other The RingBuffer to copy.
 * @return A reference to this RingBuffer.
 */
template <std::movable T, class Allocator>
auto RingBuffer<T, Allocator>::operator=(const RingBuffer& other)
    -> RingBuffer& requires std::copy_constructible<T>
{
    // TODO: dumb copy if T is trivially copyable

    clear();
    reserve(other.size());
    other.for_each([&](const T& element) { emplace(element); });

    return *this;
}

/**
 * @brief Move the data from the given RingBuffer.
 *
 * @param other
 * @return
 */
template <std::movable T, class Allocator>
auto RingBuffer<T, Allocator>::operator=(RingBuffer&& other) -> RingBuffer&
{
    // TODO: dumb copy if T is trivially copyable

    clear();
    reserve(other.size());
    while (auto elem = other.pop()) {
        emplace(std::move(*elem));
    }

    return *this;
}

/**
 * @brief Add an element to the RingBuffer.
 *
 * The element is constructed in place.
 *
 * @return A reference to the element that was added.
 */
template <std::movable T, class Allocator>
template <class... Args>
    requires std::constructible_from<T, Args...>
auto RingBuffer<T, Allocator>::emplace(Args&&... args) -> T&
{
    // grow if the buffer is full
    if (m_size == m_capacity) {
        reserve(
            m_capacity == 0 ? RingBuffer::DEFAULT_CAPACITY : m_capacity * 2);
    }

    auto& element = m_buffer[(m_head + m_size) % m_capacity];

    AllocatorTraits::construct(
        m_allocator,
        &element,
        std::forward<Args>(args)...);
    m_size++;

    return element;
}

/**
 * @brief Remove the next element from the RingBuffer, if any.
 *
 * @return The element that was removed, or an empty optional if the RingBuffer
 *          was empty.
 */
template <std::movable T, class Allocator>
std::optional<T> RingBuffer<T, Allocator>::pop()
{
    if (m_size == 0) {
        return std::nullopt;
    } else {
        T* element_ptr = &m_buffer[m_head];

        m_head = (m_head + 1) % m_capacity;
        m_size--;

        if constexpr (std::is_trivially_destructible_v<T>) {
            // trivially destructible types do not need to be explicitly
            // destroyed
            return { std::move(*element_ptr) };
        } else {
            // delay destruction until after the element was moved
            utility::CallOnExit destroy_popped_elem([&, element_ptr] {
                AllocatorTraits::destroy(m_allocator, *element_ptr);
            });

            return { std::move(*element_ptr) };
        }
    }
}

/**
 * @brief Get the next element in the RingBuffer, without removing it.
 *
 * @return A pointer to the next element in the RingBuffer, or a null pointer if
 *          the RingBuffer is empty.
 */
template <std::movable T, class Allocator>
inline T* RingBuffer<T, Allocator>::peek_mut()
{
    return m_size == 0 ? nullptr : &m_buffer[m_head];
}

/**
 * @brief Get the next element in the RingBuffer, without removing it.
 *
 * The returned pointer is constant. If you want a mutable pointer, use
 * @a peek_mut instead.
 *
 * @return A constant pointer to the next element in the RingBuffer, or a null
 *          pointer if the RingBuffer is empty.
 */
template <std::movable T, class Allocator>
inline const T* RingBuffer<T, Allocator>::peek() const
{
    return m_size == 0 ? nullptr : &m_buffer[m_head];
}

/**
 * @brief Clear the RingBuffer, removing all elements.
 */
template <std::movable T, class Allocator>
void RingBuffer<T, Allocator>::clear()
{
    destroy_elements(m_head, m_size);

    // reset the buffer
    m_head = 0;
    m_size = 0;
}

/**
 * @brief Reserve space for the given number of elements.
 *
 * The new capacity will be the total amount of elements the RingBuffer can
 * hold after this call, before reallocation is needed. If the new capacity
 * is smaller than the current capacity, the RingBuffer will NOT be shrunk.
 *
 * @param new_capacity The total number of elements to reserve space for.
 */
template <std::movable T, class Allocator>
void RingBuffer<T, Allocator>::reserve(usize new_capacity)
{
    if (new_capacity > m_capacity) {
        // allocate new buffer
        T* new_buffer = AllocatorTraits::allocate(m_allocator, new_capacity);

        if (m_size != 0) {
            // buffer isn't empty, move the elements to the new buffer

            if (m_size > m_capacity - m_head) {
                // buffer looks like this:
                // |####------####|
                //  (B)       (A)

                // move part A to the beginning of the new buffer
                auto after_part_a = std::move(
                    m_buffer + m_head,
                    m_buffer + m_capacity,
                    new_buffer);

                // move part B right after the end of part A
                std::move(
                    m_buffer,
                    m_buffer + (m_head + m_size) % m_capacity,
                    after_part_a);
            } else {
                // buffer looks like this: |---#########---|

                // no need to mod because data is linear here
                std::move(
                    m_buffer + m_head,
                    m_buffer + m_head + m_size,
                    new_buffer);
            }

            // destroy moved elements
            destroy_elements(m_head, m_size);

            // head is now at the beginning of the new buffer
            m_head = 0;
        }

        // replace the old buffer with the new buffer
        AllocatorTraits::deallocate(m_allocator, m_buffer, m_capacity);
        m_buffer = new_buffer;
        m_capacity = new_capacity;
    }
}

/**
 * @brief Check if the RingBuffer is empty.
 *
 * @return true if the RingBuffer is empty, false otherwise.
 */
template <std::movable T, class Allocator>
inline bool RingBuffer<T, Allocator>::empty() const
{
    return m_size == 0;
}

/**
 * @brief Get the capacity of the RingBuffer.
 *
 * The capacity is the total number of elements the RingBuffer can hold
 * without reallocation. When the capacity is reached, the RingBuffer will
 * be reallocated to twice the capacity.
 *
 * @return The capacity of the RingBuffer.
 */
template <std::movable T, class Allocator>
inline usize RingBuffer<T, Allocator>::capacity() const
{
    return m_capacity;
}

/**
 * @brief Get the size of the RingBuffer, i.e. the number of elements it holds.
 *
 * @return The size of the RingBuffer.
 */
template <std::movable T, class Allocator>
inline usize RingBuffer<T, Allocator>::size() const
{
    return m_size;
}

/**
 * @brief Apply a function to each element in the RingBuffer.
 *
 * The function is invoked with a mutable reference to the element. If you don't
 * need a mutable reference, use @a for_each instead.
 *
 * @param f The function to apply to each element.
 */
template <std::movable T, class Allocator>
template <std::invocable<T&> F>
void RingBuffer<T, Allocator>::for_each_mut(F&& f)
{
    for (usize i = 0; i < m_size; ++i) {
        T& element = m_buffer[(m_head + i) % m_capacity];

        std::invoke(f, element);
    }
}

/**
 * @brief Apply a function to each element in the RingBuffer.
 *
 * The function is invoked with a constant reference to the element. If you need
 * a mutable reference, use @a for_each_mut.
 *
 * @param f The function to apply to each element.
 */
template <std::movable T, class Allocator>
template <std::invocable<const T&> F>
void RingBuffer<T, Allocator>::for_each(F&& f) const
{
    for (usize i = 0; i < m_size; ++i) {
        const T& element = m_buffer[(m_head + i) % m_capacity];

        std::invoke(f, element);
    }
}

/**
 * @brief Destroy all elements in the given range.
 *
 * @param start The circular index of the first element to destroy.
 * @param count The number of elements to destroy.
 */
template <std::movable T, class Allocator>
inline void RingBuffer<T, Allocator>::destroy_elements(usize start, usize count)
{
    // no-op if elements are trivially destructible
    if constexpr (std::is_trivially_destructible_v<T>) {
        return;
    }

    for (usize i = 0; i < count; ++i) {
        AllocatorTraits::destroy(
            m_allocator,
            &m_buffer[(start + i) % m_capacity]);
    }
}

/**
 * @brief Reset the RingBuffer to its initial state after a move.
 *
 * This method is called on a RingBuffer when it is moved away from. Since the
 * ownership of the buffer is transferred, the buffer is not deallocated.
 */
template <std::movable T, class Allocator>
void RingBuffer<T, Allocator>::mark_as_moved()
{
    m_buffer = nullptr;
    m_head = 0;
    m_size = 0;
    m_capacity = 0;
}

}
