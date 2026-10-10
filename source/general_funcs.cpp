#include "../headers/general_funcs.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Parses the received data seperating the individual components. Initializes a struct to hold those data. */
int dispatchCommand(std::string data) {

    std::string delimiter = "\\n";
    std::string method = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    delimiter = "\n";
    std::string request_body = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());
    // char *method = { 0 };
    // char *request_body = { 0 };
    
    // char *token = strtok(data, "\\");
    // token = strtok(NULL, "n");
    // if (token == NULL || strcmp(token, " ") == 0) {
    //     return -1;
    // }
    // method = strdup(token);

    // token = strtok(NULL, "\0");
    // if (token == NULL || strcmp(token, " ") == 0) {
    //     return -1;
    // }
    // request_body = strdup(token);

    // printf("method: %s    request body:\n %s\n", method, request_body);

    // if (strcmp(method, "SEND") == 0) {
    //     fprintf(stdout, "User sended a message! Run save message pipeline\n");
    //     // Implement here SEND logik
    // } else if (strcmp(method, "LIST") == 0) {
    //     fprintf(stdout, "User requested message list!\n");
    //     // Implement here LIST logik
    // } else if (strcmp(method, "READ") == 0) {
    //     fprintf(stdout, "User requested message read!\n");
    //     // Implement here READ logik
    // } else if (strcmp(method, "DEL") == 0) {
    //     fprintf(stdout, "User requested message delete!\n");
    //     // Implement here DEL logik
    // } else {
    //     fprintf(stderr, "Unkown command!\n");
    //     return -1;
    // }

    printf("method: %s    request body:\n %s\n", method.c_str(), request_body.c_str());
    // free(method);
    // free(request_body);

    return EXIT_SUCCESS;
}
// int send(char *sender, char *receiver, char *subject, char *message) {

// }