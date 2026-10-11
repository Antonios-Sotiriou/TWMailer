#include "../headers/general_funcs.h"

#include <iostream>
#include <stdlib.h>
#include <filesystem>
#include <fstream>

#ifndef CURRENT_DIR
    #define CURRENT_DIR    ""
#endif

/* Parses the received data seperating the individual components. Initializes a struct to hold those data. */
int dispatchUserRequest(std::string data) {

    std::string delimiter = "\\n";    // We need that here because the new line characters are interpreted as individual characters.
    std::string method = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    delimiter = "\n";    // Thats the normal new line charakter, appended to received data after enter is pressed in the console.
    std::string request_body = data.substr(0, data.find(delimiter));

    if (method == "SEND") {
        fprintf(stdout, "User sended a message! Run save message pipeline\n");
        // Implement here SEND logik
        send(data);
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

    return EXIT_SUCCESS;
}
int send(std::string data) {
    std::string delimiter = "\\n";    // We need that here because the new line characters are interpreted as individual characters.
    std::string sender = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    std::string receiver = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    std::string subject = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    std::string message = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    std::string command_end = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    // At this point we receive a correct command from the user, so create the receiver folder if it not exists.
    namespace fs = std::filesystem;
    std::string new_dir = std::string(CURRENT_DIR) + "/mail_spool_dir/" + std::string(receiver);
    if (!fs::exists(new_dir)) {    // Check if user folder exists and if not create it.
        if (!fs::create_directory(new_dir)) {
            fprintf(stdout, "Create user directory failed!\n");
        }
    }

    // Open a received_messages file or create if it not exists in append mode.
    std::fstream file;
    std::string data_file = new_dir + "/received_messages.txt";
    file.open(data_file, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        fprintf(stderr, "Could not open file %s\n", data_file.c_str());
        return -1;
    }
    // Write the data in the file
    file << sender << std::endl << receiver << std::endl << subject << std::endl << message << std::endl << command_end << std::endl;

    return EXIT_SUCCESS;
}