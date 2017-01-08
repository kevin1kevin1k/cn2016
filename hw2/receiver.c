#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UDP.h"

#define BUFFER_SIZE 5 // should be 32

int recved[1000000];
char buffer[BUFFER_SIZE][PAYLOAD+1];

void flush(int n, FILE *f) {
    for (int i = 0; i < n; i++) {
        fwrite(buffer[i], 1, strlen(buffer[i]), f);
    }
    printf("flush\n");
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s agent_ip agent_port output_path\n", argv[0]);
        exit(1);
    }
    
    char agent_ip[MAX_LEN], agent_port[MAX_LEN], output_path[MAX_LEN];
    strcpy(agent_ip, argv[1]);
    strcpy(agent_port, argv[2]);
    strcpy(output_path, argv[3]);
    
    FILE *output = fopen(output_path, "wb");
    if (output == NULL) {
        printf("Open output file error\n");
        exit(1);
    }
    
    int listen_fd;
    struct sockaddr_in server, client;
    create_socket(&listen_fd, &server, 9487);
    
    struct sockaddr_in agent;
    set_addr(&agent, agent_ip, atoi(agent_port));

    int left = 1;
    int cumu = 0;
    while (1) {
        Packet pkt;
        my_recv(listen_fd, &pkt, &client);
        if (pkt.type == DATA) {
            if (pkt.seq < left + BUFFER_SIZE) {
                if (recved[pkt.seq] > 0) {
                    printf("ignr\tdata\t#%d\n", pkt.seq);
                }
                else {
                    printf("recv\tdata\t#%d\n", pkt.seq);
                    cumu++;
                }
                Packet ack = {ACK, pkt.seq, ""};
                my_send(listen_fd, &ack, &agent);
                printf("send\tack\t#%d\n", pkt.seq);
                recved[pkt.seq]++;
                
                strcpy(buffer[pkt.seq - left], pkt.buf);
            }
            else {
                printf("drop\tdata\t#%d\n", pkt.seq);
                
                if (cumu == BUFFER_SIZE) {
                    flush(cumu, output);
                    left += BUFFER_SIZE;
                    cumu = 0;
                }
            }
        }
        else if (pkt.type == FIN) {
            printf("recv\tfin\n");
            Packet finack = {FINACK, 0, ""};
            my_send(listen_fd, &finack, &agent);
            printf("send\tfinack\n");
            break;
        }
    }

    flush(cumu, output);
    
    close(listen_fd);
    fclose(output);

    return 0;
}