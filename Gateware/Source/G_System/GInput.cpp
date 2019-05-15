// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"

#ifndef __APPLE__
#include "../../Source/G_System/GI_Callback.cpp"
#endif

#include <mutex>
#include <atomic>
#include <thread>
#include <cstring>
#include <iostream>
#include <time.h>

class Input : public SYSTEM::GInput {

private:

	/* GInterface */

	void* hWnd;

	unsigned int referenceCount;

	std::atomic_bool threadOpen;

	std::thread* inputThread;

#ifdef _WIN32

#elif __linux__
	SYSTEM::LINUX_WINDOW _linuxWindow;
#elif __APPLE__
    NSWindow * currentResponder;
#endif

public:

	/* Input */

	Input();
	virtual ~Input();

	void InputThread();

	GReturn InitializeWindows(void* _data);
	GReturn InitializeLinux(void* _data);
	GReturn InitializeMac(void* _data);

	/* GInput */

	GReturn GetState(int _keyCode, float& _outState);
	GReturn GetMouseDelta(float& _x, float& _y);
	GReturn GetMousePosition(float& _x, float& _y);
	GReturn GetKeyMask(unsigned int& _outKeyMask);
	/* GInterface */

	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
};

// This is an DLL exported version of the create function, the name is not mangled for explicit linking.
GATEWARE_EXPORT_EXPLICIT GReturn CreateGInput(void* _windowHandle, unsigned int _handleSize, GW::SYSTEM::GInput** _outPointer)
{
	unsigned int handleSize = 0;

#ifdef _WIN32
	handleSize = sizeof(HWND);
#elif __APPLE__
	handleSize = sizeof(NSWindow*);
#elif __linux__
	handleSize = sizeof(SYSTEM::LINUX_WINDOW);
#endif

	// This is NOT a recursive call, this is a call to the actual C++ name mangled version below.
	return GW::SYSTEM::CreateGInput(_windowHandle, handleSize, _outPointer);
}

GReturn GW::SYSTEM::CreateGInput(void* _windowHandle, unsigned int _handleSize, GInput** _outFpointer) {

	if (_outFpointer == nullptr || _windowHandle == nullptr) {
		return INVALID_ARGUMENT;
	}

	Input* _mInput = new Input();

	if (_mInput == nullptr) {
		return FAILURE;
	}

#ifdef _WIN32
	_mInput->InitializeWindows(_windowHandle);
#elif __APPLE__
	_mInput->InitializeMac(_windowHandle);
#elif __linux__
	_mInput->InitializeLinux(_windowHandle);
#endif


	//Todo call my intialize.
	(*_outFpointer) = _mInput;

	return SUCCESS;

}

Input::Input() {
	referenceCount = 1;
	inputThread = nullptr;
	threadOpen = false;
	hWnd = nullptr;
}

Input::~Input() {

}

GReturn Input::GetCount(unsigned int& _outCount) {

	_outCount = referenceCount;

	return SUCCESS;
}

GReturn Input::IncrementCount() {
	if (referenceCount == 0xFFFFFFFF)
		return FAILURE;

	referenceCount += 1;

	return SUCCESS;
}

GReturn Input::DecrementCount() {

	if (referenceCount == 0)
		return FAILURE;
	referenceCount -= 1;

	if (referenceCount == 0) {

#ifdef __linux__
        threadOpen = false;
        inputThread->join();
#endif
#ifdef _WIN32
		//Sets the WinProc back. (Fixes the StackOverFlow bug)
		SetWindowLongPtr((HWND)hWnd, GWLP_WNDPROC, (LONG_PTR)_userWinProc);
#endif
#ifdef __APPLE__
        [currentResponder setNextResponder:nil];
#endif
        
		delete inputThread;
		delete this;

	}

	return SUCCESS;
}

GReturn Input::RequestInterface(const GUUIID& _interfaceID, void** _outputInterface) {
	if (_outputInterface == nullptr)
		return INVALID_ARGUMENT;

	if (_interfaceID == SYSTEM::GInputUUIID)
	{
		GInput * convert = reinterpret_cast<GInput*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GSingleThreadedUUIID)
	{
		GSingleThreaded* convert = reinterpret_cast<GSingleThreaded*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GInterfaceUUIID)
	{
		GInterface* convert = reinterpret_cast<GInterface*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else
		return INTERFACE_UNSUPPORTED;

	return SUCCESS;
}

GReturn Input::InitializeWindows(void* _data) {
#ifdef _WIN32

	hWnd = _data;
	_userWinProc = SetWindowLongPtr((HWND)hWnd, GWLP_WNDPROC, (LONG_PTR)GWinProc);

	if (_userWinProc == NULL) {

	}

	//Getting Raw Input Devices. #include <time.h>.

	UINT numDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;

	//Get Number of Devices.
	if (GetRawInputDeviceList(NULL, &numDevices, sizeof(RAWINPUTDEVICELIST)) != 0) {

	}

	//Allocate the list of devices.
	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * numDevices)) == NULL) {

	}

	int nNoOfDevices = 0;
	//Using the new List and number of devices.
	//Populate the raw input device list.
	if ((nNoOfDevices = GetRawInputDeviceList(pRawInputDeviceList, &numDevices, sizeof(RAWINPUTDEVICELIST))) == ((UINT)-1)) {

	}

	RID_DEVICE_INFO rdi;
	rdi.cbSize = sizeof(RID_DEVICE_INFO);

	//For all of the devices, display their correspondent information.
	for (int i = 0; i < nNoOfDevices; i++) {

		UINT size = 256;
		TCHAR tBuffer[256] = { 0 };
		tBuffer[0] = '\0';


		//Find the device name.
		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICENAME, tBuffer, &size) < 0) {

		}

		UINT cbSize = rdi.cbSize;
		//Get the device information.
		if (GetRawInputDeviceInfo(pRawInputDeviceList[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize) < 0) {

		}
	}

	//Register the raw input devices.
	RAWINPUTDEVICE rID[2];

	//KeyBoard
	rID[0].usUsagePage = 0x01;
	rID[0].usUsage = 0x06;
	rID[0].dwFlags = RIDEV_EXINPUTSINK;
	rID[0].hwndTarget = (HWND)_data;

	//Mouse
	rID[1].usUsagePage = 0x01;
	rID[1].usUsage = 0x02;
	rID[1].dwFlags = RIDEV_EXINPUTSINK;
	rID[1].hwndTarget = (HWND)_data;

	if (RegisterRawInputDevices(rID, 2, sizeof(rID[0])) == false) {
	}



	//Capslock
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0) {
		TURNON_BIT(keyMask, G_MASK_CAPS_LOCK);
	}

	//Numlock
	if ((GetKeyState(VK_NUMLOCK) & 0x0001) != 0) {
		TURNON_BIT(keyMask, G_MASK_NUM_LOCK);
	}

	//ScrollLock
	if ((GetKeyState(VK_SCROLL) & 0x0001) != 0) {
		TURNON_BIT(keyMask, G_MASK_SCROLL_LOCK);
	}

	//Free Memory
	free(pRawInputDeviceList);

#endif

	return SUCCESS;
}

GReturn Input::InitializeLinux(void* _data) {

#ifdef __linux__
	//Copy data into a LINUX_WINDOW(void * display, void * window) structure.
	memcpy(&_linuxWindow, _data, sizeof(SYSTEM::LINUX_WINDOW));
	Display* _display;

	//Cast the void* _linuxWindow.display to a display pointer to pass to XSelectInput.
	_display = (Display*)(_linuxWindow.display);

	//Copy void* _linuxWindow.window into a Window class to pass to XSelectInput.
	//memcpy(&_window, _linuxWindow.window, sizeof(_window));
    _window = (Window)(_linuxWindow.window);

	//Select the type of Input events we wish to recieve.
	//XSelectInput(_display, _window, ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyReleaseMask | KeyPressMask | LockMask | ControlMask | ShiftMask);

#endif

	//Set our thread to open.
	threadOpen = true;
	//Create the Linux Input thread.
	inputThread = new std::thread(&Input::InputThread, this);

	return SUCCESS;

}

GReturn Input::InitializeMac(void* _data) {

#ifdef __APPLE__

	//Need to convert data back into an NSWindow*.
	//NSWindow * currentResponder = ((__bridge NSWindow*)_data);
    currentResponder = ((__bridge NSWindow*)_data);
    
	//We only want to process the message and pass it on. So if there is already
	//a responder we set our responders next responder to be the current next responder.
	[responder setNextResponder : currentResponder.nextResponder];

	//We then set out responder to the next responder of the window.
	[currentResponder setNextResponder : responder];

	//We also need to make our responder the first responder of the window.
	[currentResponder makeFirstResponder : responder];

	//In order to get mouse button presses we need to set our responder to be
	//the next responder in the contentView as well.
	[currentResponder.contentView setNextResponder : responder];


#endif

	return SUCCESS;
}

GReturn Input::GetState(int _keyCode, float& _outState) {
	if (_keyCode == G_MOUSE_SCROLL_DOWN || _keyCode == G_MOUSE_SCROLL_UP) {
		return FEATURE_UNSUPPORTED;
	}
	_outState = (float)n_Keys[_keyCode];
	return SUCCESS;
}

GReturn Input::GetMouseDelta(float& _x, float& _y) {

	_x = _mouseDeltaX;
	_y = _mouseDeltaY;
	if (mouseReadCount != mouseWriteCount)
	{
		mouseReadCount = mouseWriteCount;
		return SUCCESS;
	}
	return REDUNDANT_OPERATION;

}

GReturn Input::GetMousePosition(float& _x, float& _y) {

	_x = _mousePositionX;
	_y = _mousePositionY;

	return SUCCESS;
}

GReturn Input::GetKeyMask(unsigned int& _outKeyMask) {
	_outKeyMask = keyMask;
	return SUCCESS;
}

void Input::InputThread()
{

#ifdef __linux__
	int _code = -1;

	while (threadOpen)
	{

		//Cast the void* _linuxWindow. display to a display pointer to pass to XNextEvent.
		Display * _display = (Display*)(_linuxWindow.display);
		char keys_return[32];
		XQueryKeymap(_display, keys_return);

		for (unsigned int i = 0; i < 127; i++) {
			_code = Keycodes[i][1];
			if (keys_return[(i >> 3)] & (1 << (i & 7))) {
				n_Keys[_code] = 1;
			}
			else {
				n_Keys[_code] = 0;
			}
		}


		Window a, b;
		XQueryPointer(_display, _window, &a, &b, &_mouseScreenPositionX, &_mouseScreenPositionY, &_mousePositionX, &_mousePositionY, &keyMask);
		//printf("KeyMask: %d\n", keyMask);

		if (keyMask & Button1Mask) {
			//printf("Left\n");
			n_Keys[G_BUTTON_LEFT];

		}
		if (keyMask & Button3Mask) {
			//printf("Right\n");
			n_Keys[G_BUTTON_RIGHT];
		}

		if (keyMask & Button2Mask) {
			//printf("Middle\n");
			n_Keys[G_BUTTON_MIDDLE];
		}

		timespec delay = { 0, 3333333 }; // 300Hz
		//while(nanosleep(&delay, &delay)); // Throttle thread to 300Hz.

		//Set the change in mouse position.
		_mouseDeltaX = _mousePrevX - _mousePositionX;
		_mouseDeltaY = _mousePrevY - _mousePositionY;

		//Set the previous mouse position as the current.
		_mousePrevX = _mousePositionX;
		_mousePrevY = _mousePositionY;


	}
#endif

}


