#include"tcp_server.h"

SOCKET create_socket(const char* port) {

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(NULL, port, &hints, &bind_address);

    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
                            bind_address->ai_socktype,
                            bind_address->ai_protocol);
    
    if(socket_listen < 0) { printf("socket() failed\n"); exit(1); }

    // bind socket
    if(bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen)) { printf("bind() error\n"); exit(1); }
    freeaddrinfo(bind_address);

    // listen socket
    if(listen(socket_listen, 10) < 0) { printf("listen() error\n"); exit(1); }

    return socket_listen;
}

struct client_info* get_client(struct client_info** client_list, SOCKET s) {

    struct client_info* ci = *client_list;

    while(ci) {
        if(ci->socket == s) {
            break;
        }
        ci = ci->next;
    }

    if(ci) { return ci; }

    struct client_info* n = (struct client_info*) calloc(1, sizeof(struct client_info));

    if(!n) { printf("out of memory\n"); exit(1); }

    n->address_len = sizeof(n->address);
    n->next = *client_list;
    *client_list = n;
    return n;
}

void drop_client(struct client_info** client_list, struct client_info* client) {

    close(client->socket);

    struct client_info** p = client_list;

    while(*p) {
        printf("haystack: %p\n", *p);
        if(*p == client) {
            *p = client->next;
            printf("needle: %p\n", client);
            free(client);
            return;
        }
        p = &(*p)->next;
    }
    printf("dropped client not found\n"); exit(1);
}

const char* get_client_addr(struct client_info* ci) {

    getnameinfo((struct sockaddr*)&ci->address,
                ci->address_len,
                ci->address_buffer, sizeof(ci->address_buffer), 0, 0,
                NI_NUMERICHOST);

    return ci->address_buffer;
}

fd_set wait_on_clients(struct client_info** client_list, SOCKET server) {

    fd_set reads;
    FD_ZERO(&reads);
    FD_SET(server, &reads);
    SOCKET max_socket = server;

    struct client_info* ci = *client_list;

    while(ci) {
        FD_SET(ci->socket, &reads);
        if(ci->socket > max_socket) {
            max_socket = ci->socket;
        }
        ci = ci->next;
    }
    
    if(select(max_socket+1, &reads, 0, 0, 0) < 0) { printf("select() error\n"); exit(1); }

    return reads;
}

void send_message(struct client_info** client_list, struct client_info* client, char msg[MAX_MSG_SIZE], int bytes) {

    struct client_info** p = client_list;

    while(*p) {
        if(*p != client) {
            struct client_info* cl = *p;
            send(cl->socket, msg, bytes, 0);
        }
        p = &(*p)->next;
    }
}