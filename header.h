//
// Created by olegartys on 23.02.16.
//

#ifndef CPP_HTTP_SERVER_HEADER_H
#define CPP_HTTP_SERVER_HEADER_H

#include <string>

struct header final {
    std::string name;
    std::string value;
};


#endif //CPP_HTTP_SERVER_HEADER_H
