#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
typedef int SOCKET;

int main() {

    printf("Configuring local address...\n");

    // hints tells what info we are looking for
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // bind server to our local ip for inet4 or inet 6
    // and bind to proper port this case 8080
    struct addrinfo *bind_address;
    int result = getaddrinfo(0, "8080", &hints, &bind_address);
    if(!ISVALIDSOCKET(result)) {
        printf("binding address faield");
        return 1;
    }

    printf("Creating socket...\n");

    // generate and initialize socket_listen
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family,
                           bind_address->ai_socktype,
                           bind_address->ai_protocol);

    if(!ISVALIDSOCKET(socket_listen)) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }

    // convert any IPv4 to IPv6
    int option = 0;
    if(setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY,
                  (void*)&option, sizeof(option))) {

        fprintf(stderr, "setsockopt() failed. (%d)\n", errno);
        return 1;
    }

    printf("Binding socket to local address...\n");
    if(bind(socket_listen,
            bind_address->ai_addr, bind_address->ai_addrlen)) {
        fprintf(stderr, "bind() failed. (%d)\n", errno);
        return 1;
    }
    
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if(listen(socket_listen, 10) < 0) {
        fprintf(stderr, "listen() failed. (%d)\n", errno);
        return 1;
    }

    // connect and store client socket
    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t clinet_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen,
                                  (struct sockaddr*) &client_address, &clinet_len);
    if(!ISVALIDSOCKET(socket_client)) {
        fprintf(stderr, "accept() failed, (%d)\n", errno);
        return 1;
    }

    printf("Client connected>>> ");
    char address_buffer[100];
    getnameinfo((struct sockaddr*) &client_address,
                clinet_len, address_buffer, sizeof(address_buffer), 0, 0,
                NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recv(socket_client, request, 1024, 0);
    printf("Received %d bytes,\n", bytes_received);

    printf("%.*s", bytes_received, request);

    printf("Sending response...\n");
    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Local time is: ";
    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

    printf("Closing connection...\n");
    close(socket_client);

    printf("Closing listening socket...\n");
    close(socket_listen);

    printf("Finished\n");

    return 0;
}
