#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UDP.h"

#define MAX_LEN 1024

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s sender_ip sender_port receiver_ip receiver_port\n", argv[0]);
        exit(1);
    }
    char sender_ip[MAX_LEN], sender_port[MAX_LEN], receiver_ip[MAX_LEN], receiver_port[MAX_LEN];
    strcpy(sender_ip, argv[1]);
    strcpy(sender_port, argv[2]);
    strcpy(receiver_ip, argv[3]);
    strcpy(receiver_port, argv[4]);

    char buf[1024];
    int listen_fd;
    struct sockaddr_in server, client;
    create_socket(&listen_fd, &server, 7487);
    
    struct sockaddr_in sender, receiver;
    set_addr(&sender, sender_ip, atoi(sender_port));
    set_addr(&receiver, receiver_ip, atoi(receiver_port));

    while (1) {
        socklen_t len = sizeof(client);
        memset(&buf, 0, sizeof(buf));
        recvfrom(listen_fd, buf, MAX_LEN, 0, (struct sockaddr *)&client, &len);
        printf("get\tdata\n");
        sendto(listen_fd, buf, strlen(buf), 0, (struct sockaddr *)&receiver, len);
        printf("fwd\tdata\n");
    }
    close(listen_fd);
    
    return 0;
}