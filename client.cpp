#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUF         1024

int main(int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
    struct sockaddr_in address;
    int size;
    int isQuit;

    if (argc < 3 || argc > 3) {
        fprintf(stdout, "Wrong arguments are provided. Please use following syntax: \n");
        fprintf(stderr, "./twmailer-client <ip> <port>\n");
        return EXIT_FAILURE;
    }

    // CREATE A SOCKET
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    // INIT ADDRESS
    memset(&address, 0, sizeof(address)); // init storage with 0
    address.sin_family = AF_INET;         // IPv4

    inet_aton(argv[1], &address.sin_addr);
    address.sin_port = htons(atoi(argv[2]));    // Assign the port received from user console arguments to the socket.

    // CREATE A CONNECTION
    if (connect(create_socket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    // ignore return value of printf
    printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));

    // RECEIVE DATA
    size = recv(create_socket, buffer, BUF - 1, 0);
    if (size == -1) {
        perror("recv error");
    } else if (size == 0) {
        printf("Server closed remote socket\n"); // ignore error
    } else {
        buffer[size] = '\0';
        printf("%s", buffer); // ignore error
    }

    do {
        printf(">> ");
        if (fgets(buffer, BUF - 1, stdin) != NULL) {
            int size = strlen(buffer);

            isQuit = strcmp(buffer, "quit") == 0;

            if (strcmp(buffer, "clear") == 0) {
                system("clear");
                continue;
            }

            // SEND DATA
            if ((send(create_socket, buffer, size + 1, 0)) == -1)  {
                perror("send error");
                break;
            }

            // RECEIVE FEEDBACK. This function blocks until it receives a response from the server.
            size = recv(create_socket, buffer, BUF - 1, 0);
            if (size == -1) {
                perror("recv error");
                break;
            } else if (size == 0) {
                printf("Server closed remote socket\n");
                break;
            } else {
                buffer[size] = '\0';
                printf("<< %s\n", buffer);
            }
        }
    } while (!isQuit);

    // CLOSES THE DESCRIPTOR
    if (create_socket != -1) {
        if (shutdown(create_socket, SHUT_RDWR) == -1) {
            // invalid in case the server is gone already
            perror("shutdown create_socket"); 
        }
        if (close(create_socket) == -1) {
            perror("close create_socket");
        }
        create_socket = -1;
    }

    return EXIT_SUCCESS;
}
