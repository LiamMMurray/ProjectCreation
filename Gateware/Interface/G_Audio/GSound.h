#pragma once
#ifndef GSound_H
#define GSound_H

/*!
File: GAudio.h
Purpose: A Gateware internal interface that handles sounds.
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



		//! Unique Identifier for this interface. {0004A714-6E71-4A9D-B1F9-23E2468D2817}.
		static const GUUIID GSoundUUIID =
		{

		 0x4a714, 0x6e71, 0x4a9d,{ 0xb1, 0xf9, 0x23, 0xe2, 0x46, 0x8d, 0x28, 0x17 }

		};
		class GSound : public GW::CORE::GMultiThreaded
		{

		public:
		
			//SetPCMShader is not yet implemented
		//	virtual GReturn SetPCMShader(const char* _data) = 0;

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
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval FAILURE Internal Source voice is NULL - (which could happen if specified .wav file could not be found or encountered an error upon reading in data)
			*	\retval FAILURE _values is NULL
			*	\retval FAILURE _values tried to read garbadge data - (_numChannels > amoutn of values in _values)
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn SetChannelVolumes(float *_values, int _numChannels) = 0;

			//! Attempts to change the overall volume.
			/*!
			* The amount of values in _values that will be used is based on _numChannels.
			* If you attempt to set a volume higher that GAudio's master volume, it will be set to the master volume without an error message.
			*
			*	\param [in] _newVolume The output volume to be set.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval FAILURE _newVolume is less than Zero.
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateSound" did not return an error upon filling out this object.			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*/
			virtual GReturn SetVolume(float _newVolume) = 0;

			//! Attempts to playback a sound effect.
			/*!
			* Currenntly only supports .wav files. 
			* GSound fully loads entire audio file into memory. If file is extremely large, consider using GMusic instead.
			* If sound is currently playing, will attempt to stop and restart playback.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval FAILURE Internal Source voice is NULL - (which could happen if specified .wav file could not be found or encountered an error upon reading in data).
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateSound" did not return an error upon filling out this object.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn Play() = 0;

			//! Attempts to pause a currently playing sound effect.
			/*!
			* Currenntly only supports .wav files.
			* GSound fully loads entire audio file into memory. If file is extremely large, consider using GMusic instead.
			* If sound is currently paused, will not attempt to pause playback without returning an error.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval FAILURE Internal Source voice is NULL - (which could happen if specified .wav file could not be found or encountered an error upon reading in data).
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateSound" did not return an error upon filling out this object.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn Pause() = 0;

			//! Attempts to resume a currently paused sound effect.
			/*!
			* Currenntly only supports .wav files.
			* GSound fully loads entire audio file into memory. If file is extremely large, consider using GMusic instead.
			* If sound is currently playing, will not attempt to play file without returning an error.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval FAILURE Internal Source voice is NULL - (which could happen if specified .wav file could not be found or encountered an error upon reading in data).
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateSound" did not return an error upon filling out this object.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn Resume() = 0;

			//! Attempts to stop playback of sound effect and reset it to begining for future use.
			/*!
			* Currenntly only supports .wav files.
			* GSound fully loads entire audio file into memory. If file is extremely large, consider using GMusic instead.
			* Will attempt to comppletely stop playback regaurdless of whether or not sound is playing or paused.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval FAILURE Internal Source voice is NULL - (which could happen if specified .wav file could not be found or encountered an error upon reading in data).
			*	\retval FAILURE If internal object incounters an error. Unlikely to occur if GAudio's "CreateSound" did not return an error upon filling out this object.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn StopSound() = 0;

			//! Stores the .wav files internal amount of channels the sound was recorded with into the passed in unsigned int.
			/*!
			* This value is read and stored upon creation of GSound.
			*
			*	\param [out] The unsigned int which will be stored with the value.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn GetSoundSourceChannels(unsigned int & returnedChannelNum) = 0;

			//! Stores the number of specified outputs into the passed in unsigned int.
			/*!
			* This value is obtained from the GAudio that created this sound effect.
			* By default the number of output channels is 2 unless GAudio was created with a different amount.
			*
			*	\param [out] The unsigned int which will be stored with the value.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*
			*/
			virtual GReturn GetSoundOutputChannels(unsigned int & returnedChannelNum) = 0;

			//! Fills out the passed in bool with information as to if the sound thinks it is playing or not.
			/*!
			* GSound internally keeps track of whether or not it playing via the Play,Pause,Resume,Stop functions as well as if the buffer has finished playing.
			*
			*	\param [out] The bool which will be stored with the value.
			*
			*	\retval FAILURE GAudio has not filled it out this object with GAudio's fuunction: "CreateSound".
			*	\retval SUCCESS Always, boolean passed in will tell you if sound is playing or not.
			*
			*/
			virtual GReturn isSoundPlaying(bool & _returnedBool) = 0;

			//! Return the total number of active references to this object.
			/*!
			*	\param [out] _outCount The total number of active references of this object.
			*
			*	\retval SUCCESS Always, boolean passed in will tell you if sound is playing or not.
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

			virtual ~GSound() = 0;
		}; // end GSound class

	} // end SYSTEM namespace
} // end GW namespace
#endif
