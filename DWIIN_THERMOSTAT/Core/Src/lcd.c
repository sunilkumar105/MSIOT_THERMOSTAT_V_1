#include "lcd.h"
#include "utility.h"

extern float current_set_room_temperature;
extern uint8_t current_fan_mode;
extern bool current_power_status;
extern uint8_t current_master_mode;

bool show = false;
uint8_t per = 50;
uint8_t temp = 25;
uint8_t test = 0;

typedef enum {
	AUTO, MANUAL, VENT
} Mode;
Mode mode;

int dwinState = 0;
int dwinLen = 0;
int dwinInst = 0;
int dwinVp[2];
int dwinData[30];
int dwinError = 0;
int dwindDataPending = 0;
int dwinIter = 0;

void dwinRx(uint8_t c) {
	switch (dwinState) {
	case 0:
		if (c == 0x5a) {
			dwinState = 1;
		}
		break;
	case 1:
		if (c == 0xa5) {
			dwinState = 2;
		} else {
			dwinError = 2;
			dwinState = 0;
		}
		break;
	case 2:
		dwinLen = c;
		// overflow
		if (dwinLen > 30) {
			dwinError = 3; // overflow
			dwinState = 0;
		} else {
			dwinState = 3;
		}
		break;
	case 3:
		dwinInst = c;
		if (dwinInst == 0x80 || dwinInst == 0x81 || dwinInst == 0x82
				|| dwinInst == 0x83 || dwinInst == 0x84 || dwinInst == 0x86
				|| dwinInst == 0x87) {
			dwinState = 4;
		} else {
			dwinError = 4;
			dwinState = 0;
		}
		break;
	case 4:
		dwinVp[0] = c;
		dwinState = 5;
		break;
	case 5:
		dwinVp[1] = c;
		dwinLen = dwinLen - 3;
		if (dwinLen == 0) {
			// Ok condition, send when flash read
			dwinState = 0;
		} else {
			dwinState = 6;
		}
		dwinIter = 0;
		break;
	case 6:
		dwinData[dwinIter++] = c;
		if (dwinIter >= dwinLen) {
			dwindDataPending = 1;
			dwinState = 0; // reset state
		} else if (dwinIter >= 25) {
			dwinIter = 0;
			dwinState = 0; // reset state
			dwinError = 1;
		}
		break;
	default:
		break;
	}
}

//  .. lcd.c ends
void vpDataSend(uint16_t vpAddress, uint16_t data) {
	uint8_t cmd[] = { 0x5a, 0xa5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x00 };
	//cmd[8]; // TODO, high bit
	cmd[4] = vpAddress >> 8;
	cmd[5] = (vpAddress & 0x00FF);
	cmd[6] = (data >> 8);
	cmd[7] = (data & 0x00FF);
	HAL_UART_Transmit(&huart1, cmd, 8, 10);
	HAL_Delay(50);
}
void dwinPageChange(uint16_t pageNumber) {
	uint8_t cmd[] =
			{ 0x5a, 0xa5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, 0x00 };
	cmd[9] = (pageNumber & 0x00FF);
	HAL_UART_Transmit(&huart1, cmd, 10, 100);
	HAL_Delay(100);
}
void clear() {
	vpDataSend(0x1006, 4);
	vpDataSend(0x1007, 6);
	vpDataSend(0x1008, 8);
	vpDataSend(0x1009, 10);
}
void showHumidity(uint8_t percentage) {
	vpDataSend(0x1005, percentage);
}
void showTemperature(uint8_t temp) {
	vpDataSend(0x1004, temp);
}
void showStartComp() {

	vpDataSend(0x1001, 0);
	vpDataSend(0x1002, 3);
}

void showStopComp() {
	vpDataSend(0x1002, 2);
	vpDataSend(0x1001, 1);
}
void hideFans() {
	vpDataSend(0x1006, 12);
	vpDataSend(0x1007, 12);
	vpDataSend(0x1008, 12);
}

void showFans() {
	vpDataSend(0x1006, 4);
	vpDataSend(0x1007, 6);
	vpDataSend(0x1008, 8);
}

void update_dwin_lcd(void) {
	if (dwindDataPending) {
		if (dwinVp[0] == 0x10 && dwinVp[1] == 0x00) {
			switch (dwinData[2]) {
			case 0x01:
				current_set_room_temperature++;
				vpDataSend(0x1003, (int) current_set_room_temperature);

				break;
			case 0x02:
				current_set_room_temperature--;
				vpDataSend(0x1003, (int) current_set_room_temperature);

				break;
			case 0x03:
				//fast fan speed selected
				if (mode == MANUAL || mode == VENT) {
					set_fan(fan1_off, fan2_off, fan3_on);
					clear();
					vpDataSend(0x1006, 5);
				}
				break;
			case 0x04:
				//medium fan speed selected
				if (mode == MANUAL || mode == VENT) {
					current_fan_mode = fan_mode_2;
					set_fan(fan1_off, fan2_on, fan3_off);
					clear();
					vpDataSend(0x1007, 7);
				}
				break;
			case 0x05:
				if (mode == MANUAL || mode == VENT) {
					current_fan_mode = fan_mode_1;
					set_fan(fan1_on, fan2_off, fan3_off);
					//slow fan speed selected
					clear();
					vpDataSend(0x1008, 9);
				}
				break;
			case 0x06:
				//AUTO BUTTON
				current_fan_mode = fan_mode_auto;
				clear();
				vpDataSend(0x1009, 11);
				break;
			case 0x07:
				//POWER BUTTON PRESSED stop all functionality
				set_fan(fan1_off, fan2_off, fan3_off);
				set_compressor(compressor_off);
				current_power_status = power_status_off;
				dwinPageChange(2);
				break;
			case 0x0A:
				dwinPageChange(1);
				thermostat_display_initial_setup();
				current_power_status = power_status_on;
				break;
			default:
				break;
			}
		}
		if (dwinVp[0] == 0x10 && dwinVp[1] == 0x0A) {
			switch (dwinData[2]) {
			case 13:
				mode = AUTO;
				hideFans();
				current_master_mode = master_mode_auto;
				current_fan_mode = fan_mode_auto;
				break;
			case 14:
				mode = MANUAL;
				current_master_mode = master_mode_manual;
				showFans();
				break;
			case 15:
				mode = VENT;
				current_master_mode = master_mode_only_fan;
				set_fan(fan1_off, fan2_on, fan3_off);
				vpDataSend(0x1007, 7);
				showFans();
				break;
			default:
				break;
			}
		}
		dwindDataPending = 0;
	} //end pending

	if (show) {
		showHumidity(per);
		showTemperature(temp);
		show = false;
	}
	if (test == 1) {
		showStartComp();
		test = 0;
	}

	if (test == 2) {
		showStopComp();
		test = 0;
	}
}

