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

// ------------------------------------------
class Listener: public std::enable_shared_from_this<Listener> {
public:
    Listener(net::io_context &ioc, tcp::endpoint endpoint)
            : io_context(ioc), acceptor(ioc) {}
            void run(){}
private:
    net::io_context& io_context;
    tcp::acceptor acceptor;
};
// ------------------------------------------
class RVWSServer
{
public:
    RVWSServer(std::string host, unsigned short port, int num_threads = 1)
    {
        m_address = net::ip::make_address(host);
        m_port = port;
        m_threads = std::max<int>(1, num_threads);
    }

    void start()
    {
        net::io_context ioc{ m_threads };
        std::make_shared<Listener>(ioc, tcp::endpoint{ m_address, m_port })->run();

        std::vector<std::thread> vThreads;
        vThreads.reserve(m_threads - 1);

        for (auto i = 0; i < m_threads - 1; ++i)
        {
            vThreads.emplace_back([&ioc] {
                std::thread::id threadId = std::this_thread::get_id();
                std::cout << "Running io_context in thread: " << threadId << "\n";
                ioc.run();
            });
        }

        std::thread::id threadId = std::this_thread::get_id();
        std::cout << "Running io_context in thread: " << threadId << "\n";

        ioc.run();
    }

private:
    boost::asio::ip::address m_address;
    unsigned short m_port;
    int m_threads;
};
// ------------------------------------------

int main(int argc, char** argv)
{
    //std::unique_ptr<RVWSServer>(new RVWSServer("127.0.0.1", 3030))->start();
    std::make_unique<RVWSServer>("127.0.0.1", 3030)->start();
    return 0;
}