//
// Created by olegartys on 23.02.16.
//

#include "response.h"

const std::string response::ok = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
const std::string response::not_found = "HTTP/1.0 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n";
