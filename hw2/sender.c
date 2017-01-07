#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LEN 100
#define PAYLOAD 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s agent_ip agent_port input_path\n", argv[0]);
        exit(1);
    }
    
    char agent_ip[MAX_LEN], agent_port[MAX_LEN], input_path[MAX_LEN];
    strcpy(agent_ip, argv[1]);
    strcpy(agent_port, argv[2]);
    strcpy(input_path, argv[3]);
    
    FILE *fin = fopen(input_path, "rb");
    if (fin == NULL) {
        printf("Open input file error\n");
        exit(1);
    }
    
    int listen_fd;
    struct sockaddr_in server, client;
    listen_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(5487);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listen_fd, (struct sockaddr *)&server, sizeof(server));
    
    struct sockaddr_in agent;
    memset(&agent, 0, sizeof(agent));
    agent.sin_family = AF_INET;
    agent.sin_port = htons(atoi(agent_port));
    inet_pton(AF_INET, agent_ip, &agent.sin_addr);

    char buf[PAYLOAD+1];
    while (1) {
        memset(buf, 0, PAYLOAD);
        int res = fread(buf, 1, PAYLOAD, fin);
        if (res == 0) {
            break;
        }
        socklen_t len = sizeof(agent);
        sendto(listen_fd, buf, sizeof(buf), 0, (struct sockaddr *)&agent, len);
        printf("send\tdata\n");
    }
    
    socklen_t len = sizeof(agent);
    sendto(listen_fd, "fin", 3, 0, (struct sockaddr *)&agent, len);

    close(listen_fd);
    fclose(fin);

    return 0;
}