#include "server.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

int main(int argc, char* argv[]) {

    /*
    if (argc != 4)
    {
        std::cerr <<
                  "Usage: websocket-server-async <address> <port> <threads>\n" <<
                  "Example:\n" <<
                  "    websocket-server-async 0.0.0.0 8080 1\n";
        return EXIT_FAILURE;
    }

    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const threads = std::max<int>(1, std::atoi(argv[3]));
    */

    auto const address = net::ip::make_address("127.0.0.1");
    const unsigned short port = 3030;
    auto const threads = 1;

    net::io_context ioc{threads};
    std::make_shared<server>(ioc, tcp::endpoint{address, port})->run();

    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back([&ioc] { ioc.run(); });

    ioc.run();

    return EXIT_SUCCESS;
}
