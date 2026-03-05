#include "Session.hpp"
#include "Entry.hpp"
#include "ConfigReader.hpp"
#include "StatPrinter.hpp"

Session::Session(tcp::socket socket)
:
    m_socket(std::move(socket))
{}

void Session::readSocket()
{
    auto self = shared_from_this();
    readVal<bool>([this, self](bool isGetOperation) {
        readVal<size_t>([this, self, isGetOperation](size_t bufSize) {
            readVal<char>(bufSize, [this, self, isGetOperation](std::vector<char> buf) {
                std::string key(buf.begin(), buf.end());

                if (isGetOperation)
                {
                    m_selectedEntry = ConfigReader::inst().safeRead<Entry*>([&](EntriesContainer& entries) {
                            return entries.contains(key) ? &entries.at(key).read() : nullptr;
                        });
                    writeSocket();
                }
                else
                {
                    readVal<size_t>([this, self, key = std::move(key)](size_t bufSize) {
                        readVal<char>(bufSize, [this, self, key = std::move(key)](std::vector<char> buf) {
                            std::string newVal(buf.begin(), buf.end());
                            m_selectedEntry = ConfigReader::inst().safeWrite<Entry*>([&](EntriesContainer& entries) {
                                if (!entries.contains(key))
                                {
                                    entries.emplace(key, newVal);
                                    return &entries.at(key);
                                }
                                    return &entries.at(key).write(newVal);
                                });
                            writeSocket();
                        });
                    });
                }
            });
        });
    });
}

void Session::writeSocket()
{
    StatPrinter::incStat();
    auto self = shared_from_this();
    if (!m_selectedEntry)
    {
        boost::asio::async_write(
            m_socket, boost::asio::buffer({false}, sizeof(bool)),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (ec)
                    return;
                readSocket();
            });
        return;
    }

    std::vector<uint8_t> data;
    const std::string& val = m_selectedEntry->getVal();
    size_t valSize = val.size();
    data.reserve(sizeof(bool) + sizeof(size_t) + valSize * sizeof(char) + sizeof(m_selectedEntry->getReads()) + sizeof(m_selectedEntry->getWrites()));
    // [bool isSuccess] [len of getVal()] [string bytes from getVal()] [uint32_t reads] [uint32_t writes]

    addToBuf(data, true);
    addToBuf(data, valSize);
    data.insert(data.end(), val.begin(), val.end());
    addToBuf(data, m_selectedEntry->getReads());
    addToBuf(data, m_selectedEntry->getWrites());
    boost::asio::async_write(
        m_socket, boost::asio::buffer(data, data.size() * sizeof(char)),
        [this, self](boost::system::error_code ec, std::size_t) {
            if (ec)
            {
                m_socket.close();
                return;
            }
            readSocket();
        });
    m_selectedEntry = nullptr;
}
