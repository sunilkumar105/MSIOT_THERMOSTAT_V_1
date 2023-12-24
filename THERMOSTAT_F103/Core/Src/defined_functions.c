/*
 * defined_functions.c
 *
 *  Created on: May 25, 2023
 *      Author: sunil
 */

#include "defined_functions.h"

enum power_status {
	power_status_on = 1, power_status_off = 0
};
void draw_fan_speed(uint8_t fan_mode) {
	if (fan_mode == fan_mode_auto) {
	} else if (fan_mode == fan_mode_1) {
	} else if (fan_mode == fan_mode_2) {
	} else if (fan_mode == fan_mode_3) {
	}
}
void draw_master_mode(uint8_t master_mode) {
	switch (master_mode) {
	case master_mode_auto:
		break;
	case master_mode_manual:
		break;
	case master_mode_only_fan:
		break;
	}
}
void display_touched_cordinate(uint16_t x, uint16_t y) {
	char buff_x[10], buff_y[10]; //	convert the cordinate to string & print
	sprintf(buff_x, "%hu", x);
	sprintf(buff_y, "%hu", y);
}
void set_compressor(bool compressor_command) {
	if (compressor_command) {
		HAL_GPIO_WritePin(RELAY_COMPRESSOR_GPIO_Port, RELAY_COMPRESSOR_Pin,
				GPIO_PIN_SET);
	} else if (!compressor_command) {
		HAL_GPIO_WritePin(RELAY_COMPRESSOR_GPIO_Port, RELAY_COMPRESSOR_Pin,
				GPIO_PIN_RESET);
	}
}
void set_fan(bool fan1_command, bool fan2_command,
bool fan3_command) {
	//if current mode is manual and temperature is already ACHEIVED , then no need to actuate fan
	if (fan1_command) {
		HAL_GPIO_WritePin(RELAY_FAN_1_GPIO_Port, RELAY_FAN_1_Pin, GPIO_PIN_SET);
	} else if (!fan1_command) {
		HAL_GPIO_WritePin(RELAY_FAN_1_GPIO_Port, RELAY_FAN_1_Pin,
				GPIO_PIN_RESET);
	}
	if (fan2_command) {
		HAL_GPIO_WritePin(RELAY_FAN_2_GPIO_Port, RELAY_FAN_2_Pin, GPIO_PIN_SET);
	} else if (!fan2_command) {
		HAL_GPIO_WritePin(RELAY_FAN_2_GPIO_Port, RELAY_FAN_2_Pin,
				GPIO_PIN_RESET);
	}
	if (fan3_command) {
		HAL_GPIO_WritePin(RELAY_FAN_3_GPIO_Port, RELAY_FAN_3_Pin, GPIO_PIN_SET);
	} else if (!fan3_command) {
		HAL_GPIO_WritePin(RELAY_FAN_3_GPIO_Port, RELAY_FAN_3_Pin,
				GPIO_PIN_RESET);
	}
}

