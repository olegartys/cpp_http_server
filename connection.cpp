//
// Created by olegartys on 21.02.16.
//

#include <iostream>
#include <unistd.h>
#include "connection.h"

connection::connection(evutil_socket_t sock, thread_pool& pool) :
        sock(sock),
        pool(pool)
{
    std::cout << "Connection created!\n";
}

void connection::start() {
    std::cout << "Connection running!\n";
}


connection::~connection() {
    close(sock);
    std::cout << "Connection stoped!\n";
}