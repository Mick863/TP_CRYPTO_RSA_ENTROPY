#ifndef IO_H
#define IO_H

#include <inttypes.h>

// Définit le numéro de broche utilisé pour la communication série t=0
#define IOPIN 4

// Prototype de la fonction : envoie un octet via le protocole t=0
void sendbytet0(uint8_t b);

// Prototype de la fonction : reçoit un octet via le protocole t=0
uint8_t recbytet0(void);

#endif // IO_H
