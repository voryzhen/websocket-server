#pragma once

#include "session.h"
#include "server.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void fail(beast::error_code ec, char const* what);

void session::run() {
    net::dispatch(ws_.get_executor(),
    beast::bind_front_handler(&session::on_run, shared_from_this()));
}

void session::on_run() {
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    ws_.set_option(websocket::stream_base::decorator(
            [] (websocket::response_type& res) {
                res.set(http::field::server,std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
            }
    ));

    ws_.async_accept(beast::bind_front_handler(&session::on_accept, shared_from_this()));
}

void session::on_accept(beast::error_code ec) {
    if(ec)
        return fail(ec, "accept");
    do_read();
}

void session::do_read() {
    ws_.async_read(buffer_, beast::bind_front_handler( &session::on_read, shared_from_this()));
}

void session::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if(ec == websocket::error::closed)
        return;

    if(ec)
        return fail(ec, "read");

    ws_.text(ws_.got_text());

    std::string data(boost::asio::buffer_cast<const char*>(buffer_.data()), bytes_transferred);
    run_cmd(data);

    ws_.async_write(buffer_.data(), beast::bind_front_handler( &session::on_write, shared_from_this()));
}

void session::run_cmd(std::string& data) {
    const auto cmd = parse_cmd(data);
    s_ -> add_cmd(cmd, this);
}

std::string session::parse_cmd(std::string& data) {
    return data; // all user data is a command name for now
}

void session::on_write(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if(ec)
        return fail(ec, "write");

    buffer_.consume(buffer_.size());

    do_read();
}

void session::send_response() {
    std::cout << "asd" << std::endl;
}
