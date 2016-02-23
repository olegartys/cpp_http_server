//
// Created by olegartys on 23.02.16.
//

#ifndef CPP_HTTP_SERVER_RESPONSE_H
#define CPP_HTTP_SERVER_RESPONSE_H

#include <string>

struct response final {
	static const std::string ok;
	static const std::string not_found;
};

#endif
