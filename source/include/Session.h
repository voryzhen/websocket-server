#pragma once

#include <boost/beast/core.hpp>
#include <memory>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class Session: public std::enable_shared_from_this<Session>
{
public:
    explicit Session(tcp::socket&& socket) : ws(std::move(socket)) {}

    void run()
    {
        net::dispatch(
                ws.get_executor(),
                beast::bind_front_handler(&Session::on_run, shared_from_this())
        );
    }

    void on_run()
    {
        ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

        ws.set_option(websocket::stream_base::decorator(
                [](websocket::response_type & res)
                {
                    res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
                }
        ));

        ws.async_accept(beast::bind_front_handler(&Session::on_accept, shared_from_this()));
    }

    void on_accept(beast::error_code ec)
    {
        if (ec)
        {
            std::cerr << "accept: " << ec.message() << "\n";
            return;
        }

        do_read();
    }

    void do_read()
    {
        ws.async_read(buffer, beast::bind_front_handler(&Session::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t bytes_transfered)
    {
        boost::ignore_unused(bytes_transfered);

        if (ec == websocket::error::closed)
            return;

        if (ec)
        {
            std::cerr << "read: " << ec.message() << "\n";
            return;
        }

        ws.text(ws.got_text());
        ws.async_write(buffer.data(), beast::bind_front_handler(&Session::on_write, shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t bytes_transfered)
    {
        boost::ignore_unused(bytes_transfered);

        if (ec)
        {
            std::cerr << "write: " << ec.message() << "\n";
            return;
        }

        buffer.consume(buffer.size());

        do_read();
    }

private:
    websocket::stream<beast::tcp_stream> ws;
    beast::flat_buffer buffer;
};