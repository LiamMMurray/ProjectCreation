#pragma once
#ifndef GMUSIC_H
#define GMUSIC_H

/*!
File: GMusic.h
Purpose: A Gateware internal interface that handles music.
Author: TJay Cargle
Contributors: N/A
Last Modified: 8/12/2017
Interface Status: Complete
Copyright: 7thGate Software LLC.
License: MIT
*/

#include "../G_Core/GDefines.h"
#include "../G_Core/GMultiThreaded.h"

//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The namespace to which all Gateware library interfaces must belong.
	namespace AUDIO
	{

		//! Ensure identical binary padding for structures on all platforms.
#pragma pack(push, 1)

#pragma pack(pop)



		//! Unique Identifier for this interface.  {86C6E985-BFF0-499F-80E9-F0EE2271813A}.
		static const GUUIID GMusicUUIID =
		{
		
			0x86c6e985, 0xbff0, 0x499f,{ 0x80, 0xe9, 0xf0, 0xee, 0x22, 0x71, 0x81, 0x3a }

		};
		class GMusic : public GW::CORE::GMultiThreaded
		{

		public:
			// Not yet Implemented
			//virtual GReturn SetPCMShader(const char* _data) = 0;//function ptr
            
			//! Attempts to set the output volume for the specified number of outputs to the passed in values.
			/*!
			* The amount of values in _values that will be used is based on _numChannels.
			* If you attempt to set the volume of an output your hardware does not support, it will be ignored.
			* If you attempt to set a volume higher that GAudio's master volume, it will be set to the master volume without an error message.
			*
			*	Channels: Supports up to 6 channels AKA 5.1 Audio: 
			*	INDEX[0] = Left, 
			*	INDEX[1] = Right, 
			*	INDEX[2] = Front Center, 
			*	INDEX[3] = LFE, 
			*	INDEX[4] = Rear Left, 
			*	INDEX[5] = Rear Right, 
			*
			*	\param [in] _values The output volumes to be set.
			*	\param [in] _numChannels The number of channels affected.
			*
			*	\retval FAILURE _numChannels is less than 1.
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateMusicStream".
			*	\retval FAILURE  If internal object incounters an error. Could occur if GAudio's number of outputs changed durring runtime.
			*	\retval FAILURE _values is NULL
			*	\retval FAILURE _values tried to read garbadge data - (_numChannels > amoutn of values in _values)
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn SetChannelVolumes(float * _values, int _numChannels) = 0;

			//! Attempts to change the overall volume.
			/*!
			* If you attempt to set a volume higher that GAudio's master volume, it will be set to the master volume without an error message.
			*
			*	\param [in] _newVolume The output volume to be set.
			*
			*	\retval FAILURE GAudio has not filled out this object with GAudio's fuunction: "CreateMusicStream".
			*	\retval FAILURE _newVolume is less than Zero.
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateMusicStream" did not return an error upon filling out this object.			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*/
			virtual GReturn SetVolume(float _newVolume) = 0;

			//! Attempts to start a music stream.
			/*!
			* Currenntly only supports .wav files.
			* Upon Succession will launch a new thread that will continously read in audio data from file.
			* If a thread already has been launched, will not attempt to launch new thread without returning an error.
			*
			*	\retval FAILURE GAudio has not filled out this object with GAudio's fuunction: "CreateMusicStream".
			*	\retval FAILURE If specified .wav file could not be found or encountered an error upon reading in data (This happens when "CreateMusicStream" fills out data).
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateMusicStream" did not return an error upon filling out this object.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn StreamStart(bool _loop = false) = 0;

			//! Attempts to pause a currently playing music stream.
			/*!
			* Currenntly only supports .wav files.
			* If a stream is currently paused, will not attempt to pause playback without returning an error.
			*
			*	\param [in][optional] _loop Bool for whether or not should play looping.
			*
			*	\retval FAILURE GAudio has not filled out this object with GAudio's fuunction: "CreateMusicStream".
			*	\retval FAILURE If specified .wav file could not be found or encountered an error upon reading in data (This happens when "CreateMusicStream" fills out data).
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateMusicStream" did not return an error upon filling out this object.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn PauseStream() = 0;


			//! Attempts to resume a currently paused music stream.
			/*!
			* Currenntly only supports .wav files.
			* If a music stream is currently playing, will not attempt to play file without returning an error.
			*
			*	\retval FAILURE GAudio has not filled out this object with GAudio's fuunction: "CreateMusicStream".
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateMusicStream" did not return an error upon filling out this object.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn ResumeStream() = 0;

			//! Attempts to stop playback of a music stream and reset it to begining for future use.
			/*!
			* Currenntly only supports .wav files.
			* Tells the previously launched thread to stop, calls join on it, and resets thread for future use.
			* Will attempt to comppletely stop playback regaurdless of whether or not music is playing or paused.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateMusicStream".
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateMusicStream" did not return an error upon filling out this object.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn StopStream() = 0;

			//! Stores the .wav files internal amount of channels the sound was recorded with into the passed in unsigned int.
			/*!
			* This value is read and stored upon creation of GMusic.
			*
			*	\param [out] The unsigned int which will be stored with the value.
			*
			*	\retval FAILURE GAudio has not filled out this object with GAudio's fuunction: "CreateMusicStream".
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn GetStreamSourceChannels(unsigned int & returnedChannelNum) = 0;


			//! Stores the number of specified outputs into the passed in unsigned int.
			/*!
			* This value is obtained from the GAudio that created this GMusic.
			* By default the number of output channels is 2 unless GAudio was created with a different amount.
			*
			*	\param [out] The unsigned int which will be stored with the value.
			*
			*	\retval FAILURE GAudio has not filled out this object with GAudio's fuunction: "CreateMusicStream".
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn GetStreamOutputChannels(unsigned int & returnedChannelNum) = 0;

			//! Fills out the passed in bool with information as to if the music stream is playing or not.
			/*!
			* GMusic internally keeps track of whether or not it playing via the launched thread, as well as the Start/Pause/Resume/Stop functions.
			*
			*	\param [out] The bool which will be stored with the value.
			*
			*	\retval SUCCESS Always, boolean passed in will tell you if music is playing or not.
			*
			*/
			virtual GReturn isStreamPlaying(bool & _returnedBool) = 0;

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
			virtual ~GMusic() = 0;
		}; // end GMusic class


	} // end SYSTEM namespace
} // end GW namespace
#endif
