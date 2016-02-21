//
// Created by olegartys on 21.02.16.
//

#ifndef CPP_HTTP_SERVER_HTTP_SERVER_H
#define CPP_HTTP_SERVER_HTTP_SERVER_H

#include <string>
#include <cstdint>
#include <event2/util.h>
#include <event2/event_struct.h>

class http_server final {
public:
    http_server(const std::string& address, std::size_t port, std::size_t thread_pool_size);

    void run() const;

private:

    //
    void init_socket_and_listen();

    //
    evutil_socket_t master_sock;
    sockaddr_in master_addr;
    event* ev_accept;

    //
    std::string address;
    std::size_t port;
    std::size_t thread_pool_size;
};


#endif //CPP_HTTP_SERVER_HTTP_SERVER_H
