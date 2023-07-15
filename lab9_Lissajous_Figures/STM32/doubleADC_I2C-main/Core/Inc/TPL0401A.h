#ifndef __TPL0401A_H
#define __TPL0401A_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define TPL0401A_ADDR 	0x2E

void TPL0401A_Init(void);
// uint8_t Get_TPL0401A_Value(void);
void Set_TPL0401A_Value(uint8_t num);

#endif
