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
#include "Session.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class Listener: public std::enable_shared_from_this<Listener> {
public:
    Listener(net::io_context &ioc, const tcp::endpoint& endpoint)
        : m_io_context(ioc), m_acceptor(ioc)
    {
        beast::error_code ec;

        m_acceptor.open(endpoint.protocol(), ec);

        if (ec)
        {
            std::cerr << "open: " << ec.message() << "\n";
            return;
        }

        m_acceptor.set_option(net::socket_base::reuse_address(true), ec);

        if (ec)
        {
            std::cerr << "set_option: " << ec.message() << "\n";
            return;
        }

        m_acceptor.bind(endpoint, ec);

        if (ec)
        {
            std::cerr << "bind: " << ec.message() << "\n";
            return;
        }

        m_acceptor.listen(net::socket_base::max_listen_connections, ec);

        if (ec)
        {
            std::cerr << "listen: " << ec.message() << "\n";
            return;
        }
    }

    void run()
    {
        do_accept();
    }

private:

    net::io_context& m_io_context;
    tcp::acceptor m_acceptor;

    void do_accept()
    {
        m_acceptor.async_accept(net::make_strand(m_io_context),
                beast::bind_front_handler(&Listener::on_accept, shared_from_this()));
    }

    void on_accept(beast::error_code ec, tcp::socket socket)
    {
        if (ec)
        {
            std::cerr << "on accept: " << ec.message() << "\n";
            return;
        }
        else
        {
            std::make_shared<Session>(std::move(socket))->run();
        }

        do_accept();
    }

};