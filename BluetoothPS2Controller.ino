/*
 * Copyright (c) 2016 Evan Kale
 * Email: EvanKale91@gmail.com
 * Website: www.ISeeDeadPixel.com
 *          www.evankale.blogspot.ca
 *
 * This file is part of BluetoothPS2Controller.
 *
 * BluetoothPS2Controller is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "PS2X_lib.h"

#define PS2_DAT        11
#define PS2_CMD        10
#define PS2_ATT        5
#define PS2_CLK        4

#define ANALOG_DEADZONE 25

#define BLUETOOTH_SERIAL_RATE 9600

PS2X ps2x;

bool gamepadInitialized = false;

uint32_t lastBtnState = 0;
int8_t lastX1 = 0;
int8_t lastY1 = 0;
int8_t lastX2 = 0;
int8_t lastY2 = 0;

uint32_t btnState = 0;
int8_t x1 = 0;
int8_t y1 = 0;
int8_t x2 = 0;
int8_t y2 = 0;

void setup()
{
	Serial.begin(BLUETOOTH_SERIAL_RATE);

	delay(500);

	uint8_t error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, false, false);

	if (!error)
	{
		gamepadInitialized = true;
	}
}

void loop()
{
	if (gamepadInitialized)
	{
		ps2x.read_gamepad();
		btnState = ps2x.ButtonDataByte();
		x1 = (int8_t)(ps2x.Analog(PSS_LX) - 128);
		y1 = (int8_t)(ps2x.Analog(PSS_LY) - 128);
		x2 = (int8_t)(ps2x.Analog(PSS_RX) - 128);
		y2 = (int8_t)(ps2x.Analog(PSS_RY) - 128);

		if (-ANALOG_DEADZONE <= x1 && x1 <= ANALOG_DEADZONE)
			x1 = 0;
		if (-ANALOG_DEADZONE <= y1 && y1 <= ANALOG_DEADZONE)
			y1 = 0;
		if (-ANALOG_DEADZONE <= x2 && x2 <= ANALOG_DEADZONE)
			x2 = 0;
		if (-ANALOG_DEADZONE <= y2 && y2 <= ANALOG_DEADZONE)
			y2 = 0;
		
		if (lastBtnState != btnState
			|| lastX1 != x1 || lastY1 != y1
			|| lastX2 != x2 || lastY2 != y2)
		{
			sendGamepadState(btnState, x1, y1, x2, y2);

			lastBtnState = btnState;
			lastX1 = x1;
			lastY1 = y1;
			lastX2 = x2;
			lastY2 = y2;
		}
	}
	delay(13);
}

void sendGamepadState(uint32_t btnState, int8_t x1, int8_t y1, int8_t x2, int8_t y2)
{
	Serial.write((uint8_t)0xFD);
	Serial.write((uint8_t)0x06);
	Serial.write((uint8_t)x1);
	Serial.write((uint8_t)y1);
	Serial.write((uint8_t)x2);
	Serial.write((uint8_t)y2);
	uint8_t btnState1 = btnState & 0xFF;
	uint8_t btnState2 = (btnState >> 8) & 0xFF;
	Serial.write((uint8_t)btnState1);
	Serial.write((uint8_t)btnState2);
}
