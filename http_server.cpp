//
// Created by olegartys on 21.02.16.
//

#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>

#include <event2/util.h>

#include "http_server.h"

http_server::http_server(const std::string &address, std::size_t port, std::size_t thread_pool_size) {
    this->address = address;
    this->port = port;
    this->thread_pool_size = thread_pool_size;

    // инициализируем серверный сокет, биндимся и начинаем слушать
    try {
        init_socket_and_listen();
    } catch (std::runtime_error&) {
        throw;
    }


}

void http_server::run() const {

}

void http_server::init_socket_and_listen() {
    // создаём сокет
    master_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (master_sock < 0) {
        throw std::runtime_error("Can't open master socket!");
    }

    // Инициализируем его
    memset(&master_addr, 0, sizeof(master_addr));
    master_addr.sin_family = AF_INET;

    in_addr anp;
    if (inet_aton(address.c_str(), &anp) < 0) {
        throw std::runtime_error("Can't tranlaste IP into uint32!");
    }

    master_addr.sin_addr.s_addr = anp.s_addr;
    master_addr.sin_port = htons(port);

    // Биндим
    if (bind(master_sock, (struct sockaddr *)&master_addr, sizeof(master_addr)) < 0) {
        throw std::runtime_error("Bind failed!");
    }

    // Слушаем
    if (listen(master_sock, SOMAXCONN) < 0) {
        throw std::runtime_error("Listen failed!");
    }

    // Устанавливаем флаги на сокет
    int reuseaddr = 1;
    setsockopt(master_sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int));

    // Делаем его неблокирующим
    if (evutil_make_socket_nonblocking(master_sock) < 0) {
        throw std::runtime_error("Failed to set server socket to non-blocking!");
    }
}
