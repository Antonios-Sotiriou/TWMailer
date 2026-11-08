#ifndef GENERAL_FUNCS_H
#define GENERAL_FUNCS_H 1

#include <string>

//Speichert die Antwort für den Client
int dispatchUserRequest(std::string data, std::string mail_spool_directory, std::string &response); 
int send(std::string data, std::string mail_spool_directory);
//Liest die Nachrichten eines Benutzers
int list(std::string data, std::string mail_spool_directory, std::string &response); 
// Liest eine bestimmte Nachricht
int readMessage(std::string data, std::string mail_spool_directory, std::string &response); 
//Prüft, ob der Benutzername gültig ist.
bool isValidUsername(const std::string &username);
int del(std::string data, std::string mail_spool_directory);
#endif // !GENERAL_FUNCS_H