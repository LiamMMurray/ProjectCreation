#ifndef GLISTENER_H
#define GLISTENER_H

/*!
	File: GListener.h
	Purpose: A Gateware interface that can safely listen & respond to events sent from a GBroadcating interface.
	Author: Lari H. Norri
	Contributors: N/A
	Last Modified: 10/13/2016
	Interface Status: Final
	Copyright: 7thGate Software LLC.
	License: MIT
*/

//! GListener inherits directly from GMultiThreaded.
#include "GMultiThreaded.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The core namespace to which all Gateware fundamental interfaces must belong.
	namespace CORE
	{
		//! Unique Identifier for this interface. {6512ACFA-2BE3-4431-B81C-9B65398A4C90}
		static const GUUIID GListenerUUIID = 
		{
			0x6512acfa, 0x2be3, 0x4431,{ 0xb8, 0x1c, 0x9b, 0x65, 0x39, 0x8a, 0x4c, 0x90 }
		};

		//! A GListener Interface may be registered with a GBroadcaster interface to receive event notifications.
		/*!
		*	GListener is directly inherited from GMultiThreaded, therefore its implementation 
		*	must be thread safe.
		*/
		class GListener : public GMultiThreaded
		{
			// All Gateware API interfaces contain no variables & are pure virtual.
		public:
			//! This operation is called whenever a GBroadcaster a listener is registered to generates an event. 
			/*!
			*	\param [in] _senderInterface The interface of the sender object.
			*	\param [in] _eventID The ID of the event sent.
			*	\param [in] _eventData The data of the event.
			*	\param [in] _dataSize The size of _eventData in bytes.
			*/
			virtual GReturn OnEvent(const GUUIID& _senderInterface, unsigned int _eventID, void* _eventData, unsigned int _dataSize) = 0;
		};
	}// end CORE namespace
};// end GW namespace

#endif