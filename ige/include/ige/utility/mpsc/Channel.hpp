#ifndef AB7DAF94_2D25_4EC9_B663_C9002F66BBD5
#define AB7DAF94_2D25_4EC9_B663_C9002F66BBD5

#include <concepts>
#include <memory>
#include <optional>
#include <queue>
#include <utility>

namespace ige::mpsc {

template <std::movable T>
class Receiver;

template <std::movable T>
class Sender {
    friend class Receiver<T>;

public:
    Sender(const Sender&) = default;
    Sender& operator=(const Sender&) = default;
    Sender(Sender&&) = default;
    Sender& operator=(Sender&&) = default;

    template <class... Args>
    void emplace(Args&&... args);

    void send(T&&);
    void send(const T&);

    bool closed() const;

private:
    Sender(std::weak_ptr<std::queue<T>> queue);

    std::weak_ptr<std::queue<T>> m_queue;
};

template <std::movable T>
class Receiver {
public:
    Receiver() = default;

    // movable
    Receiver(Receiver&&) = default;
    Receiver& operator=(Receiver&&) = default;

    // non copyable
    Receiver(const Receiver&) = delete;
    Receiver& operator=(const Receiver&) = delete;

    std::optional<T> try_receive();

    Sender<T> make_sender() const;

private:
    std::shared_ptr<std::queue<T>> m_queue = std::make_shared<std::queue<T>>();
};

template <std::movable T>
std::pair<Sender<T>, Receiver<T>> make_channel();

}

#include "Channel.inl"

#endif /* AB7DAF94_2D25_4EC9_B663_C9002F66BBD5 */
