#ifndef GDIRECTX11SURFACE_H
#define GDIRECTX11SURFACE_H

/*!
File: GDirectX11Surface.h
Purpose: A Gateware interface initializes a DirectX11 rendering surface and manages it's core resources.
Author: Andre Reid
Contributors: N/A
Last Modified: 11/17/2017
Interface Status: Beta
Copyright: 7thGate Software LLC.
License: MIT
*/

//! GDirectX11Surface inherits directly from GListener.
#include "../../Interface/G_Core/GListener.h"
#include "../../Interface/G_System/GWindow.h"
#include "GGraphicsDefines.h"


//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The namespace to which all Gateware Graphics library interfaces must belong.
	namespace GRAPHICS
	{
		//! Unique Identifier for this interface. {F787FD1F-6218-422A-B35A-41B4B48B75FC}
		static const GUUIID GDirectX11SurfaceUUIID =
		{ 
			0xf787fd1f, 0x6218, 0x422a,{ 0xb3, 0x5a, 0x41, 0xb4, 0xb4, 0x8b, 0x75, 0xfc } 
		};

		//! A library used to initialize, create, and manage a DirectX11 rendering context.
		/*!
		*	This library automates the creation of a basic DirectX11
		*	rendering context, and is capable of accepting special requests
		*	that can guide the initialization to support requested options.
		*	GDirectX11Surface is a GListener, which allows it to receive
		*	events from a registered broadcaster and react accordingly.
		*/class GDirectX11Surface : public CORE::GListener
		{

		public:

			//! Returns the aspect ratio for the current window.
			/*!
			*
			*	\param [out] _outRatio Will contain the calculated aspect ratio.	
			*
			*	\retval SUCCESS The current aspect ratio was calculated and returned.
			*   \retval FAILURE No active GWindow exists to calculate an aspect ratio from.
			*/
			virtual GReturn GetAspectRatio(float& _outRatio) = 0;
			//! Returns the address of the current ID3D11Device.
			/*!
			*
			*	\param [out] _outDevice Will contain the address of the device.
			*
			*	\retval SUCCESS A DirectX11 device exists and was returned.
			*   \retval FAILURE No device exists to retrieve.
			*/
			virtual GReturn GetDevice(void** _outDevice) = 0;
			//! Returns the address of the current ID3D11DeviceContext.
			/*!
			*
			*	\param [out] _outContext Will contain the address of the context.
			*
			*	\retval SUCCESS A DirectX11 contexts exists and was returned.
			*   \retval FAILURE No context exists to retrieve.
			*/
			virtual GReturn GetContext(void** _outContext) = 0;
			//! Returns the address of the current IDXGISwapChain.
			/*!
			*
			*	\param [out] _outSwapchain Will contain the address of the swap chain.
			*
			*	\retval SUCCESS A DirectX11 swap chain exists and was returned.
			*   \retval FAILURE No swap chain exists to retrieve.
			*/
			virtual GReturn GetSwapchain(void** _outSwapchain) = 0;
			//! Returns the address of the current ID3D11RenderTargetView.
			/*!
			*
			*	\param [out] _outRenderTarget Will contain the address of the render target view.
			*
			*	\retval SUCCESS A DirectX11 render target view exists and was returned.
			*   \retval FAILURE No render target view exists to retrieve.
			*/
			virtual GReturn GetRenderTarget(void** _outRenderTarget) = 0;
			//! Returns the address of the current ID3D11DepthStencilView.
			/*!
			*	A Depth Stencil View will only be created if requested as
			*	a special option when the 'Initialize' method is called.
			*
			*	\param [out] _outDepthStencilView Will contain the address of the depth stencil view.
			*
			*	\retval SUCCESS A DirectX11 depth stencil view exists and was returned.
			*   \retval FAILURE No depth stencil view exists to retrieve.
			*/
			virtual GReturn GetDepthStencilView(void** _outDepthStencilView) = 0;
		}; // end GDirectX11Surface class

		   //! Creates and outputs a new GDirectX11Surface object.
		   /*!
		   *	Initializes a handle to a GDirectX11Surface object with an existing GWindow.
		   *	The created GDirectX11Surface object will have its reference count initialized
		   *	to one and register as a listener to the provided GWindow object.
		   *
		   *	This function accepts a bit mask that can hold
		   *	supported 'GGraphicsInitOptions', which will
		   *	be taken into account when creating the context.
		   *	To ignore this mask, simply pass in 0 when calling
		   *	this function and the context will be created with
		   *	default settings.
		   *
		   *	\param [in] _gWin A pointer to an existing GWindow object.
		   *	\param [in] _initMask The bit mask that can hold special initialization options.
		   *	\param [out] _outSurface Will contain the GDirectX11Surface object if successfully created.
		   *
		   *	\retval SUCCESS A GDirectX11Surface object was successfully created.
		   *	\retval FAILURE A GDirectX11Surface object was not created. _outSurface will be null.
		   *	\retval INVALID_ARGUMENT Either the _gWin, _outSurface or both arguments are nullptrs.
		   */
		GATEWARE_EXPORT_IMPLICIT GReturn CreateGDirectX11Surface(SYSTEM::GWindow* _gWin, unsigned long long _initMask, GDirectX11Surface** _outSurface);

	} // end GRAPHICS namespace
} // end GW namespace

#endif