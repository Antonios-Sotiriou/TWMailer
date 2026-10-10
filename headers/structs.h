#ifndef STRUCTS_H
#define STRUCTS_H 1

typedef struct {
    char *method, *sender, *receiver, subject[81], *message;
} TWMail;

#endif // !STRUCTS_H