#pragma once

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket);

    void run() { readSocket(); }

private:
    void readSocket();
    void writeSocket();
    template <typename T>
    void readVal(std::function<void(T)> cb);
    template <typename T>
    void readVal(size_t bufSize, std::function<void(std::vector<T>)> cb);
    template <typename vT, typename T>
    void addToBuf(std::vector<vT>& buf, T val);

    tcp::socket m_socket;
    Entry* m_selectedEntry = nullptr;
};

template <typename T>
inline void Session::readVal(std::function<void(T)> cb)
{
    std::sptr<T> pVal = std::make::sptr<T>();
    auto self = shared_from_this();
    m_socket.async_read_some(
        boost::asio::buffer(&*pVal, sizeof(T)),
        [this, self, pVal, cb = std::move(cb)](boost::system::error_code ec, std::size_t length) {
            if (ec || length != sizeof(T))
            {
                m_socket.close();
                std::println("Client disconnected");
                return;
            }
            if (cb)
                cb(std::move(*pVal));
        });
}

template <typename T>
inline void Session::readVal(size_t bufSize, std::function<void(std::vector<T>)> cb)
{
    static constexpr size_t MAX_BUF_SIZE = 2048;
    if (bufSize > MAX_BUF_SIZE)
    {
        m_socket.close();
        std::println("Client disconnected");
        return;
    }
    auto self = shared_from_this();
    std::sptr<std::vector<T>> pVal = std::make::sptr<std::vector<T>>();
    pVal->resize(bufSize);
    m_socket.async_read_some(
        boost::asio::buffer(pVal->data(), bufSize),
        [this, self, bufSize, pVal, cb = std::move(cb)](boost::system::error_code ec, std::size_t length) {
            if (ec || length != sizeof(T) * bufSize || length > MAX_BUF_SIZE)
            {
                m_socket.close();
                std::println("Client disconnected");
                return;
            }
            if (cb)
                cb(std::move(*pVal));
        });
}

template <typename vT, typename T>
inline void Session::addToBuf(std::vector<vT>& buf, T val)
{
    buf.insert(buf.end(), reinterpret_cast<vT*>(&val), reinterpret_cast<vT*>(&val) + sizeof(T));
}
