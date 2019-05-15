#include "../DLL_Export_Symbols.h"
#include "../../Interface/G_Graphics/GOpenGLSurface.h"
#include "../../Source/G_System/GUtility.h"
#include "../../Interface/G_System/GKeyDefines.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <atomic>

#ifdef _WIN32

#include <Windows.h>
#pragma comment(lib, "OpenGL32.lib")

#include <gl\GL.h>
#include "gl3.h"
#include "wgl.h"

#include <atomic>
#include <mutex>
#include <thread>

#elif __linux__

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "glxext.h"
#include "unistd.h"

#elif __APPLE__
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <mach-o/dyld.h>
#import <dlfcn.h>
#endif

using namespace GW;
using namespace CORE;
using namespace SYSTEM;
using namespace GRAPHICS;

class GOpenGL : public GOpenGLSurface
{
private:
	///////////////////////////////////////////////////////
	// declare all necessary members (platform specific) //
	///////////////////////////////////////////////////////
    std::atomic<unsigned int>	refCount;
	GWindow*		gWnd;
	unsigned int	clientX;
	unsigned int	clientY;
	unsigned int	width;
	unsigned int	height;
	float			aspectRatio;

	GLint			numExtensions = 0;
	const char*		glExtensions;

#ifdef _WIN32

    //std::atomic<unsigned int>	refCount = 1;
	HWND                        surfaceWindow;
    HDC                         hdc;
	HGLRC                       OGLcontext;

	/////////////////////////////////////////////////////////
	//				 WGL FUNCTION POINTERS				   //
	/////////////////////////////////////////////////////////
	PFNWGLGETEXTENSIONSSTRINGARBPROC	wglGetExtensionsStringARB;
	PFNWGLGETEXTENSIONSSTRINGEXTPROC	wglGetExtensionsStringEXT;
	PFNWGLCREATECONTEXTATTRIBSARBPROC	wglCreateContextAttribsARB;
	PFNWGLSWAPINTERVALEXTPROC			wglSwapIntervalEXT;
	PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;


#elif __linux__

#define GLX_CONTEXT_MAJOR_VERSION_ARB   0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB   0x2092

    ///////////////////////////
    // GLX FUNCTION POINTERS //
    ///////////////////////////
    PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB;
	PFNGLXSWAPINTERVALEXTPROC		  glXSwapIntervalEXT;

    //std::atomic<unsigned int>	refCount = 1;
    Window                      root;
    Window*                     lWindow;
    GLint                       attributes[5] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    GLXContext                  OGLXcontext;
    LINUX_WINDOW                lWnd;

#elif __APPLE__
    //std::atomic<unsigned int>	refCount;
    NSOpenGLContext*            OGLMcontext;
    NSWindow*                   nsWnd;
    NSView*                     view;

#endif

public:
	GOpenGL();
	virtual ~GOpenGL();
	GReturn Initialize(unsigned long long _initMask);
	GReturn	GetContext(void** _outContext);
	GReturn	GetAspectRatio(float& _outAspectRatio);
	GReturn	UniversalSwapBuffers();
	GReturn	QueryExtensionFunction(const char* _extension, const char* _funcName, void** _outFuncAddress);
	GReturn EnableSwapControl(bool _setSwapControl);

	void	SetGWindow(GWindow* _window);

	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
	GReturn OnEvent(const GUUIID& _senderInterface, unsigned int _eventID, void* _eventData, unsigned int _dataSize);
};

GOpenGL::GOpenGL() : refCount(1)
{
#ifdef _WIN32

	ZeroMemory(&surfaceWindow, sizeof(HWND));

#elif __linux__
#elif __APPLE__

    nsWnd = [NSWindow alloc];

#endif
}

GOpenGL::~GOpenGL()
{
	#ifdef _WIN32
	#elif __linux__
//XLockDisplay((Display*)lWnd.display);
	glXMakeCurrent((Display*)lWnd.display, None, NULL);
	glXDestroyContext((Display*)lWnd.display, OGLXcontext);
	XDestroyWindow((Display*)lWnd.display, *(Window*)lWnd.window);
//XUnlockDisplay((Display*)lWnd.display);
	XCloseDisplay((Display*)lWnd.display);

	#elif __APPLE__
	#endif

	if (gWnd) gWnd->DecrementCount();

}

void GOpenGL::SetGWindow(GWindow* _window)
{
	gWnd = _window;
	_window->IncrementCount(); // always increase the refrence count if you hold on to a pointer! Don't forget to Decrement when done!
}

GReturn GOpenGL::Initialize(unsigned long long _initMask)
{
	//Check if valid _initMask was passed in
	unsigned long long allowed = ~(COLOR_10_BIT | DEPTH_BUFFER_SUPPORT | DEPTH_STENCIL_SUPPORT | OPENGL_ES_SUPPORT);
	if (allowed & _initMask)
	{
		return FEATURE_UNSUPPORTED;
	}

	if (gWnd == nullptr)
		return FAILURE;

    gWnd->OpenWindow();

#ifdef _WIN32

	gWnd->GetWindowHandle(sizeof(HWND), (void**)&surfaceWindow);
	gWnd->GetClientWidth(width);
	gWnd->GetClientHeight(height);
	hdc = GetDC(surfaceWindow);

	aspectRatio = (float)width / (float)height;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		8, 0,
		8, 0,
		8, 0,
		8, 0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	bool ret = SetPixelFormat(hdc, pixelFormat, &pfd);

	OGLcontext = wglCreateContext(hdc);
	wglMakeCurrent(hdc, OGLcontext);

	wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
	wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");


	int pixelAttributes[] =
	{
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_COLOR_BITS_ARB, 32,
		WGL_RED_BITS_ARB, 8,
		WGL_GREEN_BITS_ARB, 8,
		WGL_BLUE_BITS_ARB, 8,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 0,
		WGL_STENCIL_BITS_ARB, 0,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		0, 0
	};

	if (_initMask & COLOR_10_BIT)
	{
		pixelAttributes[7] = 10;
		pixelAttributes[9] = 10;
		pixelAttributes[11] = 10;
		pixelAttributes[13] = 2;
	}

	if (_initMask & DEPTH_BUFFER_SUPPORT)
		pixelAttributes[15] = 32;

	if (_initMask & DEPTH_STENCIL_SUPPORT)
	{
		pixelAttributes[15] = 24;
		pixelAttributes[17] = 8;
	}

	UINT pixelCount = 0;

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
	QueryExtensionFunction("WGL_ARB_extensions_string", "wglChoosePixelFormatARB", (void**)&wglChoosePixelFormatARB);

	ret = wglChoosePixelFormatARB(hdc, pixelAttributes, NULL, 1, &pixelFormat, &pixelCount);
	ret = SetPixelFormat(hdc, pixelFormat, &pfd);

	wglMakeCurrent(NULL, NULL);
	ReleaseDC(surfaceWindow, hdc);
	wglDeleteContext(OGLcontext);

	//////////////////////////////////
	// Create an OpenGL 3.0 Context //
	//////////////////////////////////
	int contextAttributes[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	if (_initMask & OPENGL_ES_SUPPORT)
	{
		/////////////////////////////////////
		// Create an OpenGL ES 3.0 Context //
		/////////////////////////////////////
		contextAttributes[5] = WGL_CONTEXT_ES2_PROFILE_BIT_EXT;
	}

	OGLcontext = wglCreateContextAttribsARB(hdc, 0, contextAttributes);
	wglMakeCurrent(hdc, OGLcontext);

	if (_initMask & DEPTH_BUFFER_SUPPORT)
		glEnable(GL_DEPTH_TEST);

	if (_initMask & DEPTH_STENCIL_SUPPORT)
		glEnable(GL_STENCIL_TEST);

	int pfValues[6] = { 0 };
	int pfQuery[] =
	{
		WGL_RED_BITS_ARB,
		WGL_GREEN_BITS_ARB,
		WGL_BLUE_BITS_ARB,
		WGL_ALPHA_BITS_ARB,
		WGL_DEPTH_BITS_ARB,
		WGL_STENCIL_BITS_ARB
	};

	ret = wglGetPixelFormatAttribivARB(hdc, pixelFormat, PFD_MAIN_PLANE, 6, pfQuery, pfValues);

	//////////////////////////////////
	// CHECK IF INIT FLAGS WERE MET //
	//////////////////////////////////

	//////////////////
	// 10 BIT COLOR //
	//////////////////
	if (_initMask & COLOR_10_BIT)
	{
		if (pfValues[0] != 10 && pfValues[1] != 10 && pfValues[2] != 10)
			return FEATURE_UNSUPPORTED;

	}

	//////////////////////////
	// DEPTH BUFFER SUPPORT //
	//////////////////////////
	if (_initMask & DEPTH_BUFFER_SUPPORT)
	{
		if (pfValues[4] == 0 || !glIsEnabled(GL_DEPTH_TEST))
			return FEATURE_UNSUPPORTED;
	}

	///////////////////////////
	// DEPTH STENCIL SUPPORT //
	///////////////////////////
	if (_initMask & DEPTH_STENCIL_SUPPORT)
	{
		if (pfValues[5] == 0 || !glIsEnabled(GL_STENCIL_TEST))
			return FEATURE_UNSUPPORTED;
	}

	////////////////////////
	// ES CONTEXT SUPPORT //
	////////////////////////
	if (_initMask & OPENGL_ES_SUPPORT)
	{
		char* version = (char*)glGetString(GL_VERSION);

		if (strstr(version, "OpenGL ES") == NULL)
			return FEATURE_UNSUPPORTED;
	}

	//////////////////////
	// DIRECT2D SUPPORT //
	//////////////////////
	if (_initMask & DIRECT2D_SUPPORT)
	{
		return FEATURE_UNSUPPORTED;
	}

#elif __linux__
gWnd->GetWindowHandle(sizeof(LINUX_WINDOW), (void**)&lWnd);
lWindow = (Window*)lWnd.window;

    unsigned int cX, cY, cWidth, cHeight, wX, wY, wWidth, wHeight;
    gWnd->GetClientTopLeft(cX, cY);
    gWnd->GetClientWidth(cWidth);
    gWnd->GetClientHeight(cHeight);
    gWnd->GetX(wX);
    gWnd->GetY(wY);
    gWnd->GetWidth(wWidth);
    gWnd->GetHeight(wHeight);

    static int FBattribs[] =
{
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_DOUBLEBUFFER, true,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 0,
    GLX_STENCIL_SIZE, 0,
    None
};

if (_initMask & COLOR_10_BIT)
{
    FBattribs[7] = 10;
    FBattribs[9] = 10;
    FBattribs[11] = 10;
    FBattribs[13] = 2;
}

if (_initMask & DEPTH_BUFFER_SUPPORT)
    FBattribs[15] = 24;

if (_initMask & DEPTH_STENCIL_SUPPORT)
{
	FBattribs[15] = 24;
    FBattribs[17] = 8;
}

//////////////////////////////////////////////////
// Select the Default Framebuffer Configuration //
//////////////////////////////////////////////////
XLockDisplay((Display*)lWnd.display);
int fbCount;
GLXFBConfig* fbc = glXChooseFBConfig((Display*)lWnd.display, DefaultScreen((Display*)lWnd.display), FBattribs, &fbCount);
XVisualInfo* vi = glXGetVisualFromFBConfig((Display*)lWnd.display, fbc[0]);
Colormap cMap = XCreateColormap((Display*)lWnd.display, RootWindow((Display*)lWnd.display, vi->screen), vi->visual, AllocNone);

XSetWindowAttributes swa;
swa.background_pixel = XWhitePixel((Display*)lWnd.display, 0);
swa.border_pixel = XBlackPixel((Display*)lWnd.display, 0);
swa.event_mask = SubstructureNotifyMask | PropertyChangeMask | ExposureMask;

unsigned long valueMask = 0;
valueMask |= CWBackPixel;
valueMask |= CWEventMask;

XChangeWindowAttributes((Display*)lWnd.display, *lWindow, valueMask, &swa);
GLXContext oldContext = glXCreateContext((Display*)lWnd.display, vi, 0, GL_TRUE);

/////////////////////////
// Load GLX Extensions //
/////////////////////////

glExtensions = glXQueryExtensionsString((Display*)lWnd.display, DefaultScreen((Display*)lWnd.display));

glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalEXT");

if(!glXMakeCurrent((Display*)lWnd.display, 0, 0))
    return FAILURE;
glXDestroyContext((Display*)lWnd.display, oldContext);

////////////////////////
// Create New Context //
////////////////////////

    int contextAttribs[] =
    {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 0,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    if (_initMask & OPENGL_ES_SUPPORT)
    {
        contextAttribs[5] = GLX_CONTEXT_ES2_PROFILE_BIT_EXT;
    }

    OGLXcontext = glXCreateContextAttribsARB((Display*)lWnd.display, fbc[0], NULL, true, contextAttribs);
    if(!glXMakeCurrent((Display*)lWnd.display, *lWindow, OGLXcontext))
        return FAILURE;
    XUnlockDisplay((Display*)lWnd.display);

	if (_initMask & DEPTH_BUFFER_SUPPORT)
		glEnable(GL_DEPTH_TEST);

	if (_initMask & DEPTH_STENCIL_SUPPORT)
		glEnable(GL_STENCIL_TEST);

	//////////////////////////////////
	// CHECK IF INIT FLAGS WERE MET //
	//////////////////////////////////
	//////////////////
	// 10 BIT COLOR //
	//////////////////
	if (_initMask & COLOR_10_BIT)
	{
		GLint red, green, blue;
		glGetIntegerv(GL_RED_BITS, &red);
		glGetIntegerv(GL_GREEN_BITS, &green);
		glGetIntegerv(GL_BLUE_BITS, &blue);

		if (red != 10 &&
			green != 10 &&
			blue != 10)
			return FAILURE;

	}
	//////////////////////////
	// DEPTH BUFFER SUPPORT //
	//////////////////////////
	if (_initMask & DEPTH_BUFFER_SUPPORT)
	{
		GLint depth;
		glGetIntegerv(GL_DEPTH_BITS, &depth);

		if (depth == 0 || !glIsEnabled(GL_DEPTH_TEST))
			return FAILURE;
	}
	///////////////////////////
	// DEPTH STENCIL SUPPORT //
	///////////////////////////
	if (_initMask & DEPTH_STENCIL_SUPPORT)
	{
		GLint stencil;
		glGetIntegerv(GL_STENCIL_BITS, &stencil);

		if (stencil == 0 || !glIsEnabled(GL_STENCIL_TEST))
			return FAILURE;
	}
	////////////////////////
	// ES CONTEXT SUPPORT //
	////////////////////////
	if (_initMask & OPENGL_ES_SUPPORT)
	{
		char* version = (char*)glGetString(GL_VERSION);

		if (strstr(version, "OpenGL ES") == NULL)
			return FAILURE;
	}
	//Clean Memory
	XFree(vi);
	XFree(fbc);


#elif __APPLE__

    gWnd->GetWindowHandle(sizeof(NSWindow*), (void**)&nsWnd);

    unsigned int viewWidth;
    unsigned int viewHeight;
    unsigned int viewX;
    unsigned int viewY;

    gWnd->GetClientWidth(viewWidth);
    gWnd->GetClientHeight(viewHeight);
    gWnd->GetX(viewX);
    gWnd->GetY(viewY);

    view = [NSView alloc];
    [view initWithFrame:NSMakeRect(viewX, viewY, viewWidth, viewHeight)];

    [nsWnd setContentView:view];

    NSOpenGLPixelFormatAttribute pixelAttributes[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 32,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADepthSize, 0,
        NSOpenGLPFAStencilSize, 0,
        0,
    };

    //////////////////////////////////
    // CHECK IF INIT FLAGS WERE MET //
    //////////////////////////////////


	//////////////////
	// 10 BIT COLOR //
	//////////////////
	if (_initMask & COLOR_10_BIT)
        return FEATURE_UNSUPPORTED;


	//////////////////////////
	// DEPTH BUFFER SUPPORT //
	//////////////////////////
	if (_initMask & DEPTH_BUFFER_SUPPORT)
        pixelAttributes[8] = 32;


	///////////////////////////
	// DEPTH STENCIL SUPPORT //
	///////////////////////////
    if (_initMask & DEPTH_STENCIL_SUPPORT)
    {
        pixelAttributes[8] = 24;
        pixelAttributes[10] = 8;
    }

    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelAttributes];

    OGLMcontext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    [OGLMcontext makeCurrentContext];
    [OGLMcontext setView:view];

    GLint depth, stencil;

    [pixelFormat getValues:&depth forAttribute:NSOpenGLPFADepthSize forVirtualScreen:0];
    [pixelFormat getValues:&stencil forAttribute:NSOpenGLPFAStencilSize forVirtualScreen:0];

    if (_initMask & DEPTH_BUFFER_SUPPORT)
    {
        glEnable(GL_DEPTH_TEST);

        if (!glIsEnabled(GL_DEPTH_TEST) || depth == 0)
            return FAILURE;
    }

    if (_initMask & DEPTH_STENCIL_SUPPORT)
    {
        glEnable(GL_STENCIL_TEST);

        if (!glIsEnabled(GL_STENCIL_TEST) || stencil == 0)
            return FAILURE;
    }


#endif

	return SUCCESS;
}

GReturn GOpenGL::GetContext(void ** _outContext)
{
#ifdef _WIN32

	if (!OGLcontext)
		return FAILURE;

	*_outContext = OGLcontext;

#elif __linux__

	if (!OGLXcontext)
		return FAILURE;

    *_outContext = OGLXcontext;

#elif __APPLE__

	if (!OGLMcontext)
		return FAILURE;

    *_outContext = OGLMcontext;

#endif

	return SUCCESS;
}

GReturn GOpenGL::GetAspectRatio(float& _outAspectRatio)
{
	if (!gWnd)
		return FAILURE;

	_outAspectRatio = aspectRatio;

	return SUCCESS;
}

GReturn GOpenGL::UniversalSwapBuffers()
{
#ifdef _WIN32

	if (!hdc)
		return FAILURE;

	SwapBuffers(hdc);

#elif __linux__
	if (!lWnd.display || !lWnd.window || lWindow == nullptr)
		return FAILURE;
XLockDisplay((Display*)lWnd.display);
	glXSwapBuffers((Display*)lWnd.display, *lWindow);
XUnlockDisplay((Display*)lWnd.display);
#elif __APPLE__

	if (!OGLMcontext)
		return FAILURE;

    [OGLMcontext flushBuffer];

#endif

	return SUCCESS;

}

GReturn GOpenGL::QueryExtensionFunction(const char* _extension, const char* _funcName, void** _outFuncAddress)
{
	///////////////////////
	// Invalid Arguments //
	///////////////////////
	if ((_funcName == nullptr && _outFuncAddress != nullptr) ||
		(_funcName != nullptr && _outFuncAddress == nullptr) ||
		 _extension == nullptr && _funcName == nullptr)
		return INVALID_ARGUMENT;

#ifdef __APPLE__

    static void* funcImage = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);

    if (funcImage)
    {
        _outFuncAddress = (void**)dlsym(funcImage, _funcName);
        return SUCCESS;
    }
    else
        return FAILURE;

#endif

	//////////////////////////////////////////////////////////
	// User only passed in function name, without extension //
	//////////////////////////////////////////////////////////

	if (_extension == nullptr && _funcName != nullptr && _outFuncAddress != nullptr)
	{

#ifdef _WIN32

		*_outFuncAddress = wglGetProcAddress(_funcName);

		if (*_outFuncAddress == nullptr)
			return FAILURE;

		return SUCCESS;

#elif __linux__
		_outFuncAddress = (void**)glXGetProcAddress((const GLubyte*)_funcName);

		if (_outFuncAddress == nullptr)
			return FAILURE;

		return SUCCESS;

#endif

	}

    //////////////////////////////////////////////////////////
	// User only passed in extension name, without function //
    //////////////////////////////////////////////////////////

	if (_funcName == nullptr && _outFuncAddress == nullptr)
	{
#ifdef _WIN32

		if (wglGetExtensionsStringEXT)
		{
			glExtensions = wglGetExtensionsStringEXT();

			if (strstr(glExtensions, _extension) != NULL)
				return SUCCESS;
		}

		if (wglGetExtensionsStringARB)
		{
			glExtensions = wglGetExtensionsStringARB(hdc);

			if (strstr(glExtensions, _extension) != NULL)
				return SUCCESS;
		}

		return FAILURE;

#elif __linux__
        if (strstr(glExtensions, _extension) != NULL)
            return SUCCESS;

        return FAILURE;

#endif
	}

    /////////////////////////////////////////////////////
	// User passed in extension name and function name //
    /////////////////////////////////////////////////////

#ifdef _WIN32

	if (wglGetExtensionsStringEXT)
	{
		glExtensions = wglGetExtensionsStringEXT();

		if (strstr(glExtensions, _extension) != NULL)
		{
			if (_funcName != NULL)
				*_outFuncAddress = wglGetProcAddress(_funcName);
			else
				*_outFuncAddress = wglGetProcAddress(_extension);

			return SUCCESS;
		}
	}

	if (wglGetExtensionsStringARB)
	{
		glExtensions = wglGetExtensionsStringARB(hdc);

		if (strstr(glExtensions, _extension) != NULL)
		{
			if (_funcName != NULL)
				_outFuncAddress = (void**)wglGetProcAddress(_funcName);
			else
				_outFuncAddress = (void**)wglGetProcAddress(_extension);

			return SUCCESS;
		}

	}
	return FAILURE;

#elif __linux__
    if (strstr(glExtensions, _extension) != NULL)
    {

        if (_funcName != NULL)
            _outFuncAddress = (void**)glXGetProcAddress((const GLubyte*)_funcName);
        else
            _outFuncAddress = (void**)glXGetProcAddress((const GLubyte*)_extension);

        return SUCCESS;
    }

    return FAILURE;

#endif

}

GReturn GOpenGL::EnableSwapControl(bool _setSwapControl)
{

#if _WIN32

    if (!wglSwapIntervalEXT)
        return FEATURE_UNSUPPORTED;

	if (!OGLcontext)
		return FAILURE;

    if (_setSwapControl == true)
        wglSwapIntervalEXT(1);
    else
        wglSwapIntervalEXT(0);

    return SUCCESS;

#elif __linux__

	if (!glXSwapIntervalEXT)
		return FEATURE_UNSUPPORTED;

	if (!OGLXcontext)
		return FAILURE;

XLockDisplay((Display*)lWnd.display);
	if (_setSwapControl == true)
		glXSwapIntervalEXT((Display*)lWnd.display, *lWindow, 1);
	else
		glXSwapIntervalEXT((Display*)lWnd.display, *lWindow, 0);
XUnlockDisplay((Display*)lWnd.display);
	return SUCCESS;

#elif __APPLE__

	if (!OGLMcontext)
		return FAILURE;

	GLint swapInt;
	if (_setSwapControl)
		swapInt = 1;
	else
		swapInt = 0;

	[OGLMcontext setValues : &swapInt forParameter : NSOpenGLCPSwapInterval];

	return SUCCESS;

#endif

}

GReturn GOpenGL::GetCount(unsigned int& _outCount)
{
	_outCount = refCount;

	return SUCCESS;
}

GReturn GOpenGL::IncrementCount()
{
	if (refCount == G_UINT_MAX)
		return FAILURE;

	++refCount;

	return SUCCESS;
}

GReturn GOpenGL::DecrementCount()
{
	if (refCount == 0)
		return FAILURE;

	--refCount;

	if (refCount == 0)
	{
		delete this;
	}


	return SUCCESS;
}

GReturn GOpenGL::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
{
	if (_outputInterface == nullptr)
		return INVALID_ARGUMENT;

	if (_interfaceID == GWindowUUIID)
	{
		GWindow* convert = reinterpret_cast<GWindow*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GBroadcastingUUIID)
	{
		GBroadcasting* convert = reinterpret_cast<GBroadcasting*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GMultiThreadedUUIID)
	{
		GMultiThreaded* convert = reinterpret_cast<GMultiThreaded*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GInterfaceUUIID)
	{
		GInterface* convert = reinterpret_cast<GInterface*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GOpenGLSurfaceUUIID)
	{
		GOpenGLSurface* convert = reinterpret_cast<GOpenGLSurface*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else
		return INTERFACE_UNSUPPORTED;

	return SUCCESS;
}

GReturn GOpenGL::OnEvent(const GUUIID & _senderInterface, unsigned int _eventID, void * _eventData, unsigned int _dataSize)
{

	if (_senderInterface == GWindowUUIID)
	{

		GWINDOW_EVENT_DATA* eventStruct = (GWINDOW_EVENT_DATA*)_eventData;

#ifdef _WIN32

        switch (_eventID)
        {
            case GW::SYSTEM::MAXIMIZE:
            case GW::SYSTEM::RESIZE:
            {
                unsigned int maxWidth;
                unsigned int maxHeight;
                unsigned int currX;
                unsigned int currY;

                gWnd->GetClientWidth(maxWidth);
                gWnd->GetClientHeight(maxHeight);
                gWnd->GetClientTopLeft(currX, currY);

                aspectRatio = (float)maxWidth / (float)maxHeight;

                glViewport(0, 0, maxWidth, maxHeight);
            }
                break;
            case GW::SYSTEM::MOVE:
            {
                unsigned int maxWidth;
                unsigned int maxHeight;
                unsigned int currX;
                unsigned int currY;

                gWnd->GetClientWidth(maxWidth);
                gWnd->GetClientHeight(maxHeight);
                gWnd->GetClientTopLeft(currX, currY);

                glViewport(0, 0, maxWidth, maxHeight);
            }
                break;
            case GW::SYSTEM::DESTROY:
            {
                this->~GOpenGL();
            }
                break;
        }

#elif __linux__
XLockDisplay((Display*)lWnd.display);
        switch (_eventID)
        {
            case GW::SYSTEM::MAXIMIZE:
            case GW::SYSTEM::RESIZE:
            {
                unsigned int maxWidth;
                unsigned int maxHeight;
                unsigned int currX;
                unsigned int currY;

                gWnd->GetWidth(maxWidth);
                gWnd->GetHeight(maxHeight);
                gWnd->GetClientTopLeft(currX, currY);

                aspectRatio = maxWidth / maxHeight;

                glViewport(currX, currY, maxWidth, maxHeight);
            }
                break;
            case GW::SYSTEM::MOVE:
            {
                unsigned int maxWidth;
                unsigned int maxHeight;
                unsigned int currX;
                unsigned int currY;

                gWnd->GetWidth(maxWidth);
                gWnd->GetHeight(maxHeight);
                gWnd->GetClientTopLeft(currX, currY);

                glViewport(currX, currY, maxWidth, maxHeight);
            }
                break;
            case GW::SYSTEM::DESTROY:
            {
                this->~GOpenGL();
            }
                break;
        }
XUnlockDisplay((Display*)lWnd.display);
#elif __APPLE__

        switch (_eventID)
        {
            case GW::SYSTEM::MAXIMIZE:
            case GW::SYSTEM::RESIZE:
            {
                unsigned int maxWidth;
                unsigned int maxHeight;

                gWnd->GetClientWidth(maxWidth);
                gWnd->GetClientHeight(maxHeight);

                aspectRatio = (float)maxWidth / (float)maxHeight;

                //glViewport(0, 0, maxWidth, maxHeight);
            }
                break;
            case GW::SYSTEM::MOVE:
            {
                unsigned int maxWidth;
                unsigned int maxHeight;

                gWnd->GetClientWidth(maxWidth);
                gWnd->GetClientHeight(maxHeight);

                glViewport(0, 0, maxWidth, maxHeight);
            }
                break;
            case GW::SYSTEM::DESTROY:
            {
				gWnd->DecrementCount(); // release our hold on GWindow
				gWnd = nullptr; // pointer is no longer valid
            }
                break;
        }


#endif

	}

	return SUCCESS;
}

GATEWARE_EXPORT_EXPLICIT GReturn CreateGOpenGLSurface(SYSTEM::GWindow* _gWin, unsigned long long _initMask, GOpenGLSurface** _outSurface)
{
	return GW::GRAPHICS::CreateGOpenGLSurface(_gWin, _initMask, _outSurface);
}

GReturn GW::GRAPHICS::CreateGOpenGLSurface(SYSTEM::GWindow* _gWin, unsigned long long _initMask, GOpenGLSurface** _outSurface)
{
	GReturn r; // result of init
	if (_outSurface == nullptr || _gWin == nullptr)
		return INVALID_ARGUMENT;

	GOpenGL* Surface = new GOpenGL();

	if (Surface == nullptr)
		return FAILURE;

	Surface->SetGWindow(_gWin);
	if (G_SUCCESS(r = Surface->Initialize(_initMask)))
	{
		if (G_SUCCESS(r = _gWin->RegisterListener(Surface, 0)))
		{
			*_outSurface = Surface;
			return SUCCESS;
		}
	}
	// didn't work...
	return r;
}
