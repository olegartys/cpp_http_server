//
// Created by olegartys on 21.02.16.
//

#ifndef CPP_HTTP_SERVER_THREAD_POOL_H
#define CPP_HTTP_SERVER_THREAD_POOL_H


#include <bits/c++config.h>

class thread_pool final {
public:
    explicit thread_pool(std::size_t thread_pool_size);

private:
    std::size_t thread_pool_size;

};


#endif //CPP_HTTP_SERVER_THREAD_POOL_H
