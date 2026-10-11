#ifndef GENERAL_FUNCS_H
#define GENERAL_FUNCS_H 1

#include <string>

int dispatchUserRequest(std::string data, std::string mail_spool_directory);
int send(std::string data, std::string mail_spool_directory);

#endif // !GENERAL_FUNCS_H