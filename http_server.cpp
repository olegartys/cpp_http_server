//
// Created by olegartys on 21.02.16.
//

#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>

#include <event2/util.h>
#include <unistd.h>
#include <iostream>

#include "http_server.h"


http_server::http_server(const std::string &address, std::size_t port, std::size_t thread_pool_size):
    address(address),
    port(port),
    thread_pool_size(thread_pool_size)
{
    pool = std::unique_ptr<thread_pool>(new thread_pool(thread_pool_size)); // создаём пул потоков
    try {
        init_socket_and_listen(); // инициализируем серверный сокет, биндимся и начинаем слушать
        init_event_loop_for_accept(); // инициализируем цикл, принимающий соединения
    } catch (std::runtime_error&) {
        throw;
    }
}

void http_server::run() {
    std::cout << "I am running!\n";
    event_base_dispatch(evbase_accept); // запскаем цикл, принимающий соединения
}

void http_server::init_event_loop_for_accept() {
    // инициализиурем базу цикла
    if ((evbase_accept = event_base_new()) == nullptr) {
        close(master_sock);
        throw std::runtime_error("Unable to create event loop for listen socket!");
    }

    // создаём ивент на покдлючение к слушающему сокету
    ev_accept = event_new(evbase_accept, master_sock, EV_READ | EV_PERSIST, on_accept, this);
    if (ev_accept == nullptr) {
        close(master_sock);
        throw std::runtime_error("Can't create event for accept!");
    }

    // добавляём ивент в цикл
    event_add(ev_accept, nullptr);
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

void http_server::on_accept(evutil_socket_t fd, short flags, void *arg) {
    http_server* _this = static_cast<http_server*>(arg); // мы передавали в эту статическую функцию this вызывающего класса

    evutil_socket_t client_sock = accept(fd, nullptr, 0);

    std::cout << "Accepted connection! fd = " << client_sock << std::endl;

    //std::unique_ptr<connection> conn(new connection(client_sock, *(static_cast<thread_pool*>(arg))));
    //auto conn = new connection(client_sock, *(static_cast<thread_pool*>(arg)));
    _this->conn = std::unique_ptr<connection>(new connection(client_sock, *(_this->pool.get())));
    _this->conn->start();
}

http_server::~http_server() {

    event_base_free(evbase_accept);
    evbase_accept = nullptr;

    close(master_sock);

}
