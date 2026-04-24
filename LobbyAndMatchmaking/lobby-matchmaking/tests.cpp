#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "LobbyServer.h"
#include "Client.h"
#include <thread>
#include <chrono>

using boost::asio::ip::tcp;
using namespace std::chrono_literals;

// ============================================================================
// Helper: read a newline-delimited message from a TCP socket
// ============================================================================
static std::string read_line(tcp::socket& sock) {
    boost::asio::streambuf buf;
    boost::asio::read_until(sock, buf, '\n');
    std::istream is(&buf);
    std::string line;
    std::getline(is, line);
    // Strip trailing \r if present
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    return line;
}

// Helper: write a newline-delimited message to a TCP socket
static void write_line(tcp::socket& sock, const std::string& msg) {
    std::string data = msg + "\n";
    boost::asio::write(sock, boost::asio::buffer(data));
}

TEST_SUITE("Multi-Client Support") {
    TEST_CASE("Server handles 10 simultaneous clients") {
        boost::asio::io_context server_io;
        TcpChatServer server(server_io, 0);
        auto server_port = server.port();

        std::thread server_thread([&]() {
            server.start();
            server_io.run_for(4s);
        });

        std::this_thread::sleep_for(100ms);

        boost::asio::io_context client_io;
        tcp::socket c1(client_io), c2(client_io), c3(client_io), c4(client_io),
        c5(client_io), c6(client_io), c7(client_io), c8(client_io), c9(client_io), c10(client_io);

        tcp::endpoint server_ep(
            boost::asio::ip::make_address("127.0.0.1"),
            server_port
        );

        // Connect all
        c1.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c2.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c3.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c4.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c5.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c6.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c7.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c8.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c9.connect(server_ep);
        std::this_thread::sleep_for(500ms);
        c10.connect(server_ep);
        std::this_thread::sleep_for(1000ms);

        // All should be connected
        CHECK(c1.is_open());
        CHECK(c2.is_open());
        CHECK(c3.is_open());
        CHECK(c4.is_open());
        CHECK(c5.is_open());
        CHECK(c6.is_open());
        CHECK(c7.is_open());
        CHECK(c8.is_open());
        CHECK(c9.is_open());
        CHECK(c10.is_open());

        std::this_thread::sleep_for(5000ms);

        c1.close();
        c2.close();
        c3.close();
        c4.close();
        c5.close();
        c6.close();
        c7.close();
        c8.close();
        c9.close();
        c10.close();
        server_io.stop();
        server_thread.join();
    }
}