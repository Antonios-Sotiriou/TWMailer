#include "../headers/general_funcs.h"

#include <iostream>
#include <stdlib.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stdexcept>

#ifndef CURRENT_DIR
#define CURRENT_DIR ""
#endif

// Prüft, ob der Benutzername gültig ist
bool isValidUsername(const std::string &username)
{
    // Prüft die Länge
    if (username.empty() || username.length() > 8)
    {
        return false;
    }

    // Prüft jedes Zeichen
    for (char character : username)
    {

        if (!((character >= 'a' && character <= 'z') ||
              (character >= '0' && character <= '9')))
        {
            return false;
        }
    }

    return true;
}

/* Parses the received data seperating the individual components. Initializes a struct to hold those data. */
int dispatchUserRequest(std::string data, std::string mail_spool_directory, std::string &response)
{
    // The client sends actual newline characters when the user presses Enter.
    // We use "\n" as a delimiter to separate the individual lines of the received data.
    // Using "\\n" would search for the two characters '\' and 'n' instead of an actual newline
    std::string delimiter = "\n";
    std::string method = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    delimiter = "\n"; // Thats the normal new line charakter, appended to received data after enter is pressed in the console.
    std::string request_body = data.substr(0, data.find(delimiter));

    if (method == "SEND")
    {
        fprintf(stdout, "User sended a message! Run save message pipeline\n");
        int result = send(data, mail_spool_directory);
        if (result == EXIT_SUCCESS)
        {
            response = "OK\n"; // Gibt eine Bestätigung bei erfolgreichem Senden zurück
        }
        return result;
    }
    else if (method == "LIST")
    {
        fprintf(stdout, "User requested message list!\n");
        return list(data, mail_spool_directory, response); // Erstellt die Nachrichtenliste für den Client
    }
    else if (method == "READ")
    {
        fprintf(stdout, "User requested message read!\n");
        return readMessage(data, mail_spool_directory, response); // Liest eine bestimmte Nachricht
    }
    else if (method == "DEL")
    {
        fprintf(stdout, "User requested message delete!\n");
        int result = del(data, mail_spool_directory);
        if (result == EXIT_SUCCESS)
        {
            response = "OK\n"; // Gibt eine Bestätigung bei erfolgreichem Senden zurück
        }
        return result;
    }
    else
    {
        fprintf(stderr, "Unkown command: %s!\n", method.c_str());
        return -1;
    }

    return EXIT_SUCCESS;
}
int send(std::string data, std::string mail_spool_directory)
{
    std::string delimiter = "\n";
    std::string sender = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    std::string receiver = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    // Prüft Sender und Empfänger
    if (!isValidUsername(sender) || !isValidUsername(receiver))
    {
        return -1;
    }

    std::string subject = data.substr(0, data.find(delimiter));
    data.erase(0, data.find(delimiter) + delimiter.length());

    std::string message;           // Stores the complete message, including multiple lines.
    std::string command_end = "."; // Stores the end-of-message marker.

    // Read message lines until the dot marks the end of the message.
    while (!data.empty())
    {
        std::string line = data.substr(0, data.find(delimiter)); // Extract the next message line.

        data.erase(0, data.find(delimiter) + delimiter.length()); // Remove the extracted line from the remaining data.
        

        if (line == ".")
        {          // Check if the end-of-message marker was received.
            break; // Stop reading the message.
        }

        message += line + "\n"; // Add the current line to the complete message.
    }

    // At this point we receive a correct command from the user, so create the receiver folder if it not exists.
    namespace fs = std::filesystem;
    std::string new_dir = std::string(CURRENT_DIR) + "/" + mail_spool_directory + "/" + std::string(receiver);
    if (!fs::exists(new_dir))
    { // Check if user folder exists and if not create it.
        if (!fs::create_directory(new_dir))
        {
            fprintf(stdout, "Create user directory failed!\n");
        }
    }

    // Open a received_messages file or create if it not exists in append mode.
    std::fstream file;
    std::string data_file = new_dir + "/received_messages.txt";
    file.open(data_file, std::ios::out | std::ios::app);
    if (!file.is_open())
    {
        fprintf(stderr, "Could not open file %s\n", data_file.c_str());
        return -1;
    }
    // Write the data in the file
    file << sender << std::endl
         << receiver << std::endl
         << subject << std::endl
         << message << std::endl
         << command_end << std::endl;

    return EXIT_SUCCESS;
}

// Erstellt eine Liste der Nachrichten
int list(std::string data, std::string mail_spool_directory, std::string &response)
{
    std::string delimiter = "\n";

    // Liest den Benutzernamen aus der Anfrage
    std::string username = data.substr(0, data.find(delimiter));
    // Prüft den Benutzernamen
    if (!isValidUsername(username))
    {
        return -1;
    }

    // Erstellt den Pfad zur Nachrichtendatei
    std::string data_file = std::string(CURRENT_DIR) + "/" +
                            mail_spool_directory + "/" +
                            username + "/received_messages.txt";

    // Öffnet die Nachrichtendatei
    std::ifstream file(data_file);

    if (!file.is_open())
    {
        response = "0\n";
        return EXIT_SUCCESS;
    }

    std::vector<std::string> subjects;

    std::string sender;
    std::string receiver;
    std::string subject;
    std::string line;

    // Liest alle gespeicherten Nachrichten
    while (std::getline(file, sender))
    {

        if (!std::getline(file, receiver) ||
            !std::getline(file, subject))
        {
            return -1;
        }

        // Speichert den Betreff der Nachricht
        subjects.push_back(subject);

        // Liest den Nachrichteninhalt bis zum Endzeichen
        bool end_found = false;

        while (std::getline(file, line))
        {
            if (line == ".")
            {
                end_found = true;
                break;
            }
        }

        if (!end_found)
        {
            return -1;
        }
    }

    // Erstellt die Antwort mit Anzahl und Betreffzeilen
    response = std::to_string(subjects.size()) + "\n";

    for (const std::string &current_subject : subjects)
    {
        response += current_subject + "\n";
    }

    return EXIT_SUCCESS;
}

// Liest eine bestimmte Nachricht
int readMessage(std::string data, std::string mail_spool_directory, std::string &response)
{
    std::string delimiter = "\n";

    // Liest den Benutzernamen aus der Anfrage
    std::size_t delimiter_position = data.find(delimiter);

    if (delimiter_position == std::string::npos)
    {
        return -1;
    }

    std::string username = data.substr(0, delimiter_position);
    // Prüft den Benutzernamen.
    if (!isValidUsername(username))
    {
        return -1;
    }
    data.erase(0, delimiter_position + delimiter.length());

    // Liest die Nachrichtennummer aus der Anfrage.
    delimiter_position = data.find(delimiter);
    std::string message_number_text = data.substr(0, delimiter_position);

    if (username.empty() || message_number_text.empty())
    {
        return -1;
    }

    int requested_number;

    try
    {
        std::size_t processed_chars = 0;

        requested_number = std::stoi(message_number_text, &processed_chars);

        if (processed_chars != message_number_text.length())
        {
            return -1;
        }
    }
    catch (const std::exception &)
    {
        return -1;
    }

    if (requested_number < 1)
    {
        return -1;
    }

    // Erstellt den Pfad zur Nachrichtendatei.
    std::string data_file = std::string(CURRENT_DIR) + "/" +
                            mail_spool_directory + "/" +
                            username + "/received_messages.txt";

    // Öffnet die Nachrichtendatei
    std::ifstream file(data_file);

    if (!file.is_open())
    {
        return -1;
    }

    std::string sender;
    std::string receiver;
    std::string subject;
    std::string line;
    int current_number = 0;

    // Liest alle gespeicherten Nachrichten
    while (std::getline(file, sender))
    {

        if (!std::getline(file, receiver) ||
            !std::getline(file, subject))
        {
            return -1;
        }

        current_number++;

        std::string message_body;
        bool end_found = false;

        // Liest den Nachrichteninhalt bis zum Endzeichen
        while (std::getline(file, line))
        {

            if (line == ".")
            {
                end_found = true;
                break;
            }

            message_body += line + "\n";
        }

        if (!end_found)
        {
            return -1;
        }

        // Prüft, ob die gewünschte Nachricht gefunden wurde
        if (current_number == requested_number)
        {

            response = "OK\n" +
                       sender + "\n" +
                       receiver + "\n" +
                       subject + "\n" +
                       message_body + ".\n";

            return EXIT_SUCCESS;
        }
    }

    return -1;
}
int del(std::string data, std::string mail_spool_directory) {
    std::string delimiter = "\n";

    // Liest den Benutzernamen aus der Anfrage
    std::string username = data.substr(0, data.find(delimiter));
    // Prüft den Benutzernamen
    if (!isValidUsername(username))
    {
        return -1;
    }
    data.erase(0, data.find(delimiter) + delimiter.length());

    // Liest den Message-Nummer aus der Anfrage
    int message_nummer = std::stoi(data.substr(0, data.find(delimiter)));
    data.erase(0, data.find(delimiter) + delimiter.length());

    // Erstellt den Pfad zur Nachrichtendatei des Benutzers.
    std::string file_name = std::string(CURRENT_DIR) + "/" +
                            mail_spool_directory + "/" +
                            username + "/received_messages.txt";

    // Öffnet die Nachrichtendatei
    std::ifstream file_in(file_name);
    if (!file_in.is_open()) {
        return -1;
    }

    // Liest den Nachrichteninhalt bis zum Endzeichen
    int message_index = 0;
    std::string line;
    std::string file_data; // The data to be rewritten to the file, except the message that we want to reset.
    std::string message; // The messages gotten from the file.
    while (std::getline(file_in, line)) {
        message += line + "\n";

        if (line == ".") {
            if (message_nummer != message_index) { 
                file_data += message;
            }
            message_index++;
            message.clear();
        }
    }
    file_in.close();

    if (message_nummer >= message_index) {
        // Message Nummer out of range.
        return -1;
    }

    // Open a file to write the new data without the deleted message.
    std::ofstream file_out(file_name);
    if (!file_out.is_open()) {
        return -1;
    }
    file_out.write(file_data.c_str(), file_data.length());
    file_out.close();
    
    return EXIT_SUCCESS;
}