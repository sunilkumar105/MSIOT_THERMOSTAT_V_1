/*
 * defined_functions.h
 *
 *  Created on: May 25, 2023
 *      Author: sunil
 */

#ifndef INC_DEFINED_FUNCTIONS_H_
#define INC_DEFINED_FUNCTIONS_H_

#include "main.h"
#include <stdio.h>

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <string.h>

//extern FONT4;
void draw_power_btn(uint8_t state);
void draw_humidity(uint32_t humidity);
void draw_plus_minus(void);
void draw_sensor_temperature(float sensor_temperature);
void toggle_power(void);
void set_compressor(bool compressor_command);
void set_fan(bool fan1_command, bool fan2_command,
bool fan3_command);
void display_touched_cordinate(uint16_t x, uint16_t y);
void draw_master_mode(uint8_t master_mode);
void draw_fan_speed(uint8_t fan_mode);
void set_room_temperature(uint8_t current_temperature);

enum fan_mode {
	fan_mode_auto = 0, fan_mode_1 = 1, fan_mode_2 = 2, fan_mode_3 = 3
};
enum master_mode {
	master_mode_auto = 1, master_mode_manual = 2, master_mode_only_fan = 3
};
#endif /* INC_DEFINED_FUNCTIONS_H_ */
