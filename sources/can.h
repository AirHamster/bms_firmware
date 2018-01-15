#ifndef CAN_H
#define CAN_H

#include "defines.h"
#include "usart.h"

void can_setup(void); 
void can_send_test(uint8_t number, uint16_t msg);

#endif
