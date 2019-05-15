#ifndef GCONTROLLERINPUTDEFINES_H
#define GCONTROLLERINPUTDEFINES_H

/*!
File: GControllerCodes.h
Purpose: The Gateware controller input codes
Author: Devin Wright
Contributors: N/A
Last Modified: 11/29/2018
Copyright: 7thGate Software LLC.
License: MIT
*/

// Controller IDs
#define G_GENERAL_CONTROLLER			0x00000 
#define G_XBOX_CONTROLLER				0x00001 
#define G_PS4_CONTROLLER				0x00002 

// Input type
#define G_SOUTH_BTN						0x00
#define G_EAST_BTN						0x01
#define G_NORTH_BTN						0x02
#define G_WEST_BTN						0x03
#define G_LEFT_SHOULDER_BTN				0x04
#define G_RIGHT_SHOULDER_BTN			0x05
#define G_LEFT_TRIGGER_AXIS				0x06
#define G_RIGHT_TRIGGER_AXIS			0x07
#define G_DPAD_LEFT_BTN					0x08
#define G_DPAD_RIGHT_BTN				0x09
#define G_DPAD_UP_BTN					0x0A
#define G_DPAD_DOWN_BTN					0x0B
#define G_LEFT_THUMB_BTN				0x0C
#define G_RIGHT_THUMB_BTN				0x0D
#define G_START_BTN						0x0E
#define G_SELECT_BTN					0x0F
#define G_LX_AXIS						0x10
#define G_LY_AXIS						0x11
#define G_RX_AXIS						0x12
#define G_RY_AXIS						0x13
#define G_UNKOWN_INPUT                  0xff // Should never be received

#endif 
