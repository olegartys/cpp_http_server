//
// Created by olegartys on 21.02.16.
//

#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <event2/event.h>
#include <thread>
#include <event2/bufferevent_struct.h>
#include <string.h>
#include <fstream>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "connection.h"
#include "response.h"

extern std::ofstream log;

connection::connection(evutil_socket_t client_sock, thread_pool& pool) :
        client_sock(client_sock),
        pool(pool)
{
    log << "Connection created!\n";

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

    // устанавливаем колбеки на это событие
    bufferevent_setcb(buf_ev, buff_on_read, buff_on_write, buff_on_err, this);
    // Включаем их (?) - так надо, чтобы работало
    bufferevent_enable(buf_ev, EV_READ);
}

// выполняется в новом потоке
void connection::on_start(connection* c) {
    event_base_dispatch(c->evbase); // запускаем цикл обработки событий
    close(c->client_sock); // как только но кончился - закрываем сокет и т.д.
    log << "Connection stoped!\n";
}

void connection::start() {
    // Инициализируем парсер на парсинг реквестов
    http_parser_init(&parser, HTTP_REQUEST);

    // добавляем в пул потоков задачу
    pool.runAsync(on_start, this);

    log << "Connection running!\n";
}

// Эта штука срабатывает, когда кто-то нам пишет
void connection::buff_on_read(struct bufferevent *bev, void *ctx) {
    connection* _this = static_cast<connection*>(ctx);

    log << "Sth read\n";

    // буфер с нашими данными
    evbuffer* input = bev->input;

    // копируем всё это дело в строку
    size_t len = evbuffer_get_length(bev->input);
    char* data = new char[len];
    if ((evbuffer_copyout(input, data, len)) < 0) {
        event_base_loopbreak(_this->evbase);
    } else {
        log << "Parsing...\n";
        // ПАРСИМ (TODO: здесь в оригинале должно быть всё красиво. Нужно смотреть тип запроса, обрабатывать все заголовки, вешать нормальные колбеки и много всего.), но сейчас нам важно поолучить только URL

        _this->parser.data = _this; // устанавливаем для парсера связь с внешним миром

        http_parser_settings settings;
        memset(&settings, 0, sizeof(settings));

        // устанавливаем обработчики
        settings.on_url = [](http_parser* p, const char* at, size_t len) -> int {
            // копируем URL
            printf("Url: %.*s\n", (int)len, at);
            char* t = new char[len-1];
            memset(t, '\0', len*sizeof(char));
            strncpy(t, at+1, len-1); // len-1 т.к. там пробел
            // избавляемся от параметров
            for (char* i = t; *i; ++i) {
                if (*i == '?') { *i = '\0'; break; }
            }
            static_cast<connection*>(p->data)->client_request.uri = t;
            delete t;
            return 0;
        };

        settings.on_header_field = [](http_parser* p, const char* at, size_t len) -> int {return 0;};
        settings.on_header_value = [](http_parser* p, const char* at, size_t len) -> int {return 0;};
        settings.on_headers_complete = [](http_parser*) -> int {return 0;};

        size_t nparsed = http_parser_execute(&_this->parser, &settings, data, len);
        if (nparsed != len) {
            log << "Can't parse\n";
            event_base_loopbreak(_this->evbase);
        }

        // НА ЭТОМ ЭТАПЕ МЫ ВСЁ РАСПАРСИЛИ И ИМЕЕМ ЗАПОЛНЕННЫЙ РЕКВЕСТ (client_request)
//        _this->client_request.method = http_method_str(_this->parser.method);
        log << "URI=" << _this->client_request.uri << std::endl;

        // Пытаемся открыть требуемый файл и скрамливаем его клиенту

        int fd = open(_this->client_request.uri.c_str(), O_RDONLY);
        //вывод имени файла по байтам
//        for (auto& i: _this->client_request.uri) {
//            printf("%d ", i);
//        }
//        puts("");

        // Если не получилось - шлём ошибку
        if (fd < 0) {
            perror("KEK");
            if ((send(_this->client_sock, response::not_found.c_str(), response::not_found.length(), MSG_NOSIGNAL)) < 0) {
                log << "cant't send!!!\n";
            }
        } else {
            // если всё окей
            if ((send(_this->client_sock, response::ok.c_str(), response::ok.length(), MSG_NOSIGNAL | MSG_MORE)) < 0) {
                log << "can't send\n";
            } else {
                struct stat stat_buf;
                fstat(fd, &stat_buf);

                // Посылаем файл
                ssize_t recv = sendfile( _this->client_sock, fd, nullptr, stat_buf.st_size);
                if (recv != stat_buf.st_size) {
                    log << "file not sent at all\n";
                }
            }

        }

        event_base_loopexit(_this->evbase, nullptr);
    }
}

void connection::buff_on_write(struct bufferevent *bev, void *ctx) {
    log << "Sth wrote\n";
}

void connection::buff_on_err(struct bufferevent *bev, short what, void *ctx) {
    log << "Sth err\n";
}