#include"tcp_server.h"

int main() {

    SOCKET server = create_socket("8080");

    struct client_info* client_list = NULL;

    while(1) {

        fd_set reads;
        reads = wait_on_clients(&client_list, server);

        if(FD_ISSET(server, &reads)) {
            struct client_info* client = get_client(&client_list, -1);
            
            client->socket = accept(server,
                                    (struct sockaddr*)&(client->address),
                                    &(client->address_len));

            if(client->socket < 0) {
                printf("accept() error\n"); return 1;
            }

            printf("New connection from %s\n", get_client_addr(client));
        }

        struct client_info* client = client_list;
        while(client) {

            struct client_info* next = client->next;

            if(FD_ISSET(client->socket, &reads)) {

                char msg[MAX_MSG_SIZE];

                int bytes_recv = recv(client->socket, msg, MAX_MSG_SIZE, 0);
                if(bytes_recv < 1) {
                    printf("Dropped %s\n", get_client_addr(client));
                    drop_client(&client_list, client);
                } else {
                    send_message(&client_list, client, msg, bytes_recv);
                }
            }
            client = next;
        }
    } // end while(1)

    close(server);
}