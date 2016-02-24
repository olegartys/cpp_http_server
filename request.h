//
// Created by olegartys on 23.02.16.
//

#ifndef CPP_HTTP_SERVER_request_H
#define CPP_HTTP_SERVER_request_H

#include <string>
#include <vector>

#include "header.h"

struct request final {
    std::string method;
    std::string uri;
    std::vector<header> headers;
};


#endif //CPP_HTTP_SERVER_request_H
