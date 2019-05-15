#pragma once

#include "../../Source/G_System/GI_Static.cpp"
#include "../../Interface/G_System/GBufferedInput.h"

#include <algorithm>
#include <vector>

using namespace GW;
using namespace CORE;
using namespace SYSTEM;

namespace {

	// vector of Listeners to send event information to.
	// ordering is important for event listeners so we need to switch containers
	std::vector<std::pair<GListener *, unsigned long long>> _listeners;
    unsigned int keyMask;



#ifdef _WIN32

	//Variables

	//! Store the users implementation of the windows procedure.
	LONG_PTR _userWinProc;


	//Methods
	LRESULT CALLBACK GWinProc(HWND window, unsigned int msg, WPARAM wp, LPARAM lp) {
		switch (msg)
		{

		case WM_INPUT:
		{
			UINT dwSize;

			//Get the size of RawInput
			GetRawInputData((HRAWINPUT)lp, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

			LPBYTE lpb = new BYTE[dwSize];

			if (GetRawInputData((HRAWINPUT)lp, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
				//std::cout << "GetRawInputData does not return correct size.\n";
			}

			RAWINPUT* raw = (RAWINPUT*)lpb;

			int _event = -1;
			int data = -1;

			GBUFFEREDINPUT_EVENT_DATA _dataStruct;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				//Get G_KEY
				data = Keycodes[raw->data.keyboard.MakeCode][0];

				//Set state released or pressed.
				switch (raw->data.keyboard.Message) {
				case 256:
					_event = KEYPRESSED;
					switch (data) {
					case G_KEY_RIGHTSHIFT:
					case G_KEY_LEFTSHIFT:
						TURNON_BIT(keyMask, G_MASK_SHIFT);
						break;
					case G_KEY_CONTROL:
						TURNON_BIT(keyMask, G_MASK_CONTROL);
						break;
					case G_KEY_CAPSLOCK:
						TOGGLE_BIT(keyMask, G_MASK_CAPS_LOCK);
						break;
					case G_KEY_NUMLOCK:
						TOGGLE_BIT(keyMask, G_MASK_NUM_LOCK);
						break;
					case G_KEY_SCROLL_LOCK:
						TOGGLE_BIT(keyMask, G_MASK_SCROLL_LOCK);
						break;
					}
					break;
				case 257:
					_event = KEYRELEASED;
					switch (data) {
					case G_KEY_RIGHTSHIFT:
					case G_KEY_LEFTSHIFT:
						TURNOFF_BIT(keyMask, G_MASK_SHIFT);
						break;
					case G_KEY_CONTROL:
						TURNOFF_BIT(keyMask, G_MASK_CONTROL);
						break;
					}
					break;
				}
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{

				//Set Code
				switch (raw->data.mouse.ulButtons) {
				case 1:
				case 2:
					data = G_BUTTON_LEFT;
					break;
				case 4:
				case 8:
					data = G_BUTTON_RIGHT;
					break;
				case 16:
				case 32:
					data = G_BUTTON_MIDDLE;
					break;
				}

				switch (raw->data.mouse.usButtonData) {
				case 120:
					data = G_MOUSE_SCROLL_UP;
					break;
				case 65416:
					data = G_MOUSE_SCROLL_DOWN;
					break;
				}


				switch (raw->data.mouse.usButtonFlags) {
					//Mouse Pressed
				case 1:
				case 4:
				case 32:
					_event = BUTTONPRESSED;
					break;
					//Mouse Released
				case 2:
				case 8:
				case 16:
					_event = BUTTONRELEASED;
					break;
					//Scroll
				case 1024:
					_event = MOUSESCROLL;
					break;
				}

			}
			_dataStruct.data = data;

			POINT p;
			if (GetCursorPos(&p))
			{
				_dataStruct.screenX = p.x;
				_dataStruct.screenY = p.y;
			}
			if (ScreenToClient(window, &p))
			{
				_dataStruct.x = p.x;
				_dataStruct.y = p.y;
			}

			_dataStruct.keyMask = keyMask;

			if (_dataStruct.data != -1 && _event != -1) {
				std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
				for (; iter != _listeners.end(); ++iter) {
					iter->first->OnEvent(GBufferedInputUUIID, _event, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
				}
			}

			delete[] lpb;
		}
		break;

		default:

			break;
		}
		return CallWindowProcW((WNDPROC)_userWinProc, window, msg, wp, lp);
	}
#endif

#ifdef __linux__

    Window _window;
    unsigned int n_Keys[256];
    unsigned int n_Buttons[32];

#endif // __linux__

#ifdef __APPLE__

//#include "../../Interface/G_System/GResponder.h"

    GResponder * responder = [GResponder alloc];

#endif

}
