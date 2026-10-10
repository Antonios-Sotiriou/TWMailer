#ifndef GENERAL_FUNCS_H
#define GENERAL_FUNCS_H 1

#include "structs.h"

int parseReceivedData(TWMail *twmail, char data[]);
void dispatchCommand(TWMail *twmail);

#endif // !GENERAL_FUNCS_H