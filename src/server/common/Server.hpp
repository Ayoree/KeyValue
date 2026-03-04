#pragma once

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context, unsigned short port);

private:
    void do_accept();

    tcp::acceptor acceptor_;
};