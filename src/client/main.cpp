using boost::asio::ip::tcp;

struct Entry // hardcoded
{
    std::string val;
    uint32_t reads;
    uint32_t writes;
};

constexpr std::array<const char*, 21> HARDCODED_KEYS = {
    "long",
    "key12",
    "key11",
    "key10",
    "key9",
    "tree",
    "key",
    "lol",
    "idk",
    "key7",
    "counter",
    "hire",
    "key4",
    "key8",
    "key2",
    "key5",
    "key3",
    "key6",
    "SpecialnotFoundKey",
    "AnotherNotFoundKey",
    "YetAnotherUnexistedKey",
};

bool isTerminateCode(boost::system::error_code err)
{
    if (err == boost::asio::error::eof) {
        std::println("connection closed");
        return true;
    }
    else if (err)
        throw boost::system::system_error(err);
    return false;
}

bool isBadRead(const size_t& readedBytes, const size_t& expectedBytes, const boost::system::error_code ec)
{
    if (isTerminateCode(ec)) [[unlikely]]
        return true;
    if (readedBytes != expectedBytes) [[unlikely]]
        return true;
    return false;
}

bool process(const std::string& str, tcp::socket& socket) {
    const auto eqPos = str.find('=');
    bool isGetCommand = eqPos == std::string::npos ? true : false;
    
    // [bool isGetCommand] [len of key] [string bytes from key] ([len of value] [string bytes from value])
    boost::asio::write(socket, boost::asio::buffer(&isGetCommand, sizeof(isGetCommand)));
    if (isGetCommand)
    {
        size_t strLen = str.length();
        boost::asio::write(socket, boost::asio::buffer(&strLen, sizeof(strLen)));
        boost::asio::write(socket, boost::asio::buffer(str));
    }
    else
    {
        size_t keyLen = eqPos;
        size_t valLen = str.length() - keyLen - 1;
        boost::asio::write(socket, boost::asio::buffer(&keyLen, sizeof(keyLen)));
        boost::asio::write(socket, boost::asio::buffer(std::string_view{str.data(), keyLen}));
        boost::asio::write(socket, boost::asio::buffer(&valLen, sizeof(valLen)));
        boost::asio::write(socket, boost::asio::buffer(std::string_view{str.data() + keyLen + 1, valLen}));
    }

    boost::system::error_code error;
    bool isSucces = false;

    {
        size_t readed = socket.read_some(boost::asio::buffer(&isSucces, sizeof(isSucces)), error);
        if (isBadRead(readed, sizeof(isSucces), error)) [[unlikely]]
            return false;
    }

    if (!isSucces)
    {
        std::println("Key not found\n");
        return false;
    }

    size_t valueLen;
    {
        size_t readed = socket.read_some(boost::asio::buffer(&valueLen, sizeof(valueLen)), error);
        if (isBadRead(readed, sizeof(valueLen), error)) [[unlikely]]
            return false;
    }
    
    Entry message;
    message.val.resize(valueLen);
    {
        size_t readed = socket.read_some(boost::asio::buffer(message.val.data(), valueLen * sizeof(char)), error);
        if (isBadRead(readed, valueLen * sizeof(char), error)) [[unlikely]]
            return false;
    }
    {
        size_t readed = socket.read_some(boost::asio::buffer(&message.reads, sizeof(message.reads)), error);
        if (isBadRead(readed, sizeof(message.reads), error)) [[unlikely]]
            return false;
    }

    {
        size_t readed = socket.read_some(boost::asio::buffer(&message.writes, sizeof(message.writes)), error);
        if (isBadRead(readed, sizeof(message.writes), error)) [[unlikely]]
            return false;
    }

    std::print("{}={}\n", str.substr(0, eqPos), message.val);
    std::print("reads={}\n", message.reads);
    std::println("writes={}\n", message.writes);
    return true;
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::println(stderr, "Usage: <host> <port> [isAuto (0,1)]");
        return 1;
    }
    bool isAuto = false;
    if (argc >= 4)
    isAuto = atoi(argv[3]);
    
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    tcp::socket socket(io_context);
    
    bool isFinished = false;
    do
    {
        try {
            boost::asio::connect(socket, endpoints);
            std::println("Connected to {}:{}", argv[1], argv[2]);

            if (!isAuto)
            {
                for (std::string line; std::cin >> std::ws && std::getline(std::cin >> std::ws, line);)
                {
                    if (line == ":q")
                        break;
                    process(line, socket);
                    
                }
            }
            else
            {
                std::srand(std::time(nullptr));
                for (size_t i = 0; i < 10'000; ++i)
                {
                    std::string input = HARDCODED_KEYS[std::rand() % HARDCODED_KEYS.size()];

                    if (std::rand() % 100 < 1)
                    {
                        std::stringstream ss;
                        ss << '=';
                        size_t newValLen = std::rand() % 100 + 1;
                        for (size_t j = 0; j < newValLen; ++j)
                            ss << (std::rand() % ('A' - 'z')) + 'A';
                        input += ss.str();
                    }

                    std::print("input: {}\n", input);
                    process(input, socket);
                }
            }

            socket.close();
            isFinished = true;
        } catch (std::exception& e) {
            std::println(stderr, "Exception: {}", e.what());
            std::this_thread::sleep_for(std::chrono::seconds{5});
        }
    } while (!isFinished);

    return 0;
}
