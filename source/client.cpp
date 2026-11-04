#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#ifndef CURRENT_DIR
#define CURRENT_DIR ""
#endif

#include <filesystem>

#define BUF 1024

int main(int argc, char **argv)
{
    char buffer[BUF];
    struct sockaddr_in address;
    int create_socket = 0, size = 0;

    if (argc < 3 || argc > 3)
    {
        fprintf(stdout, "Wrong arguments are provided. Please use following syntax: \n");
        fprintf(stderr, "./twmailer-client <ip> <port>\n");
        return EXIT_FAILURE;
    }

    // CREATE A SOCKET
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    // INIT ADDRESS
    memset(&address, 0, sizeof(address)); // init storage with 0
    address.sin_family = AF_INET;         // IPv4

    inet_aton(argv[1], &address.sin_addr);
    address.sin_port = htons(atoi(argv[2])); // Assign the port received from user console arguments to the socket.

    // CREATE A CONNECTION
    if (connect(create_socket, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    // ignore return value of printf
    printf("Connection with server (%s) established\n", inet_ntoa(address.sin_addr));

    // RECEIVE DATA
    size = recv(create_socket, buffer, BUF - 1, 0);
    if (size == -1)
    {
        perror("recv error");
    }
    else if (size == 0)
    {
        printf("Server closed remote socket\n"); // ignore error
    }
    else
    {
        buffer[size] = '\0';
        printf("%s", buffer); // ignore error
    }

    while (1)
    {
        printf(">> ");
        if (fgets(buffer, BUF - 1, stdin) != NULL)
        {

            if (strncmp(buffer, "quit", 4) == 0)
            {
                break;
            }

            if (strncmp(buffer, "clear", 5) == 0)
            {
                int clear_result = system("clear"); // Execute the clear command and store its return value.
                (void)clear_result;                 // Explicitly mark the return value as intentionally unused.
                continue;
            }

            // SEND DATA
            /*if ((send(create_socket, buffer, size + 1, 0)) == -1)  {
                perror("send error");
                break;
            }*/

            // SEND DATA
            std::string outgoing; // Stores the complete request before sending it.

            if (strcmp(buffer, "SEND\n") == 0)
            {
                outgoing = "SEND\n"; // Add the SEND command to the request.

                char line[BUF]; // Temporary buffer for reading user input.

                printf("Sender: ");
                if (fgets(line, BUF, stdin) == NULL)
                    break;
                outgoing += line; // Add the sender to the request.

                printf("Receiver: ");
                if (fgets(line, BUF, stdin) == NULL)
                    break;
                outgoing += line; // Add the receiver to the request.

                printf("Subject: ");
                if (fgets(line, BUF, stdin) == NULL)
                    break;
                outgoing += line; // Add the subject to the request.

                printf("Message (finish with a dot on a separate line):\n");

                // Read message lines until the user enters a dot.
                while (fgets(line, BUF, stdin) != NULL)
                {
                    outgoing += line; // Add the current message line.

                    if (strcmp(line, ".\n") == 0 || strcmp(line, ".") == 0)
                    {
                        break; // Stop reading when the end marker is entered.
                    }
                }
            }
            else
            {
                outgoing = buffer; // Use the original input for other commands.
            }

            // Send the complete request to the server.
            if (send(create_socket, outgoing.c_str(), outgoing.length(), 0) == -1)
            {
                perror("send error");
                break;
            }

            // RECEIVE FEEDBACK. This function blocks until it receives a response from the server.
            size = recv(create_socket, buffer, BUF - 1, 0);
            if (size == -1)
            {
                perror("recv error");
                break;
            }
            else if (size == 0)
            {
                printf("Server closed remote socket\n");
                break;
            }
            else
            {
                buffer[size] = '\0';
                printf("<< %s\n", buffer);
            }
        }
    }

    // CLOSES THE DESCRIPTOR
    if (create_socket != -1)
    {
        if (shutdown(create_socket, SHUT_RDWR) == -1)
        {
            // invalid in case the server is gone already
            perror("shutdown create_socket");
        }
        if (close(create_socket) == -1)
        {
            perror("close create_socket");
        }
        create_socket = -1;
    }

    return EXIT_SUCCESS;
}
