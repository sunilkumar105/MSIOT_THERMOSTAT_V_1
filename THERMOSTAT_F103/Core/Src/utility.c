/*
 * utility.c
 *
 *  Created on: 22-May-2023
 *      Author: sunil
 */
#include "utility.h"
#include <stdio.h>
#include <string.h>
#include "AHT21.h"
#include <math.h>
#include "MY_NRF24.h"
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"
#include "stm32f1xx_hal.h"

#define huart6 huart2  //replace it later

float previous_temp_from_sensor = 1;
uint32_t previous_humidity_from_sensor = 1;
uint32_t time_2_update_temperature_and_humidity;

char my_node_id[2] = "01";

bool current_power_status = power_status_on;
uint8_t current_fan_mode = fan_mode_auto;
uint8_t current_master_mode = master_mode_auto;
uint8_t current_compressor_status;
uint8_t prev_compressor_status;

const uint64_t Reading_Writing_Pipe_Address = 0x11223344AA;

extern UART_HandleTypeDef huart6;
extern SPI_HandleTypeDef hspi2;

void read_and_update_current_temperature();
void read_and_update_current_humidity();
void process_read_command_from_server(char *received_data_from_gateway,
		char *return_string);
void Switch_to_Transmitt_mode();
void Switch_to_Receiver_mode(void);
void Transmitt_reply_to_gateway(char *msg);

float current_set_room_temperature = 22;
float last_set_room_temperature = 22;
float sensor_temperature = 1;
uint32_t sensor_humidity = 50;

void thermostat_display_initial_setup(void) { //Draw thermostat_display_initial_setup
	HAL_StatusTypeDef ret = AHT21_init();
	HAL_Delay(500);
	while (ret != HAL_OK) {
		ret = AHT21_init();
		HAL_GPIO_TogglePin(LED_HEALTH_PIN_GPIO_Port, LED_HEALTH_PIN_Pin);
		HAL_Delay(50);
	}
	HAL_Delay(100);
	read_and_update_current_temperature();
	read_and_update_current_humidity();
	current_power_status = power_status_on;
	current_fan_mode = fan_mode_auto;
	hideFans();
	set_fan_speed_according_to_temp_diff();
	set_compressor_according_to_temp_diff();
}
void adjust_temperature(void) {
	time_2_update_temperature_and_humidity++;
	if (time_2_update_temperature_and_humidity > 30) {
		HAL_GPIO_TogglePin(LED_HEALTH_PIN_GPIO_Port, LED_HEALTH_PIN_Pin);
		read_and_update_current_temperature();
		read_and_update_current_humidity();
		time_2_update_temperature_and_humidity = 0;
	}
	//SETTING COMPRESSOR ACCORDING TO TEMP DIFF
	prev_compressor_status = current_compressor_status;
	if ((current_master_mode != master_mode_only_fan) //IF MASTER MODE IS NOT IN VENT AND POWER STATUS IS ON
	&& (current_power_status == power_status_on)) {
		if (current_set_room_temperature > sensor_temperature) {
			current_compressor_status = compressor_off;
			set_compressor(compressor_off);
		} else if (current_set_room_temperature == sensor_temperature) {
			current_compressor_status = compressor_off;
			set_compressor(compressor_off);
		} else if (current_set_room_temperature < sensor_temperature) {
			current_compressor_status = compressor_on;
			set_compressor(compressor_on);
		}
		set_fan_speed_according_to_temp_diff();
	}

	if (prev_compressor_status != current_compressor_status) { //SET COOLING STATUS
		draw_cooling_on_off(current_compressor_status);
	} else if ((current_master_mode == master_mode_only_fan)
			&& (current_power_status == power_status_on)) { //IF MASTER MODE IS VEN >> turn off comp
		set_compressor(compressor_off);
		current_compressor_status = compressor_off;
	}
}

//SETTING FAN ACC TO TEMP DIFF
void set_fan_speed_according_to_temp_diff(void) {
	if (current_fan_mode == fan_mode_auto) {
		if ((current_set_room_temperature - sensor_temperature) <= -2.5) {
			set_fan(fan1_off, fan2_off, fan3_on);
		} else if ((current_set_room_temperature - sensor_temperature)
				<= -1.5) {
			set_fan(fan1_off, fan2_on, fan3_off);
		} else if ((current_set_room_temperature - sensor_temperature)
				<= -0.5) {
			set_fan(fan1_on, fan2_off, fan3_off);
		} else if ((current_set_room_temperature - sensor_temperature) == 0) {
			set_fan(fan1_off, fan2_off, fan3_off);
		} else if ((current_set_room_temperature - sensor_temperature) >= 0) {
			set_fan(fan1_off, fan2_off, fan3_off);
		}
	} else {
//		set_fan(fan1_off, fan2_off, fan3_off);
	}
}
void set_compressor_according_to_temp_diff(void) {
	if (current_master_mode != master_mode_only_fan) {
		if (current_set_room_temperature > sensor_temperature) {
			set_compressor(compressor_off);
		} else if (current_set_room_temperature == sensor_temperature) {
			set_compressor(compressor_off);
		} else if (current_set_room_temperature < sensor_temperature) {
			set_compressor(compressor_on);
		}
	}
}
void draw_cooling_on_off(uint8_t status_of_compre) {
	if (current_power_status) {
		switch (status_of_compre) {
		case compressor_on:
			break;
		case compressor_off:
			break;
		default:
			break;
		}
	}
}

void read_and_update_current_temperature() {
	sensor_temperature = AHT21_Read_Temperature();
	sensor_temperature = ceilf(sensor_temperature * 10) / 10;
	if (previous_temp_from_sensor != sensor_temperature) {
		showTemperature((int) sensor_temperature);
	}
	previous_temp_from_sensor = sensor_temperature;
}
void read_and_update_current_humidity() {
	sensor_humidity = AHT21_Read_Humidity();
	if (previous_humidity_from_sensor != sensor_humidity) {
		showHumidity((int) sensor_humidity);
	}
	previous_humidity_from_sensor = sensor_humidity;
}

////////===========NRF CODE===============///////////
void NRF_init_as_thermo_node(void) {
	NRF24_begin(NRF_CE_GPIO_Port, NRF_CSN_Pin, NRF_CE_Pin, hspi2);
	nrf24_DebugUART_Init(huart6);
	NRF24_setAutoAck(true);
	NRF24_setChannel(My_Channel_ID);
	NRF24_setPayloadSize(PayLoad_Size_In_Byte);
	NRF24_openReadingPipe(Reading_Writing_Pipe_Num,
			Reading_Writing_Pipe_Address);
	NRF24_startListening();
	printRadioSettings();
}

void Manage_NRF_Data() {
	char received_data_from_gateway[50] = "";
	HAL_UART_Transmit(&huart6, (uint8_t*) "\n DATA RECEIVED FROM NRF::>> ",
			strlen("\n DATA RECEIVED FROM NRF::>> "), 10);
	NRF24_read(received_data_from_gateway, 5); //store received data
	HAL_UART_Transmit(&huart6, (uint8_t*) received_data_from_gateway, 5, 10);
	char rcvd_node_id[2];
	char rcvd_command;
	memcpy(rcvd_node_id, received_data_from_gateway, 2);
	rcvd_command = received_data_from_gateway[2];

	//============CHECK IF NODE MATCH WITH ID======================//
	if (!(memcmp(rcvd_node_id, my_node_id, 2))) {
		HAL_UART_Transmit(&huart6, (uint8_t*) "\n NODE ID MATCHED",
				strlen("\n NODE ID MATCHED"), 10);
		HAL_UART_Transmit(&huart6, (uint8_t*) "\n COMMAND ::> ",
				strlen("\n COMMAND ::> "), 10);
		HAL_UART_Transmit(&huart6, (uint8_t*) (uintptr_t) rcvd_command, 1, 10);
	}

	else {
		return;
	}
	//==========if received command is to SET TEMPERATURE===============//
	if (rcvd_command == '0') {
		char temperture_to_write[2];
		temperture_to_write[0] = received_data_from_gateway[3];
		temperture_to_write[1] = received_data_from_gateway[4];

		float temperature_from_gateway = atof(temperture_to_write);
		current_set_room_temperature = temperature_from_gateway;
		vpDataSend(0x1003, (int) current_set_room_temperature);
		current_set_room_temperature = temperature_from_gateway;
		HAL_UART_Transmit(&huart6, (uint8_t*) "\n TEMPERATURE UPDATED",
				strlen("\n TEMPERATURE UPDATED"), 10);
	}
	//==================if received command is to GET TEMPERATURE/HUMIDITY============//
	else if (rcvd_command == '1') {
		//pass received_data_from_gateway and retrun the final string
		char return_string[10];
		process_read_command_from_server(received_data_from_gateway,
				return_string);
		char return_string1[10];
		memcpy(return_string1, my_node_id, 2);
		return_string1[2] = rcvd_command;
		return_string1[3] = return_string[0];
		return_string1[4] = return_string[1];
		return_string1[5] = return_string[2];
		return_string1[6] = return_string[3];
		return_string1[7] = return_string[4];
		HAL_UART_Transmit(&huart6, (uint8_t*) "\nReturning data ::> ",
				strlen("\nReturning data ::> "), 10);
		HAL_UART_Transmit(&huart6, (uint8_t*) return_string1, 7, 10);
		Switch_to_Transmitt_mode();
		Transmitt_reply_to_gateway(return_string1);
		Switch_to_Receiver_mode();
	}
}
void process_read_command_from_server(char *received_data_from_gateway,
		char *return_string) {
	char read_temperature = '0';
	char read_humidity = '1';
	char read_set_temperature = '2';
	char return_data[6];

	char read_command = received_data_from_gateway[3];

	if (read_command == read_temperature) {  //Read Temp From Sensor
		float temperature = AHT21_Read_Temperature();
		sprintf(return_data, "%f", temperature);
	} else if (read_command == read_humidity) { //Read From humidity
		uint32_t humidity = AHT21_Read_Humidity();
		sprintf(return_data, "%lu", humidity);

	} else if (read_command == read_set_temperature) { //Read Current Set Room Temp
		sprintf(return_data, "%f", current_set_room_temperature);
	} else {
		HAL_UART_Transmit(&huart6, (uint8_t*) "\n INVALID READ COMMAND",
				strlen("\n INVALID READ COMMAND"), 10);
		return;
	}
	HAL_UART_Transmit(&huart6, (uint8_t*) "\nReturning data ::> ",
			strlen("\nReturning data ::> "), 10);
	HAL_UART_Transmit(&huart6, (uint8_t*) return_data, 4, 10);
	strcpy(return_string, return_data);
}
void Switch_to_Transmitt_mode() {
	NRF24_stopListening();
	NRF24_openWritingPipe(Reading_Writing_Pipe_Address);
	NRF24_setChannel(Server_Channel);
}
void Transmitt_reply_to_gateway(char *msg) {
	HAL_UART_Transmit(&huart6,
			(uint8_t*) "\nTransmitting reply to gateway .. waiting for ack",
			strlen("\nTransmitting reply to gateway .. waiting for ack"), 10);
	char received_data_from_srvr1[10];
	memcpy(received_data_from_srvr1, msg, 5);
	for (int i = 0; i <= 10; i++) {
		if (!NRF24_write(received_data_from_srvr1, 32)) {
			NRF24_write(received_data_from_srvr1, 32);
			HAL_UART_Transmit(&huart6, (uint8_t*) ".", strlen("."), 10);
			HAL_Delay(400);
		} else {
			HAL_UART_Transmit(&huart6,
					(uint8_t*) "\nReply Transmitted to gateway successfully",
					strlen("\nReply Transmitted to gateway successfully"), 10);
			return;
		}
	}
	HAL_UART_Transmit(&huart6,
			(uint8_t*) "\nTransmission Failed, No acknowledgement received",
			strlen("\nTransmission Failed, No acknowledgement received"), 10);

}
void Switch_to_Receiver_mode(void) {
	NRF24_setChannel(My_Channel_ID);
	NRF24_openReadingPipe(Reading_Writing_Pipe_Num, Reading_Writing_Pipe_Address);
	NRF24_startListening();
}

//DWIIN DISPPLAY FUNCTIONALITY
void select_fan_speed_HIGH(void) {

}
