#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdint.h>

// Commande APDU
extern uint8_t cla, ins, p1, p2, p3;

// Status Word
extern uint8_t sw1, sw2;

// Registre de contrôle de reset (simulé)
extern uint8_t MCUSR;

// Variable utilisée pour stocker la valeur de MCUSR
extern uint8_t y_y;

#endif // GLOBALS_H

