#include "../headers/general_funcs.h"

#include <stdio.h>
#include <string.h>

/* Parses the received data seperating the individual components. Initializes a struct to hold those data. */
int parseReceivedData(TWMail *twmail, char data[]) {
    char *token = strtok(data, "\n");
    if (token != NULL) {
        twmail->method = strdup(token);
    }

    token = strtok(NULL, "\n");
    if (token != NULL) {
        twmail->sender = strdup(token);
    }

    token = strtok(NULL, "\n");
    if (token != NULL) {
        twmail->receiver = strdup(token);
    }

    token = strtok(NULL, "\n");
    if (token != NULL) {
        int len = strlen(token);
        if (len > 80) {    // Subject can not be more than 80 characters long
            return -1;
        }
        strncpy(twmail->subject, token, len);
        twmail->subject[len] = '\0';
    }

    token = strtok(NULL, "\n");
    if (token != NULL) {
        twmail->message = strdup(token);
    }

    return 0;
}
/* Identifies the command entered by the user and calls the appropriate functions, to fullfil this command. */
void dispatchCommand(TWMail *twmail) {

    // Maybe here we can do error handling, if the user entered a correct command and values.

    if (strcmp(twmail->method, "SEND") == 0) {
        fprintf(stdout, "User sended a message! Run save message pipeline\n");
        // Implement here SEND logik
    } else if (strcmp(twmail->method, "LIST") == 0) {
        fprintf(stdout, "User requested message list!\n");
        // Implement here LIST logik
    } else if (strcmp(twmail->method, "READ") == 0) {
        fprintf(stdout, "User requested message read!\n");
        // Implement here READ logik
    } else if (strcmp(twmail->method, "DEL") == 0) {
        fprintf(stdout, "User requested message delete!\n");
        // Implement here DEL logik
    }

    printf("method: %s    sender: %s    receiver: %s    Subject: %s    message: %s\n", 
        twmail->method, twmail->sender, twmail->receiver, twmail->subject, twmail->message
    );
}