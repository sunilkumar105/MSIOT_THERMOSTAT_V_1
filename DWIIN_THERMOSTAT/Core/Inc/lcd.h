#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stdbool.h"

void dwinRx(uint8_t c);
extern UART_HandleTypeDef huart1;

void hideFans();
void vpDataSend(uint16_t vpAddress, uint16_t data);
void dwinPageChange(uint16_t pageNumber);
void clear();
void showHumidity(uint8_t percentage);
void showTemperature(uint8_t temp);
void update_dwin_lcd(void);

#endif
