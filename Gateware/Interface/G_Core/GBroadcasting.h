#ifndef GBROADCASTING_H
#define GBROADCASTING_H

/*!
	File: GBroadcasting.h
	Purpose: A Gateware interface that allows an object derived from GListener to receive select events from a GBroadcaster.
	Author: Lari H. Norri
	Contributors: N/A
	Last Modified: 10/13/2016
	Interface Status: Final
	Copyright: 7thGate Software LLC.
	License: MIT
*/

#include "GListener.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The core namespace to which all Gateware fundamental interfaces must belong.
	namespace CORE
	{
		//! Unique Identifier for this interface. {FF23C564-940B-44A2-9E5F-CE264659D69A}
		static const GUUIID GBroadcastingUUIID =
		{
			0xff23c564, 0x940b, 0x44a2,{ 0x9e, 0x5f, 0xce, 0x26, 0x46, 0x59, 0xd6, 0x9a }
		};

		//! The GBroadcasting Interface is capable of registering & deregistering GListener interfaces.
		/*! 
		*	The GBroadcaster will notify all registered listeners with the listeners OnEvent function.
		*	The events being registered for can be filtered with the _eventMask (optional).
		*	GBroadcasting Inherits directly from GMultiThreaded, therefore its implementation
		*	must be thread safe.
		*/
		class GBroadcasting : public GMultiThreaded
		{
			// All Gateware API interfaces contain no variables & are pure virtual.
		public:

			//! Any listener added to this class must receive all events unless otherwise specified by the _eventMask (optional).
			/*!
			*	Listeners registered to a broadcaster will have their reference counts increased by one until deregistered.
			*	
			*
			*	\param [in] _addListener The listener object that is registering for messages.
			*	\param [in] _eventMask The events the listener is registering for. 0 will register for all events.
			*
			*	\retval SUCCESS The listener was successfully registered.
			*	\retval REDUNDANT_OPERATION The listener has already been registered by a previous call.
			*/
			 virtual GReturn RegisterListener(GListener* _addListener, unsigned long long _eventMask) = 0;
			
			//! A successfully deregistered listener will no longer receive events and have its reference count decremented by one.
			/*!
			*	\param [in] _removeListener The listener to deregister from events.
			*
			*	\retval SUCCESS The listener was successfully deregistered.
			*/
			virtual GReturn DeregisterListener(GListener* _removeListener) = 0;
		};
	}// end CORE namespace
};// end GW namespace

#endif
