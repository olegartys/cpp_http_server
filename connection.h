//
// Created by olegartys on 21.02.16.
//

#ifndef CPP_HTTP_SERVER_CONNECTION_H
#define CPP_HTTP_SERVER_CONNECTION_H

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <http_parser.h>

#include "thread_pool.h"
#include "request.h"

// Этот класс представляет собой соединение (грубо говоря, клиент)
class connection final {
public:
    explicit connection(evutil_socket_t sock, thread_pool& pool);

    void start();

    // Запрещаем семантику копирования
    connection(const connection&) = delete;
    connection& operator= (const connection&) = delete;

private:
    static void buff_on_read(struct bufferevent *bev, void *ctx);
    static void buff_on_write(struct bufferevent *bev, void *ctx);
    static void buff_on_err(struct bufferevent *bev, short what, void *ctx);

    static void on_start(connection* c);

    evutil_socket_t client_sock;

    thread_pool pool;

    event_base* evbase;
    evbuffer* output_buffer;
    bufferevent* buf_ev;

    http_parser parser;

    request client_request;
};


#endif //CPP_HTTP_SERVER_CONNECTION_H
