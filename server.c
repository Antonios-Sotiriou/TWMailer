#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "headers/general_funcs.h"

#define BUF 1024

int abortRequested = 0;
int create_socket = -1;
int new_socket = -1;

void *clientCommunication(void *data);
void signalHandler(int sig);

int main(int argc, char **argv) {
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;
    int reuseValue = 1;
    char *mail_spool_dir;

    if (argc < 3 || argc > 3) {
        fprintf(stderr, "Wrong arguments are provided. Please use following syntax: \n");
        fprintf(stderr, "./twmailer-server <port> <mail-spool-directoryname>\n");
        return EXIT_FAILURE;
    }

    // SIGNAL HANDLER
    if (signal(SIGINT, signalHandler) == SIG_ERR) {
        fprintf(stderr, "signal can not be registered");
        return EXIT_FAILURE;
    }

    // CREATE A SOCKET
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Socket error"); // errno set by socket()
        return EXIT_FAILURE;
    }

    // SET SOCKET OPTIONS
    if (setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &reuseValue, sizeof(reuseValue)) == -1) {
        fprintf(stderr, "set socket options - reuseAddr");
        return EXIT_FAILURE;
    }

    if (setsockopt(create_socket, SOL_SOCKET, SO_REUSEPORT, &reuseValue, sizeof(reuseValue)) == -1) {
        fprintf(stderr, "set socket options - reusePort");
        return EXIT_FAILURE;
    }

    // INIT ADDRESS
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));
    mail_spool_dir = strdup(argv[2]);    // Aquire the mail-spool-directory name received from user console arguments.

    // ASSIGN AN ADDRESS WITH PORT TO SOCKET
    if (bind(create_socket, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "bind error");
        free(mail_spool_dir);
        return EXIT_FAILURE;
    }

    // ALLOW CONNECTION ESTABLISHING
    if (listen(create_socket, 5) == -1) {
        fprintf(stderr, "listen error");
        free(mail_spool_dir);
        return EXIT_FAILURE;
    }

    while (!abortRequested) {
        printf("Waiting for connections...\n");

        // ACCEPTS CONNECTION SETUP
        addrlen = sizeof(struct sockaddr_in);
        if ((new_socket = accept(create_socket, (struct sockaddr *)&cliaddress, &addrlen)) == -1) {
            if (abortRequested) {
            fprintf(stderr, "accept error after aborted");
            } else {
            fprintf(stderr, "accept error");
            }
            break;
        }

        // START CLIENT. Probably here we should implement fork or threads.
        fprintf(stdout, "Client connected from %s:%d...\n", inet_ntoa(cliaddress.sin_addr), ntohs(cliaddress.sin_port));
        clientCommunication(&new_socket); // returnValue can be ignored for now.
        new_socket = -1;
    }

    // frees the descriptor
    if (create_socket != -1) {
        if (shutdown(create_socket, SHUT_RDWR) == -1) {
            fprintf(stderr, "shutdown create_socket");
        }
        if (close(create_socket) == -1) {
            fprintf(stderr, "close create_socket");
        }
        create_socket = -1;
    }

    free(mail_spool_dir);

    return EXIT_SUCCESS;
}
void *clientCommunication(void *data) {
    char buffer[BUF];
    int size;
    int *current_socket = (int*)data;
    char received_data[4096] = { 0 };    // Here we gather all informations from which a command consists.
    size_t total_size = 0;

    // SEND welcome message
    strcpy(buffer, "Welcome to myserver!\r\nPlease enter your commands...\r\n");
    if (send(*current_socket, buffer, strlen(buffer), 0) == -1) {
        fprintf(stderr, "send failed");
        return NULL;
    }

    TWMail twmail = { 0 };

    do {
        // RECEIVE
        size = recv(*current_socket, buffer, BUF - 1, 0);
        if (size == -1) {
            if (abortRequested) {
                fprintf(stderr, "recv error after aborted");
            } else {
                fprintf(stderr, "recv error");
            }
            break;
        } else if (size == 0) {
            fprintf(stdout, "Client closed remote socket\n");
            break;
        }

        total_size += size;
        strcat(received_data, buffer);
        strcat(received_data, "\n");

        if (strcmp(buffer, ".") == 0) {

            received_data[total_size] = '\0';

            if (parseReceivedData(&twmail, received_data) == -1) {    // We parse the command the user entered and initializing twmail struct.

                if (send(*current_socket, "ERR", 4, 0) == -1) {
                    fprintf(stderr, "send answer failed");
                    return NULL;
                }

                fprintf(stderr, "Received data parsing failed");
                return NULL;
            }

            dispatchCommand(&twmail);    // Here we will handle the action that the user requested.

            memset(received_data, '\0', total_size);    // reset received_data for the next command.
        }

        if (send(*current_socket, "OK", 3, 0) == -1) {
            fprintf(stderr, "send answer failed");
            return NULL;
        }
    } while (strcmp(buffer, "quit") != 0 && !abortRequested);

    // closes/frees the descriptor if not already
    if (*current_socket != -1) {
        if (shutdown(*current_socket, SHUT_RDWR) == -1) {
            fprintf(stderr, "shutdown new_socket");
        }
        if (close(*current_socket) == -1) {
            fprintf(stderr, "close new_socket");
        }
        *current_socket = -1;
    }

    return NULL;
}
void signalHandler(int sig) {
    if (sig == SIGINT) {
        printf("abort Requested... "); // ignore error
        abortRequested = 1;

        // With shutdown() one can initiate normal TCP close sequence ignoring the reference count.
        if (new_socket != -1) {
            if (shutdown(new_socket, SHUT_RDWR) == -1) {
            perror("shutdown new_socket");
            }
            if (close(new_socket) == -1) {
            perror("close new_socket");
            }
            new_socket = -1;
        }

        if (create_socket != -1) {
            if (shutdown(create_socket, SHUT_RDWR) == -1) {
            perror("shutdown create_socket");
            }
            if (close(create_socket) == -1) {
            perror("close create_socket");
            }
            create_socket = -1;
        }
    } else {
        exit(sig);
    }
}
