#include <iostream>
#include "http_server.h"

static const size_t thread_pool_size = 5;

int main(int argc, char** argv) {
    // Парсим аргументы...
    std::string address = "127.0.0.1";
    std::size_t port = 12347;

    http_server server(address, port, thread_pool_size);
    server.run();

    return 0;
}