#ifndef GLOG_H
#define GLOG_H

/*!
	File: GLog.h
	Purpose: A Gateware interface that handles logging in a thread safe manner.
	Author: Justin W. Parks
	Contributors: N/A
	Last Modified: 8/21/2017
	Interface Status: Beta
	Copyright: 7thGate Software LLC.
	License: MIT
*/

//! GFile inherits directly from GMultiThreaded. 
#include "../G_Core/GMultiThreaded.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The namespace to which all Gateware library interfaces must belong.
	namespace SYSTEM
	{
		//! Unique Identifier for this interface. {305FDF22-A4AD-40B9-92B0-E0F9DDC97D1E}
		static const GUUIID GLogUUIID =
		{
			0x305fdf22, 0xa4ad, 0x40b9,{ 0x92, 0xb0, 0xe0, 0xf9, 0xdd, 0xc9, 0x7d, 0x1e }
		};
	
		// Forward declaration so we can use GFile* in a function.
		class GFile;


		//! Cross platform threadsafe logger.
		/*! 
		*	GLog inherits directly from GMultiThreaded, therefore its implementation 
		*	must be thread safe.
		*/
		class GLog : public CORE::GMultiThreaded
		{
			// All Gateware API interfaces contain no variables & are pure virtual.
		public:

			//! Logs a null terminated string.
			/*!
			*	Date, Time, and thread ID will be appended to the front of the message unless 
			*	specified otherwise (See EnableVerboseLogging). A new line character will be 
			*	appended to the end of the string so your log messages do not require a new line. 
			*	The string is logged to the internal GFile object.
			*
			*	\param [in] _log The message to log out.
			*
			*	\retval SUCCESS  Successfully queued the message to the log.
			*	\retval FAILURE  The queue has reached maximum size (call flush).
			*	\retval INVALID_ARGUMENT  A nullptr was passed in.
			*/
			virtual GReturn Log(const char* const _log) = 0;
			
			//! Logs a null terminated string with a category.
			/*!
			*	Date, Time, and thread ID will be appended to the front of the message unless 
			*	specified otherwise (See EnableVerboseLogging). A new line character will be 
			*	appended to the end of the string so your log messages do not require a new line. 
			*	The string is logged to the internal GFile object.
			*
			*	\param [in] _category The category the log belongs in. ie. ERROR, WARNING, INFO, etc.
			*	\param [in] _log The message to log out.
			*
			*	\retval SUCCESS  Successfully queued the message to the log.
			*	\retval FAILURE  The queue has reached maximum size (call flush).
			*	\retval INVALID_ARGUMENT  Either _category or _log are nullptr.
			*/
			virtual GReturn LogCatergorized(const char* const _category, const char* const _log) = 0;
			
			//! Turns verbose logging on or off.
			/*!
			*	Use this function to ensure or prevent the addition of date, time, and threadID
			*	to your log messages.
			*
			*	\param [in] _value true to turn on or false to turn off.
			*/
			virtual GReturn EnableVerboseLogging(bool _value) = 0;
			
			//! Turns console logging on or off.
			/*!
			*	Use this function to ensure or prevent the additional console logging.
			*
			*	\param [in] _value true to turn on or false to turn off.
			*/
			virtual GReturn EnableConsoleLogging(bool _value) = 0;
			
			//! Forces a log dump to file.
			/*!
			*	This will force a log dump to the file and clear the log queue.
			*
			*	\retval SUCCESS  Successfully dumped the logs.
			*	\retval FAILURE  Most likely a file corruption or a file is not open.
			*/
			virtual GReturn Flush() = 0;
		}; // end GLog class

		//! Creates a GLog object.
		/*!
		*	This function will create a GLog object with the log file being created in the directory
		*	the program was ran from. If you want to control where the log file is going to be created 
		*	then use the custom function below to pass in a GFile* that is pre set the way you want it.
		*	Reference count of created object is initialized to one.
		*
		*	\param [in] _fileName The name of the log file.
		*	\param [out] _outLog Will contain the GLog if successfully created.
		*
		*	\retval SUCCESS  GLog was successfully created.
		*	\retval FAILURE  GLog was not created. _outLog will be null.
		*	\retval INVALID_ARGUMENT  Either one or both arguments are nullptrs.
		*/
		GATEWARE_EXPORT_IMPLICIT GReturn CreateGLog(const char* const _fileName, GLog** _outLog);

		//! Creates a GLog object.
		/*!
		*	This function will create a GLog object with the GFile object that was passed in.
		*	This is so you can have more control over your log by setting up a GFile in advance.
		*	The GFile object should already have a file open for text writing.
		*	Created GLog object will have its reference count initialized to one.
		*	
		*	\param [in] _file The GFile object that this log will use.
		*	\param [out] _outLog Will contain the GLog object if successfully created.
		*
		*	\retval SUCCESS  GLog was successfully created.
		*	\retval FAILURE  GLog was not created. _outLog will be null.
		*	\retval INVALID_ARGUMENT Either one or both the arguments are nullptr.
		*/
		GATEWARE_EXPORT_IMPLICIT GReturn CreateGLogCustom(GFile* _file, GLog** _outLog);
	} // end CORE namespace
} // end GW namespace

#endif