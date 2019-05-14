#pragma once
#include "../../Interface/G_System/GWindow.h"
#include "GUtility.h"
#include <algorithm>
#include <vector>

#ifdef __linux
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include "unistd.h"
#include <map>

#elif __APPLE__
#include <map>

#endif

using namespace GW;
using namespace CORE;
using namespace SYSTEM;

namespace
{
    // GWindow global variables.

    //! Map of Listeners to send event information to.


    std::vector<std::pair<GListener *, unsigned long long>> listeners;

#ifdef __APPLE__
    std::map<NSWindow *, GW::SYSTEM::GWindowInputEvents> eventCatchers;
#endif
    int testInt;

#ifdef _WIN32
	//Methods
	LRESULT CALLBACK GWindowProc(HWND window, unsigned int msg, WPARAM wp, LPARAM lp)
	{
		GW::SYSTEM::GWindowInputEvents* GEvents = nullptr;
		GEvents = (GW::SYSTEM::GWindowInputEvents*)GetWindowLongPtr(window, GWLP_USERDATA);
		if (GEvents == nullptr) // *BUG FIX* This will happen once if you launch a window fullscreen
			return DefWindowProcW(window, msg, wp, lp);
		// If we have a valid event handler we can process
		switch (msg)
		{
		case WM_SIZE:
		{
			GWINDOW_EVENT_DATA eventStruct;

			RECT windowRect;
			GetWindowRect(window, &windowRect);

			eventStruct.eventFlags = -1;
			eventStruct.height = (unsigned int)HIWORD(lp);
			eventStruct.width = (unsigned int)LOWORD(lp);
			eventStruct.windowHandle = window;
			eventStruct.windowX = windowRect.left;
			eventStruct.windowY = windowRect.top;

			switch (wp)
			{
			case SIZE_MAXIMIZED:
			{
				eventStruct.eventFlags = MAXIMIZE;
				*GEvents = GWindowInputEvents::MAXIMIZE;
			}
			break;

			case SIZE_MINIMIZED:
			{
				eventStruct.eventFlags = MINIMIZE;
				*GEvents = GWindowInputEvents::MINIMIZE;
			}
			break;

			case SIZE_RESTORED:
			{
				eventStruct.eventFlags = RESIZE;
				*GEvents = GWindowInputEvents::RESIZE;

			}
			break;
			}

			if (eventStruct.eventFlags != -1)
			{

				std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = listeners.begin();
				for (; iter != listeners.end(); ++iter)
					iter->first->OnEvent(GWindowUUIID, eventStruct.eventFlags, &eventStruct, sizeof(GWINDOW_EVENT_DATA));
			}

		}
		break;

		case WM_MOVE:
		{
			GWINDOW_EVENT_DATA eventStruct;

			RECT windowRect;
			GetWindowRect(window, &windowRect);

			eventStruct.eventFlags = MOVE;
			eventStruct.height = windowRect.top - windowRect.bottom;
			eventStruct.width = windowRect.right - windowRect.left;
			eventStruct.windowHandle = window;
			eventStruct.windowX = (int)LOWORD(lp);
			eventStruct.windowY = (int)HIWORD(lp);

			*GEvents = GWindowInputEvents::MOVE;

			if (eventStruct.eventFlags != -1)
			{
				std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = listeners.begin();
				for (; iter != listeners.end(); ++iter)
					iter->first->OnEvent(GWindowUUIID, eventStruct.eventFlags, &eventStruct, sizeof(GWINDOW_EVENT_DATA));
			}
		}
		break;
		case WM_CLOSE:
		{
			GWINDOW_EVENT_DATA eventStruct;

			RECT windowRect;
			GetWindowRect(window, &windowRect);

			eventStruct.eventFlags = DESTROY;
			eventStruct.height = windowRect.top - windowRect.bottom;
			eventStruct.width = windowRect.right - windowRect.left;
			eventStruct.windowHandle = window;
			eventStruct.windowX = windowRect.left;
			eventStruct.windowY = windowRect.top;

			*GEvents = GWindowInputEvents::DESTROY;

			if (eventStruct.eventFlags != -1)
			{

				std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = listeners.begin();
				for (; iter != listeners.end(); ++iter)
					iter->first->OnEvent(GWindowUUIID, eventStruct.eventFlags, &eventStruct, sizeof(GWINDOW_EVENT_DATA));
			}
		}
		break;
		case WM_DESTROY:
		{
			*GEvents = GWindowInputEvents::DESTROY;
		}
		default:
		{
            Sleep(0);
			return DefWindowProcW(window, msg, wp, lp);
			break;
		}
		//return CallWindowProcW((WNDPROC)userWinProc, window, msg, wp, lp);
		}
		return DefWindowProcW(window, msg, wp, lp);
	}
#endif // __WIN32

#ifdef __linux__
    //void LinuxWndProc(Display * _display, Window _window, GW::SYSTEM::GWindowInputEvents *Gwnd)
    void LinuxWndProc(Display * _display, Window _window, GW::SYSTEM::GWindowInputEvents &Gwnd)
    {
        Atom prop = 0;
        unsigned char * propRet = NULL;
		XEvent event;
		GWINDOW_EVENT_DATA eventStruct;
        Window rootRet = 0;
        int x = 0;     int y = 0;
        int prevX = 0; int prevY = 0;
        unsigned int width = 0;     unsigned int height = 0; unsigned int borderHeight = 0; unsigned int depth = 0;
        unsigned int prevWidth = 0; unsigned int prevHeight = 0;
        unsigned int eventFlag = 0;
        int status = 0;

        //ZeroMemory the structs
        memset(&event, 0, sizeof(event));
        memset(&eventStruct, 0, sizeof(eventStruct));

       Atom propType = XInternAtom(_display, "_NET_WM_STATE", true);
       Atom propHidden = XInternAtom(_display, "_NET_WM_STATE_HIDDEN", true);
       Atom propFull = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", true);
       Atom propClose = XInternAtom(_display, "WM_DESTROY_WINDOW", true);

        Atom actual_type = 0;
        unsigned long nitems = 0;
        unsigned long bytes_after = 0;
        int actual_format = 0;

        //bool run = true;
		while (XEventsQueued(_display, QueuedAlready) > 0)
		{
		    //std::this_thread::yield();
            //if(XEventsQueued(_display, QueuedAlready) > 0) //The fix to the hanging bug in linux
            //{

		    propRet = nullptr;

		    //Also flushes the request buffer if xlib's queue does not contain an event and waits for an event to arrive from server connection
            XNextEvent(_display, &event);

			switch (event.type)
			{
			    case Expose:
                    {
                        //Expose, when a window becomes visible on the screen, after being obscured or unmapped.
                        break;
                    }
                case PropertyNotify:
                    {
                    //PropertyNotify, when a client wants info about property changes for a specified window
                    //To receive PropertyNotify events, set the PropertyChangeMask bit in the event-mask attribute of the window.

                    status = XGetWindowProperty(event.xproperty.display, event.xproperty.window, propType, 0L, sizeof(Atom),
                                        false, AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &propRet);

                        if(status == Success && propRet && nitems > 0)
                        {
                            prop = ((Atom *)propRet)[0];
                            XGetGeometry(_display, _window, &rootRet, &x, &y, &width, &height, &borderHeight, &depth);

                            if(prop == propHidden)
                            {
                             eventFlag = MINIMIZE;
                             Gwnd = GWindowInputEvents::MINIMIZE;
                            }
                            else if(prop == 301 || prop == 302 || prop == propFull)
                            {
                            eventFlag = MAXIMIZE;
                            Gwnd = GWindowInputEvents::MAXIMIZE;
                            }
                            else if(prevX != x || prevY != y)
                            {
                            eventFlag = MOVE;
                            Gwnd = GWindowInputEvents::MOVE;
                            }
                            else if(prevHeight != height || prevWidth != width)
                            {
                            eventFlag = RESIZE;
                            Gwnd = GWindowInputEvents::RESIZE;
                            }
                            else if(prop == propClose)
                            {
                            eventFlag = DESTROY;
                            Gwnd = GWindowInputEvents::DESTROY;
                            //run = false;
                            }

                            eventStruct.eventFlags = eventFlag;
                            eventStruct.width = width;
                            eventStruct.height = height;
                            eventStruct.windowX = x;
                            eventStruct.windowY = y;
                            eventStruct.windowHandle = _display;

                            prevX = x; prevY = y; prevHeight = height; prevWidth = width;


                            if (eventStruct.eventFlags != -1 && listeners.size() > 0)
                            {
                           std::vector<std::pair<GListener *, unsigned long long>>::iterator iter = listeners.begin();
                            for (; iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GWindowUUIID, eventStruct.eventFlags, &eventStruct, sizeof(GWINDOW_EVENT_DATA));
                            }
                        }
                        XFree(propRet);
                        break;
                    }
                case ConfigureNotify:
                    {
                        //ConfigureNotify, when a client wants info about the actual changes to a window's state,
                        //such as size, position, border, and stacking order.

                        XGetGeometry(_display, _window, &rootRet, &x, &y, &width, &height, &borderHeight, &depth);

                        if(prevX != x || prevY != y) //if the previous position is not equal to the current position then we moved.
                        {
                        eventFlag = MOVE;
                        Gwnd = GWindowInputEvents::MOVE;
                        }
                        if(prevHeight != height || prevWidth != width) //if the previous width/height are not equal to the current width/height then we resized.
                        {
                        eventFlag = RESIZE;
                        Gwnd = GWindowInputEvents::RESIZE;
                        }

                            eventStruct.eventFlags = eventFlag;
                            eventStruct.width = width;
                            eventStruct.height = height;
                            eventStruct.windowX = x;
                            eventStruct.windowY = y;
                            eventStruct.windowHandle = _display;

                            prevX = x; prevY = y; prevHeight = height; prevWidth = width;


                            if (eventStruct.eventFlags != -1 && listeners.size() > 0)
                                {
                                    std::vector<std::pair<GListener *, unsigned long long>>::iterator iter = listeners.begin();
                                    for (; iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GWindowUUIID, eventStruct.eventFlags, &eventStruct, sizeof(GWINDOW_EVENT_DATA));
                                }
                        break;
                    }
                case MapNotify:
                    {
                        //MapNotify, when clients want info about which windows are mapped.
                        //The change of a window's state from unmapped to mapped

                        eventFlag = MAXIMIZE;
                        Gwnd = GWindowInputEvents::MAXIMIZE;

                            eventStruct.eventFlags = eventFlag;
                            eventStruct.width = width;
                            eventStruct.height = height;
                            eventStruct.windowX = x;
                            eventStruct.windowY = y;
                            eventStruct.windowHandle = _display;

                            prevX = x; prevY = y; prevHeight = height; prevWidth = width;

                            if (eventStruct.eventFlags != -1 && listeners.size() > 0)
                                {
                                    std::vector<std::pair<GListener *, unsigned long long>>::iterator iter = listeners.begin();
                                    for (; iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GWindowUUIID, eventStruct.eventFlags, &eventStruct, sizeof(GWINDOW_EVENT_DATA));
                                }
                    }
                case ButtonPress:
                    {
                        //Respond to button press
                        break;
                    }
                case ClientMessage:
                    {
                        /* Primarily used for transferring selection data,
                        also might be used in a private interclient
                        protocol; */

                        if(event.xclient.message_type == propHidden)
                        {
                            eventFlag = MINIMIZE;
                            Gwnd = GWindowInputEvents::MINIMIZE;

                            eventStruct.eventFlags = eventFlag;
                            eventStruct.width = width;
                            eventStruct.height = height;
                            eventStruct.windowX = x;
                            eventStruct.windowY = y;
                            eventStruct.windowHandle = _display;

                            prevX = x; prevY = y; prevHeight = height; prevWidth = width;

                            if (eventStruct.eventFlags != -1 && listeners.size() > 0)
                                {
                                    std::vector<std::pair<GListener *, unsigned long long>>::iterator iter = listeners.begin();
                                    for (; iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GWindowUUIID, eventStruct.eventFlags, &eventStruct, sizeof(GWINDOW_EVENT_DATA));
                                }
                        }

                        break;
                    }
                case SelectionClear:
                    {
                        //Clients losing ownership of a selection.
                        break;
                    }
                case SelectionNotify:
                    {
                        //A response to a ConvertSelection request when there is no owner for the selection
                        break;
                    }
                case SelectionRequest:
                    {
                        //A client requests a selection conversion by calling XConvertSelection() for the owned selection.
                        break;
                    }
                case NoExpose:
                    {
                        //Generates this event whenever a destination region could not be computed due to an obscured
                        //or out-of-bounds source region.
                        break;
                    }
                case DestroyNotify:
                    {
                    //The Destroy event gets called when a client wants info about which windows are destroyed.
                    XGetGeometry(_display, _window, &rootRet, &x, &y, &width, &height, &borderHeight, &depth);

                    eventFlag = DESTROY;
                    Gwnd = GWindowInputEvents::DESTROY;

                        eventStruct.eventFlags = eventFlag;
                        eventStruct.width = width;
                        eventStruct.height = height;
                        eventStruct.windowX = x;
                        eventStruct.windowY = y;
                        eventStruct.windowHandle = _display;

                        prevX = x; prevY = y; prevHeight = height; prevWidth = width;

                        if (eventStruct.eventFlags != -1 && listeners.size() > 0)
                            {
                                std::vector<std::pair<GListener *, unsigned long long>>::iterator iter = listeners.begin();
                                for (; iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GWindowUUIID, eventStruct.eventFlags, &eventStruct, sizeof(GWINDOW_EVENT_DATA));
                            }
                    //run = false;
                    break;
                    }

			//}

            }

        }


    }
#endif // __linux__

#ifdef __APPLE__

    GWResponder* responder = [GWResponder alloc];
    GWDelegate*  delegate = [GWDelegate alloc];
    GWAppDelegate* appDel = [GWAppDelegate alloc];
#endif
}
