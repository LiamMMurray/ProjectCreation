#ifndef GAUDIO_H
#define GAUDIO_H

/*!
File: GAudio.h
Purpose: A Gateware interface that handles sounds and music.
Author: TJay Cargle
Contributors: N/A
Last Modified: 10/19/2017
Interface Status: Complete
Copyright: 7thGate Software LLC.
License: MIT
*/

#include "GSound.h"
#include "GMusic.h"


//! The core namespace to which all Gateware interfaces/structures/defines must belong.
namespace GW
{
	//! The namespace to which all Gateware library interfaces must belong.
	namespace AUDIO
	{

		//! Ensure identical binary padding for structures on all platforms.
#pragma pack(push, 1)

#pragma pack(pop)



		//! Unique Identifier for this interface. {82DE61C1-C47A-41E5-90BE-C31604DF1140}.
		static const GUUIID GAudioUUIID =
		{

			0x82de61c1, 0xc47a, 0x41e5,{ 0x90, 0xbe, 0xc3, 0x16, 0x4, 0xdf, 0x11, 0x40 }

		};

		class GAudio : public GW::CORE::GMultiThreaded
		{
		private:
		//	virtual GReturn Destroy() = 0;
		public:
			//! Initializes all neccassary data for GAudio based on platform.
			/*!
			*	On Windows, initilaizes XAudio2 pointers IXAudio2 and IXAudio2MasteringVoice.
			*	On Mac, initilaizes AVAudioEngine and starts the engine.
			*
			*	\param [in] _numOfOutputs The number of outputs (i.e. channels or speakers) GAudio should have control over. Default is two. Will be two on Mac and Linux currently. 
			*
			*	\retval FAILURE on if _numOfOutputs greater than 
			*	\retval SUCCESS on Windows if xAudio2 is able to setup correctly.
			*	\retval FAILURE on Windows if it cannot coinitilaize.
			*	\retval FAILURE on Windows if it xAudio2Create fails.
			*	\retval FAILURE on Windows if it xAudio fails to create mastering voice.
			*	\retval SUCCESS on Mac if AVAudioEngine is able to setup correctly.
			*	\retval FAILURE on Mac if AVAudioEngine is able to setup correctly.
			*	\retval SUCCESS on Linux Always.
			*   \retval REDUNDANT_OPERATION tbd
			*/
			virtual GReturn Init(int _numOfOutputs = 2) = 0;

			//! Fills out a GSound with data from provided .wav file.
			/*!
			*	Creates a GSound to return, attempts to initallize internal variables, and loads audio fully into memeory.
			*
			*
			*   \retval FAILURE An error occurred durring loading of file.
			*   \retval FAILURE Internal objects could not be created. (On Windows these are the source voice and submix voice, on other platforms this is all in GSound's init).
			*   \retval FAILURE Failed to initialize internal variables. See Below for Platform
			*	On Linux
			*	pa_main_loop or pa_context could not be created, pa_context could not be connected, pa_channel_map could not be created, pa_stream could not be created, or pa_stream could not be connected.
			*	On Mac
			*	AvAudioPlayerNode could not be created or could not read in file.
			*	\retval SUCCESS None of the above errors occured.
			*/
			virtual GReturn CreateSound(const char* _path, GSound** _outSound) = 0;

			//! Fills out a GMusic with data from provided .wav file.
			/*!
			*	Creates a GMusic to return, attempts to initallize internal variables, and loads header information from .wav file for setup.
			*
			*
			*   \retval FAILURE An error occurred durring loading of header information.
			*   \retval FAILURE Internal objects could not be created. (On Windows these are the source voice and submix voice, on other platforms this is all in GSound's init).
			*   \retval FAILURE Failed to initialize internal variables. See Below for Platform
			*	On Linux
			*	pa_main_loop or pa_context could not be created, pa_context could not be connected, pa_channel_map could not be created, pa_stream could not be created, or pa_stream could not be connected.
			*	On Mac
			*	AvAudioPlayerNode could not be created or AvAudioPCMBuffers could not be initialized
			*	\retval SUCCESS None of the above errors occured.
			*/
			virtual GReturn CreateMusicStream(const char* _path, GMusic** _outMusic) = 0;

			//! Sets Master Volume for all sounds and music.
			/*!
			* After setting this, created sounds and music will check if their current volume are higher than passed in value and will set them to the new master volume if they are above.
			*	\param [in] _value The output master volume to be set.
			*
			*	\retval FAILURE _newVolume is less than Zero.
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*/
			virtual GReturn SetMasterVolume(float _value) = 0;

			//! Sets Master Volume for all sounds and music for specifc speakers.
			/*!
			* The amount of values in _values that will be used is based on _numChannels.
			* After setting this, created sounds and music will check if their current volumes are higher than passed in values and will set them to the new master volumes if they are above.
			* If you attempt to set the volume of an output your hardware does not support, it will be ignored.
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
			*	\retval FAILURE _values is NULL		
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*/
			virtual GReturn SetMasterChannelVolumes(const float * _values, int _numChannels) = 0;

			//! Sets Pauses all sounds and music.
			/*!
			*	Calls each created sound's and music pause function.
			*	\retval FAILURE A sound or Music returned FAILURE	
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*/
			virtual GReturn PauseAll() = 0;

			//!  Resumes all paused/stoped sounds and music.
			/*!
			*	Calls each created sound's and music resume function.
			*	\retval FAILURE A sound or Music returned FAILURE
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*/
			virtual GReturn ResumeAll() = 0;

			//! Stops all paused/playing sounds and music.
			/*!
			*	Calls each created sound's and music stop function.
			*	\retval FAILURE A sound or Music returned FAILURE
			*	\retval SUCCESS Successfully ran without running into any of the above issues.
			*/
			virtual GReturn StopAll() = 0;


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
			
			virtual ~GAudio() = 0;

		}; // end GAudio class
		GATEWARE_EXPORT_IMPLICIT GReturn CreateGAudio(GAudio** _outAudio);

	

	} // end SYSTEM namespace
} // end GW namespace
#endif