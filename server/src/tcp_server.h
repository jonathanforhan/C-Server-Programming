#ifndef TCP_CONTROL_H
#define TCP_CONTROL_H

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

typedef int SOCKET;
#define MAX_MSG_SIZE 2047

struct client_info {
    socklen_t address_len;
    struct sockaddr_storage address;
    char address_buffer[128];
    SOCKET socket;
    char request[2048];
    int received;
    struct client_info* next;
};

SOCKET create_socket(const char* port);

struct client_info* get_client(struct client_info** client_list, SOCKET s);

void drop_client(struct client_info** client_list, struct client_info* client);

const char* get_client_addr(struct client_info* ci);

fd_set wait_on_clients(struct client_info** client_list, SOCKET server);

void send_message(struct client_info** client_list, struct client_info* client, char msg[MAX_MSG_SIZE], int bytes);

#endif