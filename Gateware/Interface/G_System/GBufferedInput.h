#ifndef GBUFFEREDINPUT_H
#define GBUFFEREDINPUT_H

/*!
	File: GBufferedInput.h
	Purpose: This Interface offers event based thread safe raw buffered input.
	Author: Peter Farber
	Contributors: N/A
	Last Modified: 11/16/2016
	Interface Status: Beta
	Copyright: 7thGate Software LLC.
	License: MIT
*/

//! GBufferedInput inherits directly from GBroadcasting.
#include "../G_Core/GBroadcasting.h"
#include "../G_System/GKeyDefines.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The namespace to which all Gateware library interfaces must belong.
	namespace SYSTEM
	{

//! Ensure identical binary padding for structures on all platforms.
#pragma pack(push, 1)

		//! GBUFFEREDINPUT_EVENT_DATA will hold any information you may need about an Input Event.
		struct GBUFFEREDINPUT_EVENT_DATA
		{
			int data;				/*!<  Data storing the key/button information. */ 
			int x;					/*!<  Window Mouse position x when event is sent. */
			int y;					/*!<  Window Mouse position y when event is sent. */
			int screenX;			/*!<  Screen Mouse position x when event is sent. */
			int screenY;			/*!<  Screen Mouse position y when event is sent. */
			unsigned int keyMask;	/*!<  Bit flags for (CapsLock, NumLock, ScrollLock, Shift, and Control).
										  See GKeyDefines.h for list of keyMask defines. */
		};

#pragma pack(pop)

		//! GBufferedInputEvents holds the possible events that can be sent from GBufferedInput.
		enum GBufferedInputEvents {
			KEYPRESSED,				/*!<  Key pressed event. */ 
			KEYRELEASED,			/*!<  Key released event. */ 
			BUTTONPRESSED,			/*!<  Mouse/Pointer Button pressed event. */ 
			BUTTONRELEASED,			/*!<  Mouse/Pointer Button released event. */ 
			MOUSESCROLL,			/*!<  Mouse/Pointer scroll event. */ 
		};

		//! Unique Identifier for this interface. {4CBA9D69-1B32-43DA-B7B2-A421C57818F0}
		static const GUUIID GBufferedInputUUIID =
		{
			0x4cba9d69, 0x1b32, 0x43da,{ 0xb7, 0xb2, 0xa4, 0x21, 0xc5, 0x78, 0x18, 0xf0 }
		};

		//! A Multi-threaded buffered input library.
		/*!
		*	Register with a GBufferedInput to receive mouse and keyboard events.
		*/
		class GBufferedInput : public CORE::GBroadcasting { };

		//! Creates a GBufferedInput Object.
		/*!
		*	Initializes a handle to a window based on the void* data passed in. The created 
		*	GBufferedInput object will have its reference count initialized to one.
		*
		*	\param [in] _windowHandle (Windows) The handle to the window (HWND).
		*	\param [in] _windowHandle (Linux) LINUX_WINDOW data.
		*	\param [in] _windowHandle (Mac) NSWindow data.
		*	\param [in] _handleSize The size of the handle to the window in bytes.
		*	\param [out] _outBufferedInput Will contain the GBufferedInput object if successfully created.
		*
		*	\retval SUCCESS no problems found.
		*	\retval FAILURE could not make an BufferedInput Object.
		*	\retval INVALID_ARGUMENT _outInput and or _windowHandle is nullptr.
		*/
		GATEWARE_EXPORT_IMPLICIT GReturn CreateGBufferedInput(void* _windowHandle, unsigned int _handleSize, GBufferedInput** _outBufferedInput);
	} // end SYSTEM namespace
} // end GW namespace

#endif