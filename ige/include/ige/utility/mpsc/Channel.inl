#include "Channel.hpp"
#include <concepts>
#include <optional>

namespace ige::mpsc {

template <std::movable T>
Sender<T>::Sender(std::weak_ptr<std::queue<T>> queue)
    : m_queue(queue)
{
}

/**
 * @brief Send a message to the channel.
 *
 * The message is constructed in-place in the channel's message buffer.
 *
 * @param args The arguments to use to construct the message.
 */
template <std::movable T>
template <class... Args>
void Sender<T>::emplace(Args&&... args)
{
    m_queue.lock()->emplace(std::forward<Args>(args)...);
}

/**
 * @brief Send a message to the channel.
 *
 * @param msg The message to send.
 */
template <std::movable T>
void Sender<T>::send(T&& msg)
{
    m_queue.lock()->push(msg);
}

template <std::movable T>
void Sender<T>::send(const T& msg)
{
    static_assert(std::copyable<T>, "T cannot be copied. Use std::move.");

    m_queue.lock()->push(msg);
}

/**
 * @brief Check if the channel is closed.
 *
 * When closed, no more messages can be sent.
 *
 * @return true if the channel is closed, false otherwise.
 */
template <std::movable T>
bool Sender<T>::closed() const
{
    return m_queue.expired();
}

/**
 * @brief Attempt to receive a message without blocking.
 *
 * If no message is immediately available, the function returns std::nullopt.
 * If the channel is closed, the function also returns std::nullopt. To know if
 * the channel is closed, use Receiver::closed().
 *
 * @return The received message, or std::nullopt if none was available.
 */
template <std::movable T>
std::optional<T> Receiver<T>::try_receive()
{
    if (m_queue->empty())
        return std::nullopt;

    auto v = std::move(m_queue->front());
    m_queue->pop();
    return std::move(v);
}

/**
 * @brief Create a new sender for this receiver.
 */
template <std::movable T>
Sender<T> Receiver<T>::make_sender() const
{
    return Sender<T>(m_queue);
}

/**
 * @brief Create a new MPSC channel.
 *
 * @tparam T
 * @return A sender to the channel, and the channel's only receiver.
 */
template <std::movable T>
std::pair<Sender<T>, Receiver<T>> make_channel()
{
    Receiver<T> receiver;

    return std::make_pair<Sender<T>, Receiver<T>>(
        receiver.make_sender(),
        std::move(receiver));
}

}
