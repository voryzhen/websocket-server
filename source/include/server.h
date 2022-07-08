#pragma once

#include <boost/beast/core.hpp>
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
#include <vector>

#include "session.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const* what);

// Accepts incoming connections and launches the sessions
class server : public std::enable_shared_from_this<server>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    server(net::io_context& ioc, tcp::endpoint endpoint);
    // Start accepting incoming connections
    void run();
private:
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
};
