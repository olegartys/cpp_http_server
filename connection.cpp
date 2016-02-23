//
// Created by olegartys on 21.02.16.
//

#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <event2/event.h>
#include <thread>
#include "connection.h"

connection::connection(evutil_socket_t client_sock, thread_pool& pool) :
        client_sock(client_sock),
        pool(pool)
{
    std::cout << "Connection created!\n";

    // Устанавливаем нонблок
    if (evutil_make_socket_nonblocking(client_sock) < 0) {
        close(client_sock);
        throw std::runtime_error("failed to set client socket to non-blocking");
    }

    // Создаём буфер
    if ((output_buffer = evbuffer_new()) == nullptr) {
        close(client_sock);
        throw std::runtime_error("client output buffer allocation failed");
    }

    // Создаём цикл событий для клиента
    if ((evbase = event_base_new()) == nullptr) {
        close(client_sock);
        throw std::runtime_error("client event_base creation failed");
    }

    // создаём буфер события
    buf_ev = bufferevent_socket_new(evbase, client_sock, BEV_OPT_CLOSE_ON_FREE);
    if (buf_ev == nullptr) {
        close(client_sock);
        throw std::runtime_error("client buffer_event creation failed");
    }

    // устанавливаем колбеки на это событие FIXME: NULLPTR
    bufferevent_setcb(buf_ev, buff_on_read, buff_on_write, buff_on_err, nullptr);
    // Включаем их (?) - так надо, чтобы работало
    bufferevent_enable(buf_ev, EV_READ);
}

// выполняется в новом потоке
void connection::on_start(connection* c) {
    event_base_dispatch(c->evbase); // запускаем цикл обработки событий
    close(c->client_sock); // как только но кончился - закрываем сокет и т.д.
    std::cout << "Connection stoped!\n";
}

void connection::start() {
    // добавляем в пул потоков задачу
    pool.runAsync(on_start, this);
    //on_start(this);
    std::cout << "Connection running!\n";
}

void connection::buff_on_read(struct bufferevent *bev, void *ctx) {
    //bev.
    std::cout << "Sth read\n";
}

void connection::buff_on_write(struct bufferevent *bev, void *ctx) {
    std::cout << "Sth wrote\n";
}

void connection::buff_on_err(struct bufferevent *bev, short what, void *ctx) {
    std::cout << "Sth err\n";
}