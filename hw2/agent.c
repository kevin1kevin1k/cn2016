#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

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
    listen_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(7487);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listen_fd, (struct sockaddr *)&server, sizeof(server));
    
    struct sockaddr_in sender;
    memset(&sender, 0, sizeof(sender));
    sender.sin_family = AF_INET;
    sender.sin_port = htons(atoi(sender_port));
    inet_pton(AF_INET, sender_ip, &sender.sin_addr);
    
    struct sockaddr_in receiver;
    memset(&receiver, 0, sizeof(receiver));
    receiver.sin_family = AF_INET;
    receiver.sin_port = htons(atoi(receiver_port));
    inet_pton(AF_INET, receiver_ip, &receiver.sin_addr);

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