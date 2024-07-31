set(HEADERS
    include/base_server/epoll_server.hpp
    include/instruments/socket.hpp)

set(SOURCES
    src/epoll_server.cpp
    src/socket.cpp)

add_library(echo_server_common STATIC ${SOURCES} ${HEADERS})
target_include_directories(echo_server_common PUBLIC include)
