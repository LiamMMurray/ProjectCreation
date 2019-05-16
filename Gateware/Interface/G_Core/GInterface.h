#ifndef GINTERFACE_H
#define GINTERFACE_H

/*!
	File: GInterface.h
	Purpose: The fundamental interface which all Gateware interfaces must adhere to at a minimum.
	Author: Lari H. Norri
	Contributors: N/A
	Last Modified: 9/29/2016
	Interface Status: Final
	Copyright: 7thGate Software LLC.
	License: MIT
*/

//! Contains all defined elements shared among base interfaces.
#include "GDefines.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The core namespace to which all Gateware fundamental interfaces must belong.
	namespace CORE
	{
		//! Unique Identifier for this interface. {F73C8B73-7C68-4FEE-BF82-83C267277313}.
		static const GUUIID GInterfaceUUIID =
		{
			0xf73c8b73, 0x7c68, 0x4fee,{ 0xbf, 0x82, 0x83, 0xc2, 0x67, 0x27, 0x73, 0x13 }
		};

		//! Base interface all Gateware interfaces must support at a minimum.
		/*! 
		*	Core features include: Interface Upgrades, Reference Counting, Event Broadcasting.
		*/
		class GInterface
		{
			// All Gateware API interfaces contain no variables & are pure virtual.
		public:
			//! Return the total number of active references to this object.
			/*!
			*	\param [out] _outCount The total number of active references of this object.
			*
			*	\retval SUCCESS Successfully ran.
			*	\retval FAILURE Either class does not exist or the internal reference count is corrupt.
			*/
			virtual GReturn GetCount(unsigned int& _outCount) = 0;
			
			//! Increase the total number of active references to this object.
			/*!
			*	End users should only call this operation if they are familiar with reference counting behavior.
			*
			*	\retval SUCCESS Successfully incremented the internal reference count.
			*	\retval FAILURE Incrementation of internal reference count would overflow the value.
			*/
			virtual GReturn IncrementCount() = 0;

			//! Decrease the total number of active references to this object.
			/*!
			*	Once the internal count reaches zero this object will be deallocated and your pointer will become invalid.
			*
			*	\retval SUCCESS Successfully decremented the internal reference count.
			*	\retval FAILURE Decrementing of internal reference count would underflow the value.
			*/
			virtual GReturn DecrementCount() = 0;

			//! Requests an interface that may or may not be supported by this object.
			/*!
			*	 Can be used by the end-user to query for a new interface using the 
			*	 unique ID of the interface they want and implement an interface update.
			*
			*	\param [in] _interfaceID The GUUIID of the interface you are requesting.
			*	\param [out] _outputInterface Where the interface will be stored if function is successful.
			*
			*	\retval SUCCESS The interface is supported and function succeded.
			*	\retval INTERFACE_UNSUPPORTED The requested interface is not supported.
			*/
			virtual GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface) = 0;
		};
	}// end CORE namespace
};// end GW namespace

#endif