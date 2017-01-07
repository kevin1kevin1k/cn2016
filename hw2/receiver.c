#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UDP.h"

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
    create_socket(&listen_fd, &server, 9487);
    
    struct sockaddr_in agent;
    set_addr(&agent, agent_ip, atoi(agent_port));

    char buf[PAYLOAD+1];
    while (1) {
        memset(buf, 0, PAYLOAD);
        my_recv(listen_fd, buf, &client);
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