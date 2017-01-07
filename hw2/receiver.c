#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_LEN 100
#define PAYLOAD 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s agent_ip agent_port output_path\n", argv[0]);
        exit(1);
    }
    
    char agent_ip[MAX_LEN], agent_port[MAX_LEN], output_path[MAX_LEN];
    strcpy(agent_ip, argv[1]);
    strcpy(agent_port, argv[2]);
    strcpy(output_path, argv[3]);
    
    FILE *fout = fopen(output_path, "wb");
    if (fout == NULL) {
        printf("Open output file error\n");
        exit(1);
    }
    
    int listen_fd;
    struct sockaddr_in server, client;
    listen_fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(9487);
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
        socklen_t len = sizeof(agent);
        recvfrom(listen_fd, buf, PAYLOAD, 0, (struct sockaddr *)&client, &len);
        printf("recv\tdata\n");
        if (!strcmp(buf, "fin")) {
            break;
        }
        fwrite(buf, 1, strlen(buf), fout);
    }
    close(listen_fd);
    fclose(fout);

    return 0;
}