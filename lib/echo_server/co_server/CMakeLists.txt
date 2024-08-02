set(HEADERS
    include/co_instruments/awaiter.hpp
    include/co_instruments/socket_awaiter.hpp
    include/co_instruments/co_socket_handle.hpp
    include/co_server/co_echo_socket.hpp
    include/co_server/co_listener_socket.hpp
    include/co_server/co_server.hpp)
set(SOURCES
    src/co_echo_socket.cpp
    src/co_listener_socket.cpp
    src/co_server.cpp)

add_library(co_server STATIC ${SOURCES} ${HEADERS})
target_include_directories(co_server PUBLIC include)
target_link_libraries(co_server PUBLIC echo_server_common)