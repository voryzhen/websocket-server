#pragma once

#include "server.h"

#include "session.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

server::server(net::io_context& ioc, tcp::endpoint endpoint) : ioc_(ioc), acceptor_(ioc), queue_timer_(ioc, boost::posix_time::seconds(1)) {

    queue_timer_.async_wait(boost::bind(&server::check_queue_handler, this));

    beast::error_code ec;

    acceptor_.open(endpoint.protocol(), ec);
    if(ec) {
        fail(ec, "open");
        return;
    }

    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if(ec) {
        fail(ec, "set_option");
        return;
    }

    acceptor_.bind(endpoint, ec);
    if(ec) {
        fail(ec, "bind");
        return;
    }

    acceptor_.listen(net::socket_base::max_listen_connections, ec);

    if(ec) {
        fail(ec, "listen");
        return;
    }
}

void server::check_queue_handler() {
    check_queue();
    queue_timer_.expires_at(queue_timer_.expires_at() + boost::posix_time::seconds(1));
    queue_timer_.async_wait(boost::bind(&server::check_queue_handler, this));
}

void server::run() { do_accept(); }

void server::do_accept() {
    acceptor_.async_accept(net::make_strand(ioc_), beast::bind_front_handler(&server::on_accept, shared_from_this()));
}

void server::on_accept(beast::error_code ec, tcp::socket socket) {
    if(ec) {
        fail(ec, "accept");
    } else {
        std::make_shared<session>(std::move(socket))->run();
    }

    do_accept();
}

void server::check_queue() {
    std::cout << "queue size: " << cmd_queue_.size() << std::endl;
}
