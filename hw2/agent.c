#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "UDP.h"

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s sender_ip sender_port receiver_ip receiver_port, loss_rate\n", argv[0]);
        exit(1);
    }
    char sender_ip[MAX_LEN], sender_port[MAX_LEN], receiver_ip[MAX_LEN], receiver_port[MAX_LEN];
    strcpy(sender_ip, argv[1]);
    strcpy(sender_port, argv[2]);
    strcpy(receiver_ip, argv[3]);
    strcpy(receiver_port, argv[4]);
    double loss_rate = atof(argv[5]);

    int listen_fd;
    struct sockaddr_in server, client;
    create_socket(&listen_fd, &server, 7487);
    
    struct sockaddr_in sender, receiver;
    set_addr(&sender, sender_ip, atoi(sender_port));
    set_addr(&receiver, receiver_ip, atoi(receiver_port));

    srand(time(NULL));
    int losses = 0, recvs = 0;
    int flag = 0; // testing
    while (1) {
        Packet pkt;
        my_recv(listen_fd, &pkt, &client);
        if (pkt.type == DATA) {
            printf("get\tdata\t#%d\n", pkt.seq);
            recvs++;
            
            // if (rand() < loss_rate) {
            if (pkt.seq == 4 && flag == 0) { // testing
                flag = 1; // testing
                losses++;
                printf("drop\tdata\t#%d,\tloss rate = %.4f\n", pkt.seq, 1.0 * losses / recvs);
            }
            else {
                my_send(listen_fd, &pkt, &receiver);
                printf("fwd\tdata\t#%d,\tloss rate = %.4f\n", pkt.seq, 1.0 * losses / recvs);
            }
        }
        else if (pkt.type == ACK) {
            printf("get\tack\t#%d\n", pkt.seq);
            my_send(listen_fd, &pkt, &sender);
            printf("fwd\tack\t#%d\n", pkt.seq);
        }
        else if (pkt.type == FIN) {
            printf("get\tfin\n");
            my_send(listen_fd, &pkt, &receiver);
            printf("fwd\tfin\n");
        }
        else if (pkt.type == FINACK) {
            printf("get\tfinack\n");
            my_send(listen_fd, &pkt, &sender);
            printf("fwd\tfinack\n");
            break;
        }
    }
    close(listen_fd);
    
    return 0;
}