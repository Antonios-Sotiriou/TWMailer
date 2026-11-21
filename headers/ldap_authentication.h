#ifndef LDAP_AUTHENTICATION_H
#define LDAP_AUTHENTICATION_H

#include <string>

bool authenticateLdap(const std::string& username, const std::string& password);

#endif