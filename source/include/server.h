#pragma once

#include "headers.h"

void fail(boost::beast::error_code ec, char const* what);

class server : public std::enable_shared_from_this<server> {

    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;

    void do_accept();
    void on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);

    boost::asio::deadline_timer queue_timer_;

    void check_queue();
    void check_queue_handler();

    std::queue <std::string> cmd_queue_;

public:

    server(boost::asio::io_context& ioc, boost::asio::ip::tcp::endpoint endpoint);
    void run();

};
