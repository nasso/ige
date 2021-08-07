#ifndef FC358CD8_B857_4D31_A6D9_F1BB8298A33A
#define FC358CD8_B857_4D31_A6D9_F1BB8298A33A

#include "ige/utility/RingBuffer.hpp"
#include "ige/utility/Types.hpp"
#include <concepts>
#include <optional>
#include <span>
#include <type_traits>
#include <vector>

namespace ige::core {

template <class E>
concept Event = std::is_empty_v<E> || std::movable<E>;

template <Event E>
class EventChannel;

/**
 * @brief An event reader pulling events from an event channel.
 *
 * @tparam E The event type.
 */
template <Event E>
class EventReader {
public:
    friend EventChannel<E>;

    EventReader(const EventReader&) = default;
    EventReader& operator=(const EventReader&) = default;
    EventReader(EventReader&&) = default;
    EventReader& operator=(EventReader&&) = default;

private:
    EventReader(usize id);

    usize m_id = 0;
};

/**
 * @brief An EventChannel represents an asynchronous queue of events.
 *
 * Anyone is free to push events into the channel. However, in order to read
 * events from the channel, one must register a reader. This is to make sure
 * that every registered reader receives all events exactly once.
 *
 * @tparam E The event type.
 */
template <Event E>
class EventChannel {
public:
    using Reader = EventReader<E>;

    template <class... Args>
        requires std::constructible_from<E, Args...>
    void emplace(Args&&...);

    inline void push(E&&);

    Reader create_reader();

    decltype(auto) read(Reader&) const;

    usize reader_count() const;

private:
    utility::RingBuffer<E> m_buffer;
    std::vector<usize> m_free_readers;
    mutable std::vector<usize> m_readers_last_event;
    usize m_last_event_id = 0;
};

}

#include "EventChannel.inl"

#endif /* FC358CD8_B857_4D31_A6D9_F1BB8298A33A */
