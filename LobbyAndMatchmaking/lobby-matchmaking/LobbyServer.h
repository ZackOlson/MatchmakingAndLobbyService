#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>

inline int run_chat_server(uint16_t port) {
    try {
        //boost::asio::io_context io_context;
        //TcpChatServer server(io_context, port);

        std::cout << "[Server] Listening on port " << port << "...\n";
        std::cout << "[Server] Press Ctrl+C to stop.\n\n";

        //server.start();

        // Run the io_context (or implement your own threading model)
        //io_context.run();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Server] Error: " << e.what() << "\n";
        return 1;
    }
}