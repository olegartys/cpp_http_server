//
// Created by olegartys on 21.02.16.
//

#ifndef CPP_HTTP_SERVER_CONNECTION_H
#define CPP_HTTP_SERVER_CONNECTION_H

#include <event2/util.h>
#include "thread_pool.h"

// Этот класс представляет собой соединение (грубо говоря, клиент)
class connection final {
public:
    explicit connection(evutil_socket_t sock, thread_pool& pool);
    ~connection();

    void start();

    // Запрещаем семантику копирования
    connection(const connection&) = delete;
    connection& operator= (const connection&) = delete;

private:
    evutil_socket_t sock;
    thread_pool pool;

};


#endif //CPP_HTTP_SERVER_CONNECTION_H
