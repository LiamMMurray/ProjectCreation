
// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"

#include "../../Interface/G_Audio/GAudio.h"
#ifdef __OBJC__
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#endif
#include <thread>
#include <atomic>
#include <mutex>
using namespace GW;
using namespace AUDIO;
#define G_UINT_MAX 0xffffffff


class MacAppAudio;
class MacAppSound : public GSound
{
    std::atomic<unsigned int> SoundCounter;
    
public:
	int index = -1;
	MacAppAudio * audio;
	bool loops = false;

    MacAppSound();
	GReturn Init(const char * _path);
	GReturn SetPCMShader(const char* _data);
	GReturn SetChannelVolumes(float *_values, int _numChannels);
	GReturn SetVolume(float _newVolume);
	GReturn Play();
	GReturn Pause();
	GReturn Resume();
	GReturn StopSound();
	GReturn isSoundPlaying(bool & _returnedBool);
	GReturn GetSoundSourceChannels(unsigned int & returnedChannelNum);
	GReturn GetSoundOutputChannels(unsigned int & returnedChannelNum);
	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
	~MacAppSound();
#if __APPLE__
	GMacSound * mac_snd = nullptr;
#endif
};

class MacAppMusic : public GMusic
{
private:
    std::atomic<unsigned int> MusicCounter;
    GReturn Stream();
public:
	char * myFile;
	int index = -1;
	unsigned long dataSize;
	bool loops = false;
	MacAppAudio * audio;
	
	std::thread* streamThread = nullptr;

	GReturn Init(const char * _path);
	GReturn SetPCMShader(const char* _data);
	GReturn SetChannelVolumes(float *_values, int _numChannels);
	GReturn SetVolume(float _newVolume);
	GReturn StreamStart(bool _loop = false);
	GReturn PauseStream();
	GReturn ResumeStream();
	GReturn StopStream();
	GReturn isStreamPlaying(bool & _returnedBool);
	GReturn GetStreamSourceChannels(unsigned int & returnedChannelNum);
	GReturn GetStreamOutputChannels(unsigned int & returnedChannelNum);
	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
    MacAppMusic();
	~MacAppMusic();

#if __APPLE__
	GMacMusic * mac_msc = nullptr;
#endif

};


class MacAppAudio : public GAudio
{
    std::atomic<unsigned int> AudioCounter;
public:
	const char * myFile;
	bool loops = false;
	float maxVolume;
	int maxChannels;
	
    MacAppAudio();
	GReturn Init(int _numOfOutputs = 2);
	GReturn CreateSound(const char* _path, GSound** _outSound);
	GReturn CreateMusicStream(const char* _path, GMusic** _outMusic);
	GReturn SetMasterVolume(float _value);
	GReturn SetMasterChannelVolumes(const float * _values, int _numChannels);
	GReturn PauseAll();
	GReturn ResumeAll();
	GReturn StopAll();
	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
	~MacAppAudio();

#if __APPLE__
	GMacAudio * mac_audio = nullptr;
#endif
};
//Start of GSound implementation
MacAppSound::MacAppSound() : SoundCounter(1)
{
}

GReturn MacAppSound::Init(const char * _path)
{
	GReturn result = GReturn::FAILURE;
#if __APPLE__
    
    NSString * nsPath = [[[NSString alloc] initWithUTF8String:_path] autorelease];
    mac_snd = [[GMacSound alloc]initWithPath:nsPath];
    
    if (mac_snd != nullptr)
    {
        result = SUCCESS;
    }
    else
    {
        result = FAILURE;
    }
#endif
	return result;
}
GReturn MacAppSound::SetPCMShader(const char* _data)
{
	GReturn result = GReturn::FAILURE;
	if (audio == nullptr)
		return result;

	return result;
}
GReturn MacAppSound::SetChannelVolumes(float * _values, int _numChannels)
{
	GReturn result = GReturn::INVALID_ARGUMENT;
	if (_numChannels <= 0)
		return result;

    if (_values == nullptr)
        return result;
    result = FAILURE;
    if (audio == nullptr)
        return result;

#if __APPLE__
    bool bresult = [mac_snd SetChannelVolumes:_values theNumberOfChannels:_numChannels];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
#endif
	return result;
}
GReturn MacAppSound::GetSoundSourceChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == nullptr)
	{
		return result;
	}
#if __APPLE__
    returnedChannelNum = [mac_snd GetChannels];
    if (returnedChannelNum > 0)
        result = SUCCESS;
    else
        result = FAILURE;
#endif
	return result;
}
GReturn MacAppSound::GetSoundOutputChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == nullptr)
	{
		return result;
	}
#if __APPLE__
	returnedChannelNum = [mac_snd GetChannels];
	if (returnedChannelNum > 0)
		result = SUCCESS;
	else
		result = FAILURE;
#endif
	return result;
}
GReturn MacAppSound::SetVolume(float _newVolume)
{
	GReturn result = INVALID_ARGUMENT;
	if (_newVolume < 0.0f)
		return result;
	result = SUCCESS;
	if (audio == nullptr)
		return result;
#if __APPLE__
	bool bresult = [mac_snd SetVolume : _newVolume];
	if (bresult == true)
		result = SUCCESS;
	else
		result = FAILURE;
#endif
	return result;
}
GReturn MacAppSound::Play()
{
	GReturn result = GReturn::FAILURE;
	if (audio == nullptr)
		return result;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_snd PlaySound];
	if (bresult == true)
		result = SUCCESS;
	else
		result = FAILURE;

#endif
	return result;
}
GReturn MacAppSound::Pause()
{
	GReturn result = GReturn::FAILURE;
	if (audio == nullptr)
		return result;
#if __APPLE__
	bool bresult = [mac_snd PauseSound];
	if (bresult == true)
		result = SUCCESS;
	else
		result = FAILURE;

#endif
	return result;

}
GReturn MacAppSound::Resume()
{
	GReturn result = GReturn::FAILURE;
	if (audio == nullptr)
		return result;
#if __APPLE__
	bool bresult = [mac_snd ResumeSound];
	if (bresult == true)
		result = SUCCESS;
	else
		result = FAILURE;

#endif
	return result;
}
GReturn MacAppSound::StopSound()
{
	GReturn result = GReturn::FAILURE;
	if (audio == nullptr)
		return result;
#if __APPLE__
	bool bresult = [mac_snd StopSound];
	if (bresult == true)
		result = SUCCESS;
	else
		result = FAILURE;
#endif

	return result;
}
GReturn MacAppSound::isSoundPlaying(bool & _returnedBool)
{
	GReturn result = GReturn::FAILURE;
#if __APPLE__
	_returnedBool = mac_snd->isPlaying;
	result = SUCCESS;
#endif
	return 	result;
}
GReturn MacAppSound::GetCount(unsigned int & _outCount)
{
	GReturn result = FAILURE;
	_outCount = SoundCounter;
	result = SUCCESS;

	return result;
}
GReturn MacAppSound::IncrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if increment would overflow
	if (SoundCounter == G_UINT_MAX)
		return result;
	SoundCounter++;
	result = SUCCESS;
	return result;
}
GReturn MacAppSound::DecrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if decrement would underflow
	if (SoundCounter == 0)
		return result;
	SoundCounter--;
	result = SUCCESS;
	return result;
}
GReturn MacAppSound::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
{
	GReturn result = FAILURE;
	if (_outputInterface == nullptr)
		return GW::INVALID_ARGUMENT;

	//If passed in interface is equivalent to current interface (this).
	if (_interfaceID == GW::AUDIO::GSoundUUIID)
	{
		//Temporary GFile* to ensure proper functions are called.
		GSound* convert = reinterpret_cast<GSound*>(this);

		//Increment the count of the GFile.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//If requested interface is multithreaded.
	else if (_interfaceID == GW::CORE::GMultiThreadedUUIID)
	{
		//Temporary GMultiThreaded* to ensure proper functions are called.
		GW::CORE::GMultiThreaded* convert = reinterpret_cast<GW::CORE::GMultiThreaded*>(this);

		//Increment the count of the GMultithreaded.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//If requested interface is the primary interface.
	else if (_interfaceID == GW::CORE::GInterfaceUUIID)
	{
		//Temporary GInterface* to ensure proper functions are called.
		GW::CORE::GInterface* convert = reinterpret_cast<GW::CORE::GInterface*>(this);

		//Increment the count of the GInterface.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//Interface is not supported.
	else
		return 	result = INTERFACE_UNSUPPORTED;

	return result;
}
MacAppSound::~MacAppSound()
{


}
//End of GSound implementation

//Start of GMusic implementation
MacAppMusic::MacAppMusic() : MusicCounter(1)
{
}
GReturn MacAppMusic::Init(const char * _path)
{
	GReturn result = GReturn::FAILURE;
#if __APPLE__

	NSString * nsPath = [[[NSString alloc] initWithUTF8String:_path] autorelease];
	mac_msc = [[GMacMusic alloc]initWithPath:nsPath];
	
	if (mac_msc != nullptr)
	{
		result = SUCCESS;
	}
	else
	{
		result = FAILURE;
	}
#endif
	return result;
}
GReturn MacAppMusic::SetPCMShader(const char* _data)
{
	GReturn result = FAILURE;

	return result;
}
GReturn MacAppMusic::SetChannelVolumes(float * _values, int _numChannels)
{
    GReturn result = GReturn::INVALID_ARGUMENT;
    if (_numChannels <= 0)
        return result;
    
    if (_values == nullptr)
        return result;
    result = FAILURE;
    if (audio == nullptr)
        return result;
    
#if __APPLE__
    bool bresult = [mac_msc SetChannelVolumes:_values theNumberOfChannels:_numChannels];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
#endif
    return result;
}
GReturn MacAppMusic::GetStreamSourceChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == nullptr)
	{
		return result;
	}
#if __APPLE__
	returnedChannelNum = [mac_msc GetChannels];
	if (returnedChannelNum > 0)
		result = SUCCESS;
	else
		result = FAILURE;
#endif
	return result;
}
GReturn MacAppMusic::GetStreamOutputChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == nullptr)
	{
		return result;
	}
#if __APPLE__
    returnedChannelNum = [mac_msc GetChannels];
    if (returnedChannelNum > 0)
        result = SUCCESS;
    else
        result = FAILURE;
#endif
	return result;
}
GReturn MacAppMusic::SetVolume(float _newVolume)
{
	GReturn result = INVALID_ARGUMENT;
    if(_newVolume < 0)
        return result;
    result = FAILURE;
	if (audio == nullptr)
		return result;
    
#if __APPLE__
    bool bresult = false;
    bresult = [mac_msc SetVolume:_newVolume];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
	return result;
}
GReturn MacAppMusic::Stream()
{
    GReturn result = SUCCESS;
#if __APPLE__
[mac_msc StreamMusic];
#endif
    return result;
}
GReturn MacAppMusic::StreamStart(bool _loop)
{
	GReturn result = FAILURE;
    if (audio == nullptr)
        return result;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_msc StreamStart:_loop];
    
    if (bresult == true)
    {
        result = SUCCESS;
        streamThread = new std::thread(&MacAppMusic::Stream, this);
    }
    else
    {
        result = FAILURE;
    }
#endif
	return result;
}
GReturn MacAppMusic::PauseStream()
{
	GReturn result = GReturn::FAILURE;
	if (audio == nullptr)
		return result;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_msc PauseStream];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
	return result;
}
GReturn MacAppMusic::ResumeStream()
{
	GReturn result = GReturn::FAILURE;
	if (audio == nullptr)
		return result;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_msc ResumeStream];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
	return result;
}
GReturn MacAppMusic::StopStream()
{
	GReturn result = GReturn::FAILURE;
	if (audio == nullptr)
		return result;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_msc StopStream];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
    streamThread->join();
    delete streamThread;
    streamThread = nullptr;
	return result;
}
GReturn MacAppMusic::isStreamPlaying(bool & _returnedBool)
{
	GReturn result = GReturn::FAILURE;

#if __APPLE__
	_returnedBool = mac_msc->isPlaying;
	result = SUCCESS;
#endif
	return 	result;
}
GReturn MacAppMusic::GetCount(unsigned int & _outCount)
{
	GReturn result = FAILURE;
	_outCount = MusicCounter;
	result = SUCCESS;

	return result;
}
GReturn MacAppMusic::IncrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if increment would overflow
	if (MusicCounter == G_UINT_MAX)
		return result;
	MusicCounter++;
	result = SUCCESS;
	return result;
}
GReturn MacAppMusic::DecrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if decrement would underflow
	if (MusicCounter == 0)
		return result;
	MusicCounter--;
	result = SUCCESS;
	return result;
}
GReturn MacAppMusic::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
{
	GReturn result = FAILURE;
	if (_outputInterface == nullptr)
		return GW::INVALID_ARGUMENT;

	//If passed in interface is equivalent to current interface (this).
	if (_interfaceID == GW::AUDIO::GMusicUUIID)
	{
		//Temporary GFile* to ensure proper functions are called.
		GMusic* convert = reinterpret_cast<GMusic*>(this);

		//Increment the count of the GFile.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//If requested interface is multithreaded.
	else if (_interfaceID == GW::CORE::GMultiThreadedUUIID)
	{
		//Temporary GMultiThreaded* to ensure proper functions are called.
		GW::CORE::GMultiThreaded* convert = reinterpret_cast<GW::CORE::GMultiThreaded*>(this);

		//Increment the count of the GMultithreaded.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//If requested interface is the primary interface.
	else if (_interfaceID == GW::CORE::GInterfaceUUIID)
	{
		//Temporary GInterface* to ensure proper functions are called.
		GW::CORE::GInterface* convert = reinterpret_cast<GW::CORE::GInterface*>(this);

		//Increment the count of the GInterface.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//Interface is not supported.
	else
		return 	result = INTERFACE_UNSUPPORTED;

	return result;
}
MacAppMusic::~MacAppMusic()
{
    DecrementCount();
}
//End of GMusic implementation
MacAppAudio::MacAppAudio(): AudioCounter(1)
{
}

GReturn MacAppAudio::Init(int _numOfOutputs)
{
	GReturn result = FAILURE;
	//number of outputs currently only working and tested on Windows and is not used on Mac
	result = SUCCESS;
#if __APPLE__
    mac_audio = [GMacAudio alloc];
    [mac_audio Init];
    [mac_audio autorelease];
#endif
	return result;
}
GReturn MacAppAudio::CreateSound(const char* _path, GSound** _outSound)
{

	GReturn result = FAILURE;
	if (_outSound == nullptr)
	{
		result = INVALID_ARGUMENT;
		return result;
	}
	MacAppSound* snd = new MacAppSound();

	if (snd == nullptr)
	{
		result = FAILURE;
		return result;
	}
	
	result = snd->Init(_path);
#if __APPLE__
    snd->mac_snd->myAudio = mac_audio->myAudio;
    [mac_audio->myAudio attachNode:snd->mac_snd->mySound];
    [mac_audio->myAudio connect:snd->mac_snd->mySound to:mac_audio->myAudio.mainMixerNode format:snd->mac_snd->myBuffer.format];
    [mac_audio->ActiveSounds addObject:snd->mac_snd];
    
#endif
    //	activeSounds.push_back(snd);
	snd->audio = this;
	*_outSound = snd;
	if (result == INVALID_ARGUMENT)
	{
        
		return result;
	}

	return result;
}
GReturn MacAppAudio::CreateMusicStream(const char* _path, GMusic** _outMusic)
{
	GReturn result = FAILURE;
	
	if (_outMusic == nullptr)
	{
		result = INVALID_ARGUMENT;
		return result;
	}
	MacAppMusic* msc = new MacAppMusic();

	if (msc == nullptr)
	{
		result = FAILURE;
		return result;
	}


	result = msc->Init(_path);
	if (result != SUCCESS)
	{
		return result;
	}
#if __APPLE__
    msc->mac_msc->myAudio = mac_audio->myAudio;
    [mac_audio->myAudio attachNode:msc->mac_msc->mySound];
    [mac_audio->myAudio connect:msc->mac_msc->mySound to:mac_audio->myAudio.mainMixerNode format:msc->mac_msc->myBuffers[0].format];
    [mac_audio->ActiveMusic addObject:msc->mac_msc];
#endif
	msc->audio = this;

	
	*_outMusic = msc;
	if (result == INVALID_ARGUMENT)
		return result;

	return result;
}
GReturn MacAppAudio::SetMasterVolume(float _value)
{
	GReturn result = INVALID_ARGUMENT;
	if (_value < 0.0)
	{
		return result;
	}
	
	if (_value > 1.0)
	{
		maxVolume = 1.0;
	}
#if __APPLE__
    bool bresult = false;
    bresult = [mac_audio SetMasterVolume:_value];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
	return result;
}
GReturn MacAppAudio::SetMasterChannelVolumes(const float * _values, int _numChannels)
{

	GReturn result = INVALID_ARGUMENT;
	if (_values == nullptr)
		return result;
	if (_numChannels < 1)
		return result;
	result = FAILURE;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_audio SetMasterVolumeChannels:_values theNumberOfChannels:_numChannels];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
	return result;
}
GReturn MacAppAudio::PauseAll()
{
	GReturn result = FAILURE;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_audio PauseAll];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
	return result;
}
GReturn MacAppAudio::StopAll()
{
	GReturn result = FAILURE;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_audio StopAll];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
	return result;
}
GReturn MacAppAudio::ResumeAll()
{
	GReturn result = FAILURE;
#if __APPLE__
    bool bresult = false;
    bresult = [mac_audio ResumeAll];
    if (bresult == true)
        result = SUCCESS;
    else
        result = FAILURE;
    
#endif
	return result;
}
GReturn MacAppAudio::GetCount(unsigned int & _outCount)
{
	GReturn result = FAILURE;
	_outCount = AudioCounter;
	result = SUCCESS;

	return result;
}
GReturn MacAppAudio::IncrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if increment would overflow
	if (AudioCounter == G_UINT_MAX)
		return result;
	AudioCounter++;
	result = SUCCESS;
	return result;
}
GReturn MacAppAudio::DecrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if decrement would underflow
	if (AudioCounter == 0)
		return result;
	AudioCounter--;
	result = SUCCESS;
	return result;
}
GReturn MacAppAudio::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
{
	GReturn result = FAILURE;
	if (_outputInterface == nullptr)
		return GW::INVALID_ARGUMENT;

	//If passed in interface is equivalent to current interface (this).
	if (_interfaceID == GW::AUDIO::GAudioUUIID)
	{
		//Temporary GFile* to ensure proper functions are called.
		GAudio* convert = reinterpret_cast<GAudio*>(this);

		//Increment the count of the GFile.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//If requested interface is multithreaded.
	else if (_interfaceID == GW::CORE::GMultiThreadedUUIID)
	{
		//Temporary GMultiThreaded* to ensure proper functions are called.
		GW::CORE::GMultiThreaded* convert = reinterpret_cast<GW::CORE::GMultiThreaded*>(this);

		//Increment the count of the GMultithreaded.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//If requested interface is the primary interface.
	else if (_interfaceID == GW::CORE::GInterfaceUUIID)
	{
		//Temporary GInterface* to ensure proper functions are called.
		GW::CORE::GInterface* convert = reinterpret_cast<GW::CORE::GInterface*>(this);

		//Increment the count of the GInterface.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
		result = SUCCESS;
	}
	//Interface is not supported.
	else
		return 	result = INTERFACE_UNSUPPORTED;

	return result;
}
MacAppAudio::~MacAppAudio()
{

}


//Start of GAudio implementation for Macs
GReturn PlatformGetAudio(GAudio ** _outAudio)
{
	GReturn result = FAILURE;
	if (_outAudio == nullptr)
	{
		result = INVALID_ARGUMENT;
		return result;
	}
	MacAppAudio* audio = new MacAppAudio();

	if (audio == nullptr)
	{
		result = FAILURE;
		return result;
	}
	audio->Init();

	if (result == INVALID_ARGUMENT)
		return result;

	//Initalize Gaudio's maxVolumn
	audio->maxVolume = 1.0f;

	*_outAudio = audio;
	result = SUCCESS;
	return result;
}


