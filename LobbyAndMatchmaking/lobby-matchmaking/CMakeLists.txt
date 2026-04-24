cmake_minimum_required(VERSION 3.20.6)

project(lobby-matchmaking LANGUAGES CXX)

# TCP Chat Server executable
add_executable(lobby-matchmaking-server LobbyServer.cpp)
target_compile_features(lobby-matchmaking-server PRIVATE cxx_std_23)
target_link_libraries(lobby-matchmaking-server PRIVATE Boost::asio)
if(WIN32)
    target_link_libraries(lobby-matchmaking-server PRIVATE ws2_32)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(lobby-matchmaking-server PRIVATE -Wall -Wextra -Wpedantic)
endif()

# TCP Chat Client executable
add_executable(lobby-matchmaking-client Client.cpp)
target_compile_features(lobby-matchmaking-client PRIVATE cxx_std_23)
target_link_libraries(lobby-matchmaking-client PRIVATE Boost::asio)
if(WIN32)
    target_link_libraries(lobby-matchmaking-client PRIVATE ws2_32)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(lobby-matchmaking-client PRIVATE -Wall -Wextra -Wpedantic)
endif()

# tests
add_executable(tests tests.cpp)
target_compile_features(tests PRIVATE cxx_std_23)
target_link_libraries(tests PRIVATE Boost::asio doctest::doctest)
if(WIN32)
    target_link_libraries(tests PRIVATE ws2_32)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Register with CTest
include(CTest)
add_test(NAME tests
        COMMAND tests
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
