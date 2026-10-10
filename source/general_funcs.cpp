#include "../headers/general_funcs.h"

#include <iostream>
#include <stdlib.h>

/* Parses the received data seperating the individual components. Initializes a struct to hold those data. */
int dispatchUserRequest(std::string data) {

    std::string delimiter = "\\n";    // We need that here because the new line characters are interpreted as individual characters.
    std::string method = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    delimiter = "\n";    // Thats the normal new line charakter, appended to received data after enter is pressed in the console.
    std::string request_body = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    if (method == "SEND") {
        fprintf(stdout, "User sended a message! Run save message pipeline\n");
        // Implement here SEND logik
    } else if (method == "LIST") {
        fprintf(stdout, "User requested message list!\n");
        // Implement here LIST logik
    } else if (method == "READ") {
        fprintf(stdout, "User requested message read!\n");
        // Implement here READ logik
    } else if (method == "DEL") {
        fprintf(stdout, "User requested message delete!\n");
        // Implement here DEL logik
    } else {
        fprintf(stderr, "Unkown command: %s!\n", method.c_str());
        return -1;
    }

    printf("method: %s\nrequest body: %s\n\n", method.c_str(), request_body.c_str());

    return EXIT_SUCCESS;
}
// int send(char *sender, char *receiver, char *subject, char *message) {

// }