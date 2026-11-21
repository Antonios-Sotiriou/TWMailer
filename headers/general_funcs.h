#ifndef GENERAL_FUNCS_H
#define GENERAL_FUNCS_H 1

#include <string>

// Speichert die Antwort für den Client
int dispatchUserRequest(std::string data, std::string mail_spool_directory, std::string &response); 
// Prüft, ob der Benutzername gültig ist.
bool isValidUsername(const std::string &username);
#endif // !GENERAL_FUNCS_H