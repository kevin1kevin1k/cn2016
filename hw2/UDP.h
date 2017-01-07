#include <arpa/inet.h>
#include <unistd.h>

void set_addr(struct sockaddr_in *addr, char *ip, int port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_pton(AF_INET, ip, &(addr->sin_addr));
}

void create_socket(int *fd, struct sockaddr_in *addr, int port) {
    *fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&addr, 0, sizeof(addr));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    bind(*fd, (struct sockaddr *)addr, sizeof(addr));
}
