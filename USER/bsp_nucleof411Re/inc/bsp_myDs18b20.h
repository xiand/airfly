#ifndef __BSP_MYDS18B20_H__
#define __BSP_MYDS18B20_H__

#include "bsp.h"

void bsp_myInitDs18b20(void);
uint8_t bsp_ds18b20ReadID(uint8_t *_id);
int16_t bsp_ds18b20ReadTemperature(void);
int16_t bsp_Ds18b20ReadTempByID(uint8_t *_id);


#endif

