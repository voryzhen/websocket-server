#pragma once

#include "headers.h"

class server;

void fail(boost::beast::error_code ec, char const* what);

class session : public std::enable_shared_from_this<session> {

    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    boost::beast::flat_buffer buffer_;
    server* s_;

    std::string parse_cmd(std::string& data);
    void run_cmd(std::string& data);

public:
    
    explicit session(boost::asio::ip::tcp::socket&& socket, server* s) : ws_(std::move(socket)), s_(s) {}

    void run();
    void on_run();
    void on_accept(boost::beast::error_code ec);
    void do_read();
    void on_read(boost::beast::error_code ec, std::size_t bytes_transferred);
    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred);

    void send_response();

};
