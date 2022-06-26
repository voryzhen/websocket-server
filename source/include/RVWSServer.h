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
#include "Listener.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class RVWSServer
{
public:
    RVWSServer(const std::string& host, unsigned short port, int num_threads = 1)
    {
        m_address = net::ip::make_address(host);
        m_port = port;
        m_threads = std::max<int>(1, num_threads);
    }

    void start()
    {
        net::io_context ioc{ m_threads };
        std::make_shared<Listener>(ioc, tcp::endpoint{ m_address, m_port })->run();

        std::vector<std::thread> v_threads;
        v_threads.reserve(m_threads - 1);

        for (auto i = 0; i < m_threads - 1; ++i)
        {
            v_threads.emplace_back([&ioc] {
                std::thread::id thread_id = std::this_thread::get_id();
                std::cout << "Running io_context in thread: " << thread_id << "\n";
                ioc.run();
            });
        }

        std::thread::id thread_id = std::this_thread::get_id();
        std::cout << "Running io_context in thread: " << thread_id << "\n";

        ioc.run();
    }

private:
    boost::asio::ip::address m_address;
    unsigned short m_port;
    int m_threads;
};
