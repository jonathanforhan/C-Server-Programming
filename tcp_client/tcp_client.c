#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
typedef int SOCKET;

int main(int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(stderr, "cmdline args: tcp_client hostname, port\n");
        return 1;
    }

    // configure remote address from args
    printf("Configuring remote address...\n");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *peer_address;
    if(getaddrinfo(argv[1], argv[2], &hints, &peer_address)) {
        fprintf(stderr, "getaddrinfo() failed. (%d)", errno);
        return 1;
    }

    printf("Remote addr: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
                address_buffer, sizeof(address_buffer),
                service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);
    
    printf("%s %s\n", address_buffer, service_buffer);

    // create socket
    printf("Creating socket...\n");
    SOCKET socket_peer = socket(peer_address->ai_family,
                                peer_address->ai_socktype, peer_address->ai_protocol);
    if(!ISVALIDSOCKET(socket_peer)) {
        fprintf(stderr, "socket() failed. (%d)\n", errno);
        return 1;
    }

    printf("Connecting...\n");
    if(connect(socket_peer,
               peer_address->ai_addr, peer_address->ai_addrlen)) {
        fprintf(stderr, "connect() failed, (%d)\n", errno);
        return 1;
    }
    freeaddrinfo(peer_address);

    printf("Connected.\n");
    printf("Type message\n");

    while(1) {

        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads);
        FD_SET(0, &reads);

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        // only relavent if mulitple sockets in fd_set
        fd_set copy = reads;

        if(select(socket_peer+1, &copy, 0, 0, &timeout) < 0) {
            fprintf(stderr, "select() failed (%d)\n", errno);
            return 1;
        }
        if(FD_ISSET(socket_peer, &copy)) {
            char read[4096];
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if(bytes_received < 1) {
                printf("Connection closed by peer\n");
                break;
            }
            printf("Received (%d) bytes: %.*s",
                bytes_received, bytes_received, read);
        }
        if(FD_ISSET(0, &copy)) {

            char read[4096];
            if(!fgets(read, 4096, stdin)) break;
            printf("Sending: %s", read);
            int bytes_sent = send(socket_peer, read, strlen(read), 0);
            printf("Send %d bytes\n", bytes_sent);
        }

    } // end while loop
    printf("Closing socket\n");
    close(socket_peer);

    printf("Finished\n");
    return 0;
}
