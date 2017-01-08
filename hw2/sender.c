#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "UDP.h"

#ifndef TEST
#define TEST 0
#endif

int acked[1000000];

int max(int a, int b) {
    return a > b ? a : b;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s agent_ip agent_port input_path\n", argv[0]);
        exit(1);
    }
    
    char agent_ip[MAX_LEN], agent_port[MAX_LEN], input_path[MAX_LEN];
    strcpy(agent_ip, argv[1]);
    strcpy(agent_port, argv[2]);
    strcpy(input_path, argv[3]);
    
    FILE *input = fopen(input_path, "rb");
    if (input == NULL) {
        printf("Open input file error\n");
        exit(1);
    }
    
    int listen_fd;
    struct sockaddr_in server, client;
    create_socket(&listen_fd, &server, 5487);
    
    int flags = fcntl(listen_fd, F_GETFL, 0);
    fcntl(listen_fd, F_SETFL, flags | O_NONBLOCK);
    
    struct sockaddr_in agent;
    set_addr(&agent, agent_ip, atoi(agent_port));

    struct fd_set master_set, working_set;
    struct timeval timeout;
    FD_ZERO(&master_set);
    FD_SET(listen_fd, &master_set);
    
    int seq = 0;
    int winSize = 1;
    int left = 1;
    int threshold = TEST ? 2 : 16;
    int max_sent = 0;
    int cumu = 0; // cumulative counter for congestion avoidance
    int finished = 0;
    while (1) {
        if (finished && (left == max_sent + 1)) {
            Packet pkt = {FIN, 0, ""};
            my_send(listen_fd, &pkt, &agent);
            printf("send\tfin\n");
            goto WAIT;
        }
        
        for (int i = 0; i < winSize; i++) {
            seq++;
            Packet pkt = {DATA, seq, ""};
            fseek(input, PAYLOAD * (seq-1), SEEK_SET);
            int res = fread(pkt.buf, 1, PAYLOAD, input);
            if (res == 0) {
                finished = 1;
                break;
            }
            else if (res < 0) {
                printf("Error: read file\n");
                exit(1);
            }
            
            if (seq > max_sent) {
                printf("send\tdata\t#%d,\twinSize = %d\n", pkt.seq, winSize);
            }
            else {
                printf("resnd\tdata\t#%d,\twinSize = %d\n", pkt.seq, winSize);
            }
            my_send(listen_fd, &pkt, &agent);
            max_sent = max(max_sent, pkt.seq);
        }
        
        WAIT: ;
        memcpy(&working_set, &master_set, sizeof(master_set));
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        int res = select(listen_fd+1, &working_set, NULL, NULL, &timeout);
        if (res > 0) {
            while (1) {
                Packet pkt;
                
                int num = my_recv(listen_fd, &pkt, &client);
                if (num <= 0) {
                    break;
                }
                if (pkt.type == ACK) {
                    printf("recv\tack\t#%d\n", pkt.seq);
                    if (acked[pkt.seq] == 0) {
                        if (winSize < threshold) {
                            winSize++;
                        }
                        else {
                            cumu++;
                            if (cumu == winSize) {
                                winSize++;
                                cumu = 0;
                            }
                        }
                    }
                    acked[pkt.seq]++;
                    
                    while (acked[left] > 0) {
                        left++;
                    }
                }
                else if (pkt.type == FINACK) {
                    printf("recv\tfinack\n");
                    goto END;
                }
            }
        }
        else if (res == 0) {
            threshold = max(winSize / 2, 1);
            winSize = 1;
            printf("time\tout,\t\tthreshold = %d\n", threshold);
            seq = left - 1; // last acked
            cumu = 0;
            for (int i = left; i < left + winSize; i++) {
                if (acked[i]) {
                    cumu++;
                }
            }
        }
        else {
            printf("Error: select()\n");
            exit(1);
        }
    }

    END: ;
    close(listen_fd);
    fclose(input);

    return 0;
}