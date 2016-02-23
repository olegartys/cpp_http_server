//
// Created by olegartys on 21.02.16.
//

#ifndef CPP_HTTP_SERVER_HTTP_SERVER_H
#define CPP_HTTP_SERVER_HTTP_SERVER_H

#include <string>
#include <cstdint>
#include <memory>

#include <event2/util.h>
#include <event2/event_struct.h>
#include <event2/event.h>

#include "connection.h"


class http_server final {
public:
    explicit http_server(const std::string& address, std::size_t port, std::size_t thread_pool_size);
    ~http_server();

    void run();

    // Запрещаем семантику копирования
    http_server(const http_server&) = delete;
    http_server& operator= (const http_server&) = delete;

private:

    // Инициализация
    void init_socket_and_listen();
    void init_event_loop_for_accept();

    static void on_accept(evutil_socket_t fd, short flags, void* arg);

    // Слушающий сокет и связанные с ним структуры
    evutil_socket_t master_sock;
    sockaddr_in master_addr;
    event* ev_accept;

    // Параметры сервера
    std::string address;
    std::size_t port;
    std::size_t thread_pool_size;

    // Цикл для отлавливания соединений
    event_base* evbase_accept;

    // Пул потоков
    std::unique_ptr<thread_pool> pool;

    // Соединение
    std::unique_ptr<connection> conn;
};


#endif //CPP_HTTP_SERVER_HTTP_SERVER_H
