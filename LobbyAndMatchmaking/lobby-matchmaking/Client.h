#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <string>

inline int run_chat_client(const std::string& host, uint16_t port) {
    try {
        //boost::asio::io_context io_context;
        //TcpChatClient client(io_context);

        std::cout << "[Client] TCP Chat Client\n";
        std::cout << "[Client] Connecting to " << host << ":" << port << "...\n";

        /*if (!client.connect(host, port)) {
            std::cerr << "[Client] Failed to connect to server\n";
            return 1;
        }

        std::cout << "[Client] Connected!\n";

        // Get username from user
        std::string username;
        std::cout << "Enter your username: ";
        std::getline(std::cin, username);

        if (username.empty()) {
            username = "anonymous";
        }

        client.set_username(username);
        std::cout << "[Client] Joined as '" << username << "'\n";
        std::cout << "[Client] Type messages and press Enter to send.\n";
        std::cout << "[Client] Type /quit to exit.\n\n";

        client.run();*/

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "[Client] Error: " << e.what() << "\n";
        return 1;
    }
}