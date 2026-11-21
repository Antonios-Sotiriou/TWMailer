#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "../headers/get_user_pswd.h"
#include "../headers/ldap_authentication.h"
#include "../headers/general_funcs.h"

#ifndef CURRENT_DIR
#define CURRENT_DIR ""
#endif

#include <filesystem>

#define BUF 1024

static void send(std::string user, std::string &outgoing);
static void list(std::string user, std::string &outgoing);
static void read(std::string user, std::string &outgoing);
static void del(std::string user, std::string &outgoing);
static bool login(std::string &user);

int main(int argc, char **argv)
{
    char buffer[BUF];
    struct sockaddr_in address;
    int create_socket = 0, size = 0;
    std::string user;
    bool user_logged_in = false;

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

    // Connection with server established
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

    int running = 1;
    while (running)
    {
        printf("commands (LOGIN, QUIT, CLEAR) ");
        if (fgets(buffer, BUF - 1, stdin) != NULL)
        {
            if (strncmp(buffer, "QUIT", 4) == 0)
            {
                break;
            }
            else if (strncmp(buffer, "CLEAR", 5) == 0)
            {
                int clear_result = system("clear");
                (void)clear_result;
                continue;
            }
            else if (strncmp(buffer, "LOGIN", 5) == 0) {
                if (!user_logged_in) {
                    user_logged_in = login(user);
                    // user_logged_in = true;
                    if (!user_logged_in) {
                        printf("User login failed!\n");
                        continue;
                    }
                }
            } else {
                continue;
            }

            if (user_logged_in) 
            {
                while (1) 
                {
                    printf("commands (SEND, LIST, READ, DEL, CLEAR, LOGOUT, QUIT) ");
                    if (fgets(buffer, BUF - 1, stdin) != NULL)
                    {
                        // SEND DATA
                        std::string outgoing; // Stores the complete request before sending it.

                        if (strncmp(buffer, "QUIT", 4) == 0)
                        {
                            running = 0;
                            break;
                        }
                        else if (strncmp(buffer, "CLEAR", 5) == 0)
                        {
                            int clear_result = system("clear");
                            (void)clear_result;
                            continue;
                        }
                        else if (strncmp(buffer, "LOGOUT", 6) == 0)
                        {
                            user_logged_in = false;
                            break;
                        }
                        else if (strcmp(buffer, "LIST\n") == 0)
                        {
                            list(user, outgoing);
                        }
                        else if (strcmp(buffer, "READ\n") == 0)
                        {
                            read(user, outgoing);
                        }
                        else if (strcmp(buffer, "SEND\n") == 0)
                        {
                            send(user, outgoing);
                        }
                        else if (strcmp(buffer, "DEL\n") == 0)
                        {
                            del(user, outgoing);
                        }
                        else
                        {
                            outgoing = buffer;
                        }

                        // Send the complete request to the server.
                        if (send(create_socket, outgoing.c_str(), outgoing.length(), 0) == -1)
                        {
                            perror("send error");
                            break;
                        }
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
static void send(std::string user, std::string &outgoing)
{
    outgoing = "SEND\n"; // Add the SEND command to the request.
    outgoing += user + "\n";

    char line[BUF]; // Temporary buffer for reading user input.

    printf("Receiver: ");
    if (fgets(line, BUF, stdin) == NULL)
        return;
    outgoing += line; // Add the receiver to the request.

    printf("Subject: ");
    if (fgets(line, BUF, stdin) == NULL)
        return;
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
static void del(std::string user, std::string &outgoing)
{
    outgoing = "DEL\n"; // Add the DEL command to the request.
    outgoing += user + "\n"; 

    char line[BUF]; // Temporary buffer for reading user input.

    printf("Message-Number: ");
    if (fgets(line, BUF, stdin) == NULL)
        return;
    outgoing += line; // Add the message number to the request.
}
static void list(std::string user, std::string &outgoing)
{
    outgoing = "LIST\n";
    outgoing += user + "\n"; 
}
static void read(std::string user, std::string &outgoing)
{
    outgoing = "READ\n";
    outgoing += user + "\n"; 

    char line[BUF];

    printf("Message number: ");
    if (fgets(line, BUF, stdin) == NULL)
        return;

    outgoing += line;
}
static bool login(std::string &user) {

    printf("User (max 8 chars): ");
    std::cin >> user;
    std::cin.get(); // Clears the new line char left in stdin

    if (isValidUsername(user)) {
        std::string password = getpass();

        return authenticateLdap(user, password) == 0;
    }

    printf("Invalid Username!\n");
    return false;
}
