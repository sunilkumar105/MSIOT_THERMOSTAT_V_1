/*
 * utility.h
 *
 *  Created on: 22-May-2023
 *      Author: sunil
 */

#ifndef INC_UTILITY_H_
#define INC_UTILITY_H_

#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include "defined_functions.h"
#include "lcd.h"

#define MY_NODE_ID 1
#define My_Channel_ID 55
#define Server_Channel 52
#define PayLoad_Size_In_Byte 32
#define Reading_Writing_Pipe_Num 1


enum power_status {
	power_status_on = 1, power_status_off = 0
};

enum fan_command {
	fan1_off = 0,
	fan2_off = 0,
	fan3_off = 0,
	fan1_on = 1,
	fan2_on = 1,
	fan3_on = 1
};
enum compressor_command {
	compressor_off = 0, compressor_on = 1
};

void draw_cooling_on_off(uint8_t status_of_compre);
void Manage_NRF_Data(void);
//draw initial setup
void thermostat_display_initial_setup(void);
//Touch Control
void manage_touch_control(uint16_t x, uint16_t y);
void adjust_temperature(void);

void set_fan_speed_according_to_temp_diff(void);
void set_compressor_according_to_temp_diff(void);

void NRF_init_as_thermo_node(void);

#endif /* INC_UTILITY_H_ */
