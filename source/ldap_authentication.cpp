#include "../headers/ldap_authentication.h"
#include "../headers/general_funcs.h"
#include <ldap.h>
#include <stdio.h>


//Pruft, ob ein Benutzer mit seinen LDAP-Zugangsdaten gultig ist
bool authenticateLdap(
    const std::string &username,
    const std::string &password)
{

    // Benutzername und Passwort uberprufen.
    if (username.empty() || !isValidUsername(username) || password.empty())
    {
    return false;
    }

    //Adresse des LDAP-Servers mit Port
    const char *ldapUri =
        "ldap://ldap.technikum-wien.at:389";

    const int ldapVersion = LDAP_VERSION3;

    const char *userBaseDN =
        ",ou=people,dc=technikum-wien,dc=at";

    //LDAP-Verbindung initialisieren
    LDAP *ldapHandle = nullptr;

    int result = ldap_initialize(&ldapHandle, ldapUri);

    if (result != LDAP_SUCCESS)
    {
        fprintf(stderr, "LDAP initialization failed: %s\n",
                ldap_err2string(result));
        return false;
    }

    //LDAP-Protokollversion einstellen
    result = ldap_set_option(
        ldapHandle,
        LDAP_OPT_PROTOCOL_VERSION,
        &ldapVersion);

    if (result != LDAP_OPT_SUCCESS)
    {
        fprintf(stderr, "ldap_set_option(PROTOCOL_VERSION): %s\n",
                ldap_err2string(result));

        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return false;
    }

    //Sichere Verbindung mit StartTLS herstellen
    result = ldap_start_tls_s(
        ldapHandle,
        NULL,
        NULL);

    if (result != LDAP_SUCCESS)
    {
        fprintf(stderr, "ldap_start_tls_s(): %s\n",
                ldap_err2string(result));

        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return false;
    }

    //DN erstellen
    std::string userDN =
    "uid=" + username + userBaseDN;

    //Passwort for LDAP Bind vorbereiten
    std::string passwordCopy = password;

    struct berval bindCredentials {}; //OpenLDAP structure (Basic Encoding Rules Value)

    bindCredentials.bv_val = &passwordCopy[0];
    bindCredentials.bv_len = passwordCopy.length();

    //BIND
    //Benutzer mit LDAP authentifizieren
    result = ldap_sasl_bind_s(
        ldapHandle,
        userDN.c_str(),
        LDAP_SASL_SIMPLE,
        &bindCredentials,
        NULL,
        NULL,
        NULL);

    if (result != LDAP_SUCCESS) //Bind uberprufen
    {
        fprintf(stderr, "LDAP bind error: %s\n",
                ldap_err2string(result));

        ldap_unbind_ext_s(ldapHandle, NULL, NULL);
        return false;
    }

    //LDAP-Verbindung schliessen
    ldap_unbind_ext_s(ldapHandle, NULL, NULL);

    return true;

    
}