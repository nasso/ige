#ifndef DC695F60_BA9B_40DB_A265_378CD6D4D5E7
#define DC695F60_BA9B_40DB_A265_378CD6D4D5E7

#include <concepts>
#include <cstddef>
#include <deque>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

namespace ige {
namespace core {

    template <typename E>
    concept Event = std::movable<E>;

    template <Event E>
    class EventChannel {
    private:
        using Cursor = std::size_t;
        using Handle = std::size_t;

        std::deque<E> m_buffer;
        std::unordered_map<Handle, Cursor> m_sub_handles;

        Handle make_handle()
        {
            Handle handle = 0;

            while (m_sub_handles.contains(handle)) {
                handle++;
            }

            m_sub_handles.emplace(handle, m_buffer.size());
            return handle;
        }

        void free_handle(Handle handle)
        {
            m_sub_handles.erase(handle);
        }

        std::optional<std::reference_wrapper<const E>> read_event(Handle handle)
        {
            auto iter = m_sub_handles.find(handle);
            if (iter != m_sub_handles.end()) {
                Cursor& cur = iter->second;

                if (cur < m_buffer.size()) {
                    const E& event = m_buffer[cur];

                    cur++;

                    return { std::cref(event) };
                }
            }

            return {};
        }

    public:
        class Subscription {
        public:
            Subscription(EventChannel& channel)
                : m_channel(channel)
                , m_handle(channel.make_handle())
            {
            }

            Subscription(const Subscription& other)
                : Subscription(other.m_channel)
            {
            }

            ~Subscription()
            {
                m_channel.free_handle(m_handle);
            }

            std::optional<std::reference_wrapper<const E>> next_event()
            {
                return m_channel.read_event(m_handle);
            }

        private:
            EventChannel& m_channel;
            Handle m_handle;
        };

        EventChannel() = default;

        Subscription subscribe()
        {
            return Subscription(*this);
        }

        void push(E event)
        {
            emplace(std::move(event));
        }

        template <typename... Args>
            requires std::constructible_from<E, Args...>
        void emplace(Args&&... args)
        {
            if (m_sub_handles.empty()) {
                return;
            }

            m_buffer.emplace_back(std::forward<Args>(args)...);
        }
    };

}
}

#endif /* DC695F60_BA9B_40DB_A265_378CD6D4D5E7 */
