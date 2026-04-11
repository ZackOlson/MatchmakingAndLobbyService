#include "LobbyServer.h"

int main(int argc, char* argv[]) {
    uint16_t port = (argc > 1) ? static_cast<uint16_t>(std::stoi(argv[1])) : 9999;
    return run_chat_server(port);
}