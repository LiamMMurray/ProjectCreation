#ifndef GWINDOW_H
#define GWINDOW_H

/*!
	File: GWindow.h
	Purpose: A Gateware interface that creates a window and handles its properties.
	Author: Nic Russell
	Contributors: ShuoYi Chang, Andre Reid
	Last Modified: 8/21/2017
	Interface Status: Early Stage Development
	Copyright: 7thGate Software LLC.
	License: MIT
*/

//! GWindow inherits directly from GBroadcasting.
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

		// !GWINDOW_EVENT_DATA holds the properties of the window the event was sent from.
		struct GWINDOW_EVENT_DATA
		{
			unsigned int eventFlags;
			unsigned int height;
			unsigned int width;
			int windowX;
			int windowY;
			void* windowHandle;
		};

#pragma pack(pop)

		//! GWindowStyle holds the possible types of window GWindow can create.
		enum GWindowStyle
		{
			WINDOWEDBORDERED,
			WINDOWEDBORDERLESS,
			FULLSCREENBORDERED,
			FULLSCREENBORDERLESS,
			MINIMIZED,
		};

		//! GWindowInputEvents holds the possible events a GWindow window can broadcast.
		enum GWindowInputEvents
		{
			NOTIFY,
			MINIMIZE,
			MAXIMIZE,
			RESIZE,
			MOVE,
			DESTROY,
		};

		//! Unique Identifier for this interface. {E53C2594-E47C-49A5-BD1B-5341F2603F95}
		static const GUUIID GWindowUUIID =
		{
			0xe53c2594, 0xe47c, 0x49a5,{ 0xbd, 0x1b, 0x53, 0x41, 0xf2, 0x60, 0x3f, 0x95 }
		};

		//! A thread-safe window creation and management library.
		/*!
		*	This library is used to create, move, resize, and destroy
		*	a window. Methods exist to query information from the 
		*	window as well. The window is also a broadcaster, meaning
		*	a GListener can be written to receive events from it.
		*/
		class GWindow : public CORE::GBroadcasting 
		{

		public:
			
			//! Initializes a window handle and displays a window.
			/*!
			*	The window is opened with the size, position and style specified in the parameters
			*	passed into the CreateGWindow function. Parameters were checked for invalid values
			*	during the initialization of the window after creation, so it is assumed the window
			*	has valid parameters before this function is called.
			*
			*	\retval SUCCESS The window was successfully created and displayed.
			*	\retval REDUNDANT_OPERATION The GWindow object already has a window open
			*	\retval FAILURE The window could not be created.
			*/
			virtual GReturn OpenWindow() = 0;

			//! Flushes and processes all messages from the window's event queue.
			/*!
			*	This function is meant to be called once a frame in an application's
			*	main loop. This function will break when all waiting messages have
			*   been processed and the event queue is empty.
			*
			*	\retval SUCCESS The messages were successfully processed and removed.
			*   \retval REDUNDANT_OPERATION No window exists process.
			*/
			virtual GReturn ProcessWindowEvents() = 0;

			//! Gives the currently opened window the specified size, position and style.
			/*!
			*	If width and height are equal to or greater than the native resolution, the passed in
			*	GWindowStyle will be overwritten to be the fullscreen version if it is not already.
			*	If position parameters are less than 0 then 0 will be used. If position parameters 
			*	are greater than native resoultion, maximum native resolution parameters will be used.
			*
			*	\param [in] _x The x position on screen to move the window to.
			*	\param [in] _y The y position on screen to move the window to.
			*	\param [in] _width The width to give the window.
			*	\param [in] _height The height to give the window.
			*	\param [in] _style The style to give to the window. (see GWindowStyle for style options)
			*
			*	\retval SUCCESS The window successfully had its attributes changed.
			*	\retval INVALID_ARGUMENT One of the size parameters are outside the limits of the hardware.
			*	\retval REDUNDANT_OPERATION No window exists to edit. 
			*/
			virtual GReturn ReconfigureWindow(int _x, int _y, int _width, int _height, GWindowStyle _style) = 0;

			//! Repositions the currently opened window to the specified x and y pixels on screen.
			/*!
			*	If position parameters are less than 0 then 0 will be used. If position parameters 
			*	are greater than native resoultion, maximum native resolution parameters will be used.
			*   
			*	\param [in] _x The x position on screen to move the window to.
			*	\param [in] _y The y position on screen to move the window to.
			*
			*	\retval SUCCESS The window was successfully moved.
			*	\retval INVALID_ARGUMENT The style passed in is invalid
			*   \retval REDUNDANT_OPERATION No window exists to move.	
			*/	
			virtual GReturn MoveWindow(int _x, int _y) = 0;

			//! Resizes the currently opened window to the specified width and height.
			/*!
			*	If width and height are greater than the native resolution, the GWindowStyle 
			*	will be overwritten to be the fullscreen version if it is not already.
			*   If position parameters are less than 0 then 0 will be used. If position parameters 
			*	are greater than native resoultion, maximum native resolution parameters will be used.
			*
			*	\param [in] _x The width to resize the window to.
			*	\param [in] _y The height to resize the window to.
			*
			*	\retval SUCCESS The window was successfully resized.
			*	\retval INVALID_ARGUMENT One of the size parameters are less than or equal to 0.
			*   \retval REDUNDANT_OPERATION No window exists to resize.
			*/
			virtual GReturn ResizeWindow(int _width, int _height) = 0;

			//! Resizes the currently opened window to the native maximum resolution.
			/*!
			*	GWindowStyle will be overwritten to be the fullscreen version 
			*	if it is not already.
			*
			*	\retval SUCCESS The window was successfully maximized.
			*   \retval REDUNDANT_OPERATION No window exists to maximize.
			*/
			virtual GReturn Maximize() = 0;

			//! Minimizes the currently opened window.
			/*!
			*	GWindowStyle will be overwritten to be the minimized
			*	style if it is not already.
			*
			*	\retval SUCCESS The window was successfully minimized.
			*   \retval REDUNDANT_OPERATION No window exists to minimize 
			*								or window is already maximized.
			*/
			virtual GReturn Minimize() = 0;

			//! Sets the currently opened window's style to the specified style. 
			/*!
			*	GWindowStyle will be overwritten and the window resized or moved
			*	accordingly.
			*
			*	\param [in] _style The GWindowStyle to change the window to.
			*
			*	\retval SUCCESS The window style was successfully changed.
			*   \retval REDUNDANT_OPERATION No window exists to change.
			*/
			virtual GReturn ChangeWindowStyle(GWindowStyle _style) = 0;

			//! Returns the width in pixels of the currently opened window.
			/*!
			*	\retval 0 The window is minimized.
			*   \retval -1 No window exists to query size from.
			*	\retval else Width was successfully queried and returned.
			*/
			virtual GReturn	GetWidth(unsigned int& _outWidth) = 0;

			//! Returns the height in pixels of the currently opened window.
			/*!
			*	Client width is the width of the window's drawable area.
			*
			*	\retval 0 The window is minimized.
			*   \retval -1 No window exists to query size from.
			*	\retval else Height was successfully queried and returned.
			*/
			virtual GReturn	GetHeight(unsigned int& _outHeight) = 0;

			//! Returns the client width in pixels of the currently opened window.
			/*!
			*	Client height is the height of the window's drawable area.
			*
			*	\retval 0 The window is minimized.
			*   \retval -1 No window exists to query size from.
			*	\retval else Width was successfully queried and returned.
			*/
			virtual GReturn	GetClientWidth(unsigned int& _outClientWidth) = 0;

			//! Returns the client height in pixels of the currently opened window.
			/*!
			*	\retval 0 The window is minimized.
			*   \retval -1 No window exists to query size from.
			*	\retval else Height was successfully queried and returned.
			*/
			virtual GReturn	GetClientHeight(unsigned int& _outClientHeight) = 0;

			//! Returns the X position in pixels of the currently opened window.
			/*!
			*   \retval -1 No window exists to query position from.
			*	\retval else X position was successfully queried and returned.
			*/
			virtual GReturn	GetX(unsigned int& _outX) = 0;

			//! Returns the Y position in pixels of the currently opened window.
			/*!
			*   \retval -1 No window exists to query position from.
			*	\retval else Y position was successfully queried and returned.
			*/
			virtual GReturn	GetY(unsigned int& _outY) = 0;

			//! Gets the location of the top-left pixel of the opened window's client area.
			/*!
			*	\param [out] _outX Will contain the X location of the top-left pixel.
			*	\param [out] _outY Will contain the Y location of the top-left pixel.
			*
			*   \retval -1 No window exists to query position from.
			*	\retval else Position was successfully queried and returned.
			*/
			virtual GReturn GetClientTopLeft(unsigned int &_outX, unsigned int &_outY) = 0;

			//! Returns the platform specific window handle to the currently opened window.
			/*!
			*	On Windows the void* is an HWND, on Linux a LINUX_WINDOW, and on Mac an 
			*	NSWindow. Methods exist to query window information right from these handles.
			*
			*   \retval void* The void* data to the window handle.
			*/
			virtual GReturn GetWindowHandle(unsigned int _handleSize, void** _outWindowHandle) = 0;

			//! Returns a bool specifying whether or not the currently opened window is fullscreen.
			/*!
			*   \retval true The window is fullscreen.
			*	\retval false The window is not fullscreen.
			*/
			virtual GReturn IsFullscreen(bool& _outIsFullscreen) = 0;

			//! Returns the last input event that has happened to the window.
			/*!
			*	\param [out] GWindowInputEvent you pass-in will be updated to the lastest event that was called. 
			*	
			*	\retval FAILURE The Gateware's internal event tracker wasn't a valid event.
			*	\retval SUCCESS The function run successfully and out-param is filled out.
			*
			*/
			virtual GReturn GetLastEvent(GWindowInputEvents& _LastEvent) = 0;

		}; // end GWindow class

		   //! Creates and outputs a new GWindow object.
		   /*!
		   *	Initializes a handle to a GWindow object with the parameters specified. Created
		   *	GWindow object will have its reference count initialized to one.
		   *
		   *	\param [in] _x The x position of the window on the screen.
		   *	\param [in] _y The y position of the window on the screen.
		   *	\param [in] _width The width of the window.
		   *	\param [in] _height The height of the window.
		   *	\param [in] _style The style of the window. (see GWindowStyle for style options)
		   *	\param [out] _outWindow Will contain the GWindow object if successfully created. 
		   *
		   *	\retval SUCCESS  GWindow was successfully created.
		   *	\retval FAILURE  GWindow was not created. _outWindow will be null.
		   *	\retval INVALID_ARGUMENT Either one or some arguments are nullptrs or invalid integers.
		   */
		GATEWARE_EXPORT_IMPLICIT GReturn CreateGWindow(int _x, int _y, int _width, int _height, GWindowStyle _style, GWindow** _outWindow);
	} // end SYSTEM namespace
} // end GW namespace

#endif
