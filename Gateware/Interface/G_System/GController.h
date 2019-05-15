#ifndef GCONTROLLER_H
#define GCONTROLLER_H

/*!
File: GController.h
Purpose: A Gateware interface handles controller input and provides disconnection and connection events
Author: Devin Wright
Contributors: N/A
Last Modified: 11/29/2018
Interface Status: Beta
Copyright: 7thGate Software LLC.
License: MIT
*/

//! GController inherits directly from GBroadcasting.
#include "../G_Core/GBroadcasting.h"
#include "../G_System/GControllerInputDefines.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The namespace to which all Gateware library interfaces must belong.
	namespace SYSTEM
	{


//! Ensure identical binary padding for structures on all platforms.
#pragma pack(push, 1)

		//! GCONTROLLER_EVENT_DATA will hold the information about a controller event 
		/*
		*	If the corresponding event is an Input Event inputCode and inputValue will contain the input code and value 
		*	respectively for the detected input. If the corresponding event is an Connection event inputCode and inputValue will be 0. 
		*	isConnected will be 0 for false and 1 for true.
		*	
		*/
		struct GCONTROLLER_EVENT_DATA
		{
			int controllerIndex; /*!< The Controller index where the event took place */ 
			int controllerID;	 /*!<  The Controller ID of the controller that sent the event*/
			int inputCode;		 /*!< The Code repsenting the Detected input */
			float inputValue;	 /*!< The value of the detected input */
			int isConnected;	 /*!< Value indicating whether the controller is connected */
		};
#pragma pack(pop)

		//! GControllerEvents holds the possible events that can be sent from GController
		enum GControllerEvents
		{
			CONTROLLERBUTTONVALUECHANGED,		/*!< Input event for button value change */
			CONTROLLERAXISVALUECHANGED,			/*!< Input event for axis value changed */
			CONTROLLERCONNECTED,				/*!< Connection event for a controller being connected */
			CONTROLLERDISCONNECTED				/*!< Connection event for a controller being disconnected */
		};

		//! GControllerDeadzoneTypes holds the diffrent type of deadzone calculations
		enum GControllerDeadzoneTypes
		{
			DEADZONESQUARE,	/*!< Calulates the deadzone of x and y separately*/
			DEADZONECIRCLE	/*!< Calulates the deadzone of x and y as a single magnitude*/
		};

		//! Unique Identifier for this interface. {DFF7EEBF-C333-4606-9500-BDE01CE7C1CB}
		static const GUUIID GControllerUUIID = 
		{
			0xdff7eebf, 0xc333, 0x4606,{ 0x95, 0x0, 0xbd, 0xe0, 0x1c, 0xe7, 0xc1, 0xcb }
		};


		//! A multi-threaded controller input library.
		/*!
		*	This library can be used to poll the current state of connected controllers.
		*	The controllers are assigned indices in a array for the lifetime that they are connected.
		*	It also provides events for controller connections and disconnections,
		*	which a GListener can be writen to receive.
		*/
		class GController : public CORE::GBroadcasting
		{
		public:
			//! Used to poll the current state of a button or axis on a controller
			/*
			*	Use an Inputcode to check the state of a button or axis on the controller
			*	at the selected index. If a button is being checked the _outState will be 0 for 
			*	up and 1 for down. If a axis is being checked the _outState will be between -1 and 1 inclusively.
			*
			*	\param [in] _controllerIndex The controller index to check
			*	\param [in] _inputCode The input code for the button/axis to check
			*	\param [out] _outState Is a reference to a float to store the state.
			*
			*	\retval SUCCESS The button/axis state was was successfully stored in the out-param
			*	\retval FAILURE	No controller is connected at the chosen index.
			*	\retval INVALID_ARGUMENT Ether _controllerIndex is out of range or _inputCode is invaild. 
			*/
			virtual GReturn GetState(int _controllerIndex, int _inputCode, float& _outState) = 0;

			//! Used to check if a controller is connected at a specified index
			/*
			*	\param [in] _controllerIndex The controller index to check.
			*	\param [out] _outIsConnected Is a reference to a bool to store whether a controller is at the index.
			*
			*	\retval SUCCESS The out-param was successfully filled out
			*	\retval INVALID_ARGUMENT _controllerIndex was out of range.
			*/
			virtual GReturn IsConnected(int _controllerIndex, bool& _outIsConnected) = 0;

			//! Outputs the max index of the array of controllers.
			/*
			*	The controllers will be stored in a constant array where the size of the array
			*	is the maxium amnout of supported controllers.
			*
			*	\param [out] _outMax is a reference to a int to store the max index.
			*
			*	\retval SUCCESS The max index was successfully stored in the out-param
			*/
			virtual GReturn GetMaxIndex(int &_outMax) = 0;

			//! Outputs the number of controllers connected
			/*
			*	The number returned will be how many controllers are currently stored in the 
			*	controller array.
			*
			*	\param [out] _outConnectedCount is a reference to a int to store the count of connected controllers
			*
			*	\retval SUCCESS The count of connected controllers was successfully stored in the out-param
			*/
			virtual GReturn GetNumConnected(int &_outConnectedCount) = 0;

			//! Set how the stick deadzones should be calculated
			/*
			*	Default deadzone is _type = DEADZONESQUARE, _deadzonePercentage = .2f
			*
			*	\param [in] _type Specifies the dead zone calculation to be used.
			*	\param [in] _deadzonePercentage The size of the deadzone.
			*
			*	\retval SUCCESS The deadzone was set to the new parameters
			*	\retval INVALID_ARGUMENT _deadzonePercentage is invaild
			*/
			virtual GReturn SetDeadZone(GControllerDeadzoneTypes _type, float _deadzonePercentage) = 0;

			//! Start vibration in selected controller
			/*
			*	This will not replace a currently running vibration, the previous vibration must end or be stopped first.
			*	Controllers have diffrent start up times and strengths this method does not account for this.
			*
			*	\param [in] _pan is a -1 to 1 ratio where -1 is full left motor, 1 is full right motor.
			*	\param [in] _duration In seconds how long the vibration will run.
			*	\param [in] _strength 0 to 1 ratio of how strong the vibration will be.
			*	\param [in] _controllerIndex The controller to vibrate.
			*
			*	\retval SUCCESS The vibration has been started in the selected controller.
			*	\retval	INVALID_ARGUMENT One or more arguments are out of range.
			*	\retval FAILURE Controller is currently vibrating.
			*	\retval FEATURE_UNSUPPORTED Vibration for the current controller type is not supported.
			*/
			virtual GReturn StartVibration(float _pan, float _duration, float _strength, unsigned int _controllerIndex) = 0;

			//! Used to check if a controller is currently vibrating
			/*
			*	\param [in] _controllerIndex The controller index to check.
			*	\param [out] _outIsVibrating Is a reference to a bool to store whether a controller is vibrating.
			*
			*	\retval SUCCESS The out-param was successfully filled out
			*	\retval INVALID_ARGUMENT _controllerIndex was out of range.
			*	\retval FEATURE_UNSUPPORTED Vibration for the current controller type is not supported.
			*/
			virtual GReturn IsVibrating(unsigned int _controllerIndex, bool& _outIsVibrating) = 0;

			//! Used to stop a controller from vibrating 
			/*
			*	\param [in] _controllerIndex The controller index to stop
			*
			*	\retval SUCCESS The virbration was stop
			*	\retval INVALID_ARGUMENT _controllerIndex was out of range.
			*	\retval	REDUNDANT_OPERATION The controller was not vibrating. 
			*	\retval FEATURE_UNSUPPORTED Vibration for the current controller type is not supported.
			*/
			virtual GReturn StopVirbration(unsigned int _controllerIndex) = 0;

			//! Stops all currently vibrating controllers
			/*
			*	\retval SUCCESS All vibrations were stopped
			*	\retval FEATURE_UNSUPPORTED Vibration for the current controller type is not supported.
			*/
			virtual GReturn StopAllVirbrations() = 0;
		
		}; //end GController class

		//! Creates a new GController object
		/*
		*	Initializes a handle to a GConntroller object of the type specified.
		*	Created GConntroller object will have its reference count initialized to one.
		*
		*	\param [in] _controllerType The type of Controller to be supported
		*	\param [out] _outController Will contain GConntroller object if successfully created.
		*
		*	\retval SUCCESS GConntroller was successfully created
		*	\retval	FAILURE GConntroller was not created. _outController will be NULL.
		*	\retval	INVALID_ARGUMENT _controllerType is invaild or GController is NULL.
		*	\retval	FEATURE_UNSUPPORTED	_controllerType is not currently supported. _outController will be NULL.
		*/
		GATEWARE_EXPORT_IMPLICIT GReturn CreateGController(int _controllerType, GController** _outController);
	} // end of SYSTEM namespace
} // end of GW namespace
#endif 

