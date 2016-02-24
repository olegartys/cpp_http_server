#include <getopt.h>
#include <unistd.h>
#include <linux/limits.h>
#include <fstream>
#include "http_server.h"

static const size_t thread_pool_size = 3;

std::ofstream log("/tmp/server_log");

int main(int argc, char** argv) {
    // Парсим аргументы...
    if (argc != 7) {
        log << "wrong usage!\n";
        return 1;
    }

    std::string address;
    std::size_t port = 12341;

    int res = 0;
    while ((res = getopt(argc, argv, "h:p:d:")) != -1) {
        switch(res) {
            case 'h':
                address = optarg;
                break;

            case 'p':
                //port = atoll(optarg);
                break;

            case 'd':
                chdir(optarg);
                chroot(optarg);
                break;
        }
    }

    log << address << " " << port << " " << get_current_dir_name() << std::endl;

    http_server server(address, port, thread_pool_size);
    server.run();

    return 0;
}