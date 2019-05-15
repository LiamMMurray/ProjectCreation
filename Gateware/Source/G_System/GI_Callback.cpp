#include "../../Source/G_System/GI_Static.cpp"
#include "../../Interface/G_System/GInput.h"
#include <queue>
#include <unordered_set>

using namespace GW;
using namespace CORE;

namespace {


unsigned int n_Keys[256];
int _mousePrevX = 0;
int _mousePrevY = 0;
int _mousePositionX = 0;
int _mousePositionY = 0;
int _mouseScreenPositionX = 0;
int _mouseScreenPositionY = 0;
int _mouseDeltaX = 0;
int _mouseDeltaY = 0;
unsigned int keyMask;

unsigned int mouseReadCount = 0;
unsigned int mouseWriteCount = 0;

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

			unsigned int _event = 0;
			unsigned int data = 0;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				//Get G_KEY
				data = Keycodes[raw->data.keyboard.MakeCode][0];
				//Set state released or pressed.
				switch (raw->data.keyboard.Message) {
				case 256:
					n_Keys[data] = 1;
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
					n_Keys[data] = 0;
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
				++mouseWriteCount;

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

				//switch (raw->data.mouse.usButtonData) {
				//case 120:
				//	data = G_MOUSE_SCROLL_UP;
				//	break;
				//case 65416:
				//	data = G_MOUSE_SCROLL_DOWN;
				//	break;
				//}

				switch (raw->data.mouse.usButtonFlags) {
					//pressed
				case 1:
				case 4:
				case 16:
					n_Keys[data] = 1;
					break;
					//Released
				case 2:
				case 8:
				case 32:
					n_Keys[data] = 0;
					break;
				case 1024:
					n_Keys[data] = 1;
					break;
				}

				//update delta mouse position
				if ((mouseWriteCount - mouseReadCount) <= 1)
				{
					_mouseDeltaX = raw->data.mouse.lLastX;
					_mouseDeltaY = raw->data.mouse.lLastY;
				}
				else
				{
					_mouseDeltaX += raw->data.mouse.lLastX;
					_mouseDeltaY += raw->data.mouse.lLastY;
				}

			}

			//if (data != G_MOUSE_SCROLL_UP) {
			//	n_Keys[G_MOUSE_SCROLL_UP] = 0;
			//}
			//if (data != G_MOUSE_SCROLL_DOWN) {
			//	n_Keys[G_MOUSE_SCROLL_DOWN] = 0;
			//}

			POINT p;
			if (GetCursorPos(&p))
			{

			}
			if (ScreenToClient(window, &p))
			{
				_mousePositionX = (float)p.x;
				_mousePositionY = (float)p.y;
			}
		
			_mousePrevX = _mousePositionX;
			_mousePrevY = _mousePositionY;

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

#endif // __linux__

#ifdef __APPLE__

	GIResponder * responder = [GIResponder alloc];


#endif
}

