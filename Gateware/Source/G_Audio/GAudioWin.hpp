#ifdef _WIN32
	#ifdef _MSC_VER >= 1700
	#pragma comment(lib, "xaudio2.lib")
	#endif
#endif

// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"

#include "../../Interface/G_Audio/GAudio.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <atomic>
#include <vector>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace GW;
using namespace AUDIO;

const unsigned long fourRIFFcc = 'FFIR';
const unsigned long fourDATAcc = 'atad';
const unsigned long fourFMTcc = ' tmf';
const unsigned long fourWAVEcc = 'EVAW';
const unsigned long fourXWMAcc = 'AMWX';
const unsigned long fourDPDScc = 'sdpd';

#define STREAMING_BUFFER_SIZE 65536
#define MAX_BUFFER_COUNT 3
using std::atomic;

HRESULT LoadWaveData(const char * path, WAVEFORMATEXTENSIBLE & myWFX, XAUDIO2_BUFFER & myAudioBuffer)
{
	//WAVEFORMATEXTENSIBLE myWFX = { 0 };
	//XAUDIO2_BUFFER myAudioBuffer = { 0 };
	HRESULT theResult = S_OK;

	wchar_t tpath[4096];
	MultiByteToWideChar(CP_ACP, 0, path, -1, tpath, 4096);
	//if can't find file for unit tests, use : _wgetcwd to see where to put test file 
	HANDLE theFile = CreateFile(tpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == theFile)
		return HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(theFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());


	int result = 0; //zero is good
	unsigned long dwChunktype = 0;
	unsigned long dwChunkDataSize = 0;
	unsigned long dwRiffDataSize = 0;
	unsigned long dwFileType = 0;
	unsigned long bytesRead = 0;

	unsigned long dwIsWave = 0;
	unsigned long throwAwayValue = 0;
	bool foundAudioData = false;
	while (result == 0 && foundAudioData == false)
	{
		unsigned long dwRead;
		ReadFile(theFile, &dwChunktype, 4, &dwRead, NULL);
		if (dwRead != 4)
		{
			result = -1;
			break;
		}
		bytesRead += dwRead;
		ReadFile(theFile, &dwChunkDataSize, 4, &dwRead, NULL);
		if (dwRead != 4)
		{
			result = -2;
			break;
		}
		bytesRead += dwRead;

		switch (dwChunktype)
		{
		case fourRIFFcc:
		{
			dwRiffDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			ReadFile(theFile, &dwFileType, 4, &dwRead, NULL);
			if (dwRead != 4)
			{
				result = -3;
				break;
			}
			if (dwFileType != fourWAVEcc)
			{
				result = -3;
				break;
			}
			bytesRead += dwRead;
			break;
		}
		case fourWAVEcc:
		{
			ReadFile(theFile, &dwIsWave, 4, &dwRead, NULL);
			if (dwRead != 4)
			{
				result = -4;
				break;
			}
			bytesRead += dwRead;

			break;
		}
		case fourFMTcc:
		{
			ReadFile(theFile, &myWFX, dwChunkDataSize, &dwRead, NULL);
			if (dwRead != dwChunkDataSize)
			{
				result = -5;
				break;
			}
			bytesRead += dwRead;

			break;
		}
		case fourDATAcc:
		{
			BYTE * pDataBuffer = new BYTE[dwChunkDataSize];
			ReadFile(theFile, pDataBuffer, dwChunkDataSize, &dwRead, NULL);

			if (dwRead != dwChunkDataSize)
			{
				result = -6;
				break;
			}
			myAudioBuffer.AudioBytes = dwChunkDataSize;  //contains size of the audio buffer in bytes
			myAudioBuffer.pAudioData = pDataBuffer;  // this buffer contains all audio data
			myAudioBuffer.Flags = XAUDIO2_END_OF_STREAM;// tells source this is EOF and should stop
			bytesRead += dwRead;
			foundAudioData = true;
			break;
		}
		default:
		{
			int sizeToRead = sizeof(throwAwayValue);
			int totalChunkData = dwChunkDataSize;
			while (totalChunkData > 0)
			{
				if(sizeToRead > totalChunkData)
					sizeToRead = totalChunkData;

				
				ReadFile(theFile, &throwAwayValue, sizeToRead, &dwRead, NULL);
				if (dwRead != sizeToRead)
				{
					result = -7;
					totalChunkData = 0;
				}

				bytesRead += dwRead;
				totalChunkData -= dwRead;
			}
			
			
			break;
		}

		}



		if (bytesRead - 8 >= dwRiffDataSize)//excludes the first 8 byte header information
		{

			break;
		}
	}
	if (result < 0)
	{
		theResult = S_FALSE;
		return theResult;
	}


	return theResult;
}
HRESULT FindStreamData(HANDLE _file, unsigned long & _outDataChunk, OVERLAPPED & _overLab)
{
	//Assumes that the file is opened.

	HRESULT theResult = S_OK;
	WAVEFORMATEXTENSIBLE myWFX;

	if (INVALID_HANDLE_VALUE == _file)
		return HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(_file, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());


	int result = 0; //zero is good
	unsigned long dwChunktype = 0;
	unsigned long dwChunkDataSize = 0;
	unsigned long dwRiffDataSize = 0;
	unsigned long dwFileType = 0;
	unsigned long bytesRead = 0;

	unsigned long dwIsWave = 0;
	unsigned long throwAwayValue = 0;
	bool foundAudioData = false;

	while (result == 0 && foundAudioData == false)
	{
		unsigned long dwRead;
		ReadFile(_file, &dwChunktype, 4, &dwRead, NULL);
		if (dwRead != 4)
		{
			result = -1;
			break;
		}
		bytesRead += dwRead;

		ReadFile(_file, &dwChunkDataSize, 4, &dwRead, NULL);
		if (dwRead != 4)
		{
			result = -2;
			break;
		}
		bytesRead += dwRead;

			switch (dwChunktype)
			{
			case fourRIFFcc:
			{
				dwRiffDataSize = dwChunkDataSize;
				dwChunkDataSize = 4;
				ReadFile(_file, &dwFileType, 4, &dwRead, NULL);
				if (dwRead != 4)
				{
					result = -3;
					break;
				}
				if (dwFileType != fourWAVEcc)
				{
					result = -3;
					break;
				}
				bytesRead += dwRead;
				break;
			}
			case fourWAVEcc:
			{
				ReadFile(_file, &dwIsWave, 4, &dwRead, NULL);
				if (dwRead != 4)
				{
					result = -4;
					break;
				}
				bytesRead += dwRead;

				break;
			}
			case fourFMTcc:
			{
				ReadFile(_file, &myWFX, dwChunkDataSize, &dwRead, NULL);
				if (dwRead != dwChunkDataSize)
				{
					result = -5;
					break;
				}
				bytesRead += dwRead;

				break;
			}
			case fourDATAcc:
			{
				//Found the Audio data
				_outDataChunk = dwChunkDataSize;		//contains size of the audio buffer in bytes
				_overLab.Offset = bytesRead;			//Sets the overlab to where we are
				foundAudioData = true;					//We found the data now to exit the function
				break;
			}
			default:
			{
				int sizeToRead = sizeof(throwAwayValue);
				int totalChunkData = dwChunkDataSize;
				while (totalChunkData > 0)
				{
					if (sizeToRead > totalChunkData)
						sizeToRead = totalChunkData;


					ReadFile(_file, &throwAwayValue, sizeToRead, &dwRead, NULL);
					if (dwRead != sizeToRead)
					{
						result = -7;
						totalChunkData = 0;
					}

					bytesRead += dwRead;
					totalChunkData -= dwRead;
				}
				break;
			}

			}

		if (bytesRead - 8 >= dwRiffDataSize)//excludes the first 8 byte header information
		{

			break;
		}
	}

	//If there was a result error OR no Audio data was found return S_FALSE
	if (result < 0 || foundAudioData == false)
	{
		theResult = S_FALSE;
	}
	return theResult;
}
HRESULT LoadOnlyWaveHeaderData(const char * path, WAVEFORMATEXTENSIBLE & myWFX, XAUDIO2_BUFFER & myAudioBuffer)
{

	HRESULT theResult = S_OK;
	HANDLE  returnedHandle;
	wchar_t tpath[4096];
	MultiByteToWideChar(CP_ACP, 0, path, -1, tpath, 4096);
	//if can't find file for unit tests, use : _wgetcwd to see where to put test file 

	returnedHandle = CreateFile(tpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (INVALID_HANDLE_VALUE == returnedHandle)
		return HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(returnedHandle, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());


	int result = 0; //zero is good
	unsigned long dwChunktype = 0;
	unsigned long dwChunkDataSize = 0;
	unsigned long dwRiffDataSize = 0;
	unsigned long dwFileType = 0;
	unsigned long bytesRead = 0;

	unsigned long dwIsWave = 0;
	unsigned long throwAwayValue = 0;
	bool foundAudioData = false;
	while (result == 0)
	{
		unsigned long dwRead;
		ReadFile(returnedHandle, &dwChunktype, 4, &dwRead, NULL);
		if (dwRead != 4)
		{
			result = -1;
			break;
		}
		bytesRead += dwRead;
		ReadFile(returnedHandle, &dwChunkDataSize, 4, &dwRead, NULL);
		if (dwRead != 4)
		{
			result = -2;
			break;
		}
		bytesRead += dwRead;

		switch (dwChunktype)
		{
		case fourRIFFcc:
		{
			 dwRiffDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			ReadFile(returnedHandle, &dwFileType, 4, &dwRead, NULL);
			if (dwRead != 4)
			{
				result = -3;
				break;
			}
			if (dwFileType != fourWAVEcc)
			{
				result = -3;
				break;
			}
			bytesRead += dwRead;
			break;
		}
		case fourWAVEcc:
		{
			ReadFile(returnedHandle, &dwIsWave, 4, &dwRead, NULL);
			if (dwRead != 4)
			{
				result = -4;
				break;
			}
			bytesRead += dwRead;

			break;
		}
		case fourFMTcc:
		{
			ReadFile(returnedHandle, &myWFX, dwChunkDataSize, &dwRead, NULL);
			if (dwRead != dwChunkDataSize)
			{
				result = -5;
				break;
			}
			bytesRead += dwRead;
			result = 1; //break us out of loop
			break;
		}


		}


	}
	if (result < 0)
	{
		theResult = S_FALSE;
		return theResult;
	}

	CloseHandle(returnedHandle);
	return theResult;
}
HRESULT FindChunk(HANDLE & theFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition, DWORD & dwRiffSize)
{
	//by default code assumes its reading PCM file with 'RIFF', 'fmt ', and 'data' chunks
	//but in future I hope to include .mp3 if time allows


	//Check if file is valid
	HRESULT theResult = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(theFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	//Setting data variables
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;


	while (theResult == S_OK)
	{
		//populating variables with data from file

		DWORD dwRead;
		// reads in the chunk type 
		if (0 == ReadFile(theFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			theResult = HRESULT_FROM_WIN32(GetLastError());
		bytesRead += dwRead;

		if (0 == ReadFile(theFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			theResult = HRESULT_FROM_WIN32(GetLastError());
		bytesRead += dwRead;

		switch (dwChunkType)
		{
		case fourRIFFcc:
			// RIFF is header and contains info about the rest of the data


			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			dwChunkSize = 4;

			dwRiffSize = dwRIFFDataSize;

			if (0 == ReadFile(theFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				theResult = HRESULT_FROM_WIN32(GetLastError());



			bytesRead += dwRead;

			break;

		default:
			//returns if it encounters a pointer error
			if (INVALID_SET_FILE_POINTER == SetFilePointer(theFile, dwChunkDataSize, NULL, FILE_CURRENT))
			{
				theResult = HRESULT_FROM_WIN32(GetLastError());
				return theResult;
			}
		}

		//updates the offset infromation to read from
		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc && dwChunkType)
		{
			//checks if this is the chunk we were looking for,
			//if so then returns
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (bytesRead >= dwRIFFDataSize)
			return S_FALSE;
	}
	return theResult;
}
HRESULT ReadChunkData(HANDLE & theFile, void * someBuffer, DWORD buffersize, DWORD bufferOffset)
{
	HRESULT theResult = S_OK;
	//Checks if pointer is valid
	DWORD dResult = SetFilePointer(theFile, bufferOffset, NULL, FILE_BEGIN);
	if (dResult <= 0)
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		return theResult;
	}

	DWORD dwRead;
	//attempts to read data into the supplied buffer
	//If done successfully returns S_OK, otherwise returns error
	//theResult = myRead(theFile, someBuffer, buffersize, bufferOffset);
	BOOL theBool = ReadFile(theFile, someBuffer, buffersize, &dwRead, NULL);
	if (0 == (theBool))
		theResult = HRESULT_FROM_WIN32(GetLastError());

	return theResult;
}
int GetCharSize(const char * theConstChar)
{
	int returnValue = 0;
	for (int i = 0; ; i++)
	{
		if (theConstChar[i] == '\0')
		{
			break;
		}
		returnValue++;

	}
	return returnValue;
}
void CreateCharFromConstChar(char ** myChar, const char * theConstChar, int size)
{
	char * testChar = new char[size + 1];
	for (int i = 0; i < size; i++)
	{
		testChar[i] = theConstChar[i];

	}
	testChar[size] = '\0';
	*myChar = testChar;
}

//Forward Declarations so classes can have certain members
class WindowAppAudio;
struct StreamingVoiceContext;


class WindowAppSound : public GSound
{
	atomic<unsigned int> SoundCounter = 0;

public:
	int index = -1;
	std::atomic_bool loops = false;
	std::atomic_bool isPlaying = false;
	std::atomic_bool isPaused = false;
	std::atomic_bool isComplete = false;
	float volume = 1.0f;

	WindowAppAudio * audio;
	IXAudio2SourceVoice * mySourceVoice = nullptr;
	IXAudio2SubmixVoice * mySubmixVoice = nullptr;
	WAVEFORMATEX myWFM = {0};
	XAUDIO2_BUFFER myAudioBuffer = { 0 };
	StreamingVoiceContext * myContext;

	WindowAppSound();
	GReturn Init();
	GReturn SetPCMShader(const char* _data);
	GReturn SetChannelVolumes(float *_values, int _numChannels);
	GReturn CheckChannelVolumes(const float *_values, int _numChannels);
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
	~WindowAppSound();
};

class WindowAppMusic : public GMusic
{
private:
	atomic<unsigned int> MusicCounter = 0;
	BYTE buffers[MAX_BUFFER_COUNT][STREAMING_BUFFER_SIZE];
	OVERLAPPED overlap = { 0 };
public:
	char * myFile;
	int index = -1;
	std::atomic_bool loops = false;
	std::atomic_bool isPlaying = false;
	std::atomic_bool isPaused = false;
	std::atomic_bool stopFlag = false;
	float volume = 1.0f;
	std::mutex mtx;
	std::condition_variable cv;

	WindowAppAudio * audio;
	IXAudio2SourceVoice * mySourceVoice = nullptr;
	IXAudio2SubmixVoice * mySubmixVoice = nullptr;
	WAVEFORMATEX myWFM = {0};
	XAUDIO2_BUFFER myAudioBuffer = { 0 };
	StreamingVoiceContext * myContext;
	std::thread* streamThread = nullptr;

	WindowAppMusic();
	GReturn Init();
	GReturn SetPCMShader(const char* _data);
	GReturn SetChannelVolumes(float *_values, int _numChannels);
	GReturn CheckChannelVolumes(const float *_values, int _numChannels);
	GReturn SetVolume(float _newVolume);
	GReturn StreamStart(bool _loop = false);
	GReturn Stream();
	GReturn PauseStream();
	GReturn ResumeStream();
	GReturn StopStream();
	GReturn GetStreamSourceChannels(unsigned int & returnedChannelNum);
	GReturn GetStreamOutputChannels(unsigned int & returnedChannelNum);
	GReturn isStreamPlaying(bool & _returnedBool);
	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
	~WindowAppMusic();



};


class WindowAppAudio : public GAudio
{
	atomic<unsigned int> AudioCounter = 0;

public:

	std::vector<WindowAppSound *> activeSounds;
	std::vector<WindowAppMusic *> activeMusic;
	IXAudio2 * myAudio = nullptr;
	float maxVolume;
	int maxChannels;
	int numberOfOutputs = 2;
	IXAudio2MasteringVoice * theMasterVoice = nullptr;
	WindowAppAudio();
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
	 ~WindowAppAudio();

};

struct StreamingVoiceContext : public IXAudio2VoiceCallback
{
	WindowAppSound * sndUser = nullptr;
	WindowAppMusic * mscUser = nullptr;
	HANDLE hBufferEndEvent;
	HANDLE hstreamEndEvent;
	StreamingVoiceContext() :
#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
		hBufferEndEvent(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE)), 
		hstreamEndEvent(CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_MODIFY_STATE | SYNCHRONIZE))
#else
		hBufferEndEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr)), hstreamEndEvent(CreateEvent(nullptr, FALSE, FALSE, nullptr))
#endif
	{}
	virtual ~StreamingVoiceContext() { CloseHandle(hBufferEndEvent); CloseHandle(hstreamEndEvent);}
	void STDMETHODCALLTYPE OnBufferEnd(void*)
	{
		if (mscUser != nullptr && mscUser->isPlaying == false && mscUser->mySourceVoice != nullptr)
		{
			mscUser->mySourceVoice->FlushSourceBuffers();
		}
		SetEvent(hBufferEndEvent);
	}
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) {  }
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() {  }
	void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) {  }
	void STDMETHODCALLTYPE OnStreamEnd()
	{
		SetEvent(hstreamEndEvent);
		if (sndUser != nullptr)
		{
			sndUser->isPlaying = false;
			sndUser->isPaused = true;
			// Bug fix, reload the audio buffer so the sound can play again (can't beleive I had to fix this!)
			sndUser->isComplete = true; // this shifts the fix onto the main audio thread which resolves the popping issue
		}

	}
	void STDMETHODCALLTYPE OnBufferStart(void*) { ResetEvent(hBufferEndEvent); } // YAY this fixes the CPU crunching!
	void STDMETHODCALLTYPE OnLoopEnd(void*) {  }
	void STDMETHODCALLTYPE OnLoopEnd(void*, HRESULT) {  }
};
GReturn CreateVoiceContext(StreamingVoiceContext ** outcontext)
{
	GReturn result = FAILURE;
	if (outcontext == nullptr)
	{
		result = INVALID_ARGUMENT;
		return result;
	}
	StreamingVoiceContext* context = new StreamingVoiceContext();

	if (context == nullptr)
	{
		result = FAILURE;
		return result;
	}

	if (result == INVALID_ARGUMENT)
		return result;

	*outcontext = context;

	result = SUCCESS;
	return result;
}

WindowAppSound::WindowAppSound() : SoundCounter(1)
{
}

//Start of GSound implementation 
GReturn WindowAppSound::Init()
{
	GReturn result = GReturn::FAILURE;
	HRESULT theResult = S_OK;
	if (FAILED(theResult = mySourceVoice->SubmitSourceBuffer(&myAudioBuffer)))
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		return result;
	}
	result = SUCCESS;
	return result;
}
GReturn WindowAppSound::SetPCMShader(const char* _data)
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	return result;
}
GReturn WindowAppSound::SetChannelVolumes(float * _values, int _numChannels)
{
	GReturn result = INVALID_ARGUMENT;
	if (_numChannels <= 0)
		return result;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	if (_values == nullptr)
		return result;
	result = FAILURE;
	for (int i = 0; i < _numChannels; i++)
	{
		try
		{
			if (_values[i] > audio->maxVolume)
			{
				_values[i] = audio->maxVolume;
			}
		}
		catch (const std::exception& e)
		{
			return result;
		}

	}
	unsigned int sourceChannels = 0;
	GetSoundSourceChannels(sourceChannels);
	HRESULT theResult = S_OK;


	// can only support up to 6 outputs

	float  matrix[12] = {0};
	int trueIndex = 0;
	for (size_t i = 0; i < 12;)
	{
		if (trueIndex < _numChannels)
		{
			matrix[i] = _values[trueIndex];
			matrix[i + 1] = _values[trueIndex];
			trueIndex++;
			i+=2;
		}
		else
		{
			matrix[i] = 0;
			i++;
		}
	}
	if (FAILED(theResult = mySourceVoice->SetOutputMatrix(mySubmixVoice, sourceChannels, audio->numberOfOutputs, matrix)))
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		
		return result;
	}


	result = SUCCESS;
	return result;
}
GReturn WindowAppSound::CheckChannelVolumes(const float * _values, int _numChannels)
{
	GReturn result = GReturn::FAILURE;
	if (_numChannels <= 0)
		return result;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	if (_values == nullptr)
		return result;

	uint32_t currentChannels;
	result = GetSoundSourceChannels(currentChannels);
	if (result != SUCCESS)
		return result;
	float * currentValues = new float[12];
	
	mySourceVoice->GetChannelVolumes(_numChannels, currentValues);
	if (currentValues == nullptr)
		return result;

	bool didChange = false;
	for (int i = 0; i < _numChannels; i++)
	{
		try
		{
			if (currentValues[i] > _values[i])
			{
				currentValues[i] = _values[i];
				didChange = true;
			}
		}
		catch (const std::exception& e)
		{
			return result;
		}

	}

	delete[] currentValues;

	HRESULT theResult = S_OK;
	if (didChange == true)
	{
		if (FAILED(theResult = mySourceVoice->SetChannelVolumes(_numChannels, _values)))
		{
			theResult = HRESULT_FROM_WIN32(GetLastError());
			return result;
		}
	}
	result = SUCCESS;
	return result;
}
GReturn WindowAppSound::GetSoundSourceChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == NULL)
	{
		return result;
	}
	returnedChannelNum = myWFM.nChannels;
	result = SUCCESS;
	return result;
}
 GReturn WindowAppSound::GetSoundOutputChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == NULL)
	{
		return result;
	}
	returnedChannelNum = audio->numberOfOutputs;
	result = SUCCESS;
	return result;
}
GReturn WindowAppSound::SetVolume(float _newVolume)
{
	GReturn result = INVALID_ARGUMENT;
	if (_newVolume < 0.0f)
		return result;
	result = SUCCESS;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;


	if (_newVolume > audio->maxVolume)
	{
		_newVolume = audio->maxVolume;
	}
	HRESULT theResult = S_OK;
	if (FAILED(theResult = mySourceVoice->SetVolume(_newVolume)))
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		return result;
	}
	result = SUCCESS;

	return result;
}
GReturn WindowAppSound::Play()
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	HRESULT theResult = S_OK;

	if (isPlaying || isComplete)
	{
		result = StopSound();
		if (result != SUCCESS)
			return result;
	}
	if (!isPlaying)
	{
		if (FAILED(theResult = mySourceVoice->Start()))
		{
			theResult = HRESULT_FROM_WIN32(GetLastError());
			return result;
		}
		isPlaying = true;
		isPaused = false;
	}
	result = SUCCESS;
	return result;
}
GReturn WindowAppSound::Pause()
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	HRESULT theResult = S_OK;
	if (!isPaused)
	{
		if (FAILED(theResult = mySourceVoice->Stop()))
		{
			theResult = HRESULT_FROM_WIN32(GetLastError());
			return result;
		}
		isPlaying = false;
		isPaused = true;
	}
	result = SUCCESS;
	return result;

}
GReturn WindowAppSound::Resume()
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	HRESULT theResult = S_OK;
	if (!isPlaying)
	{
		if (FAILED(theResult = mySourceVoice->Start()))
		{
			theResult = HRESULT_FROM_WIN32(GetLastError());
			return result;
		}
		isPlaying = true;
		isPaused = false;
	}
	result = SUCCESS;
	return result;
}
GReturn WindowAppSound::StopSound()
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	HRESULT theResult = S_OK;

	if (FAILED(theResult = mySourceVoice->Stop()))
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		return result;
	}
	mySourceVoice->FlushSourceBuffers();

	if (FAILED(theResult = mySourceVoice->SubmitSourceBuffer(&myAudioBuffer)))
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		return result;
	}
	WaitForSingleObject(myContext->hBufferEndEvent, INFINITE);
	isPlaying = false;
	isPaused = true;
	isComplete = false;

	result = SUCCESS;
	return result;
}
 GReturn WindowAppSound::isSoundPlaying(bool & _returnedBool)
{
	_returnedBool = isPlaying;
	return SUCCESS;
}
 GReturn WindowAppSound::GetCount(unsigned int & _outCount)
{
	 GReturn result = FAILURE;
	 _outCount = SoundCounter;
	 result = SUCCESS;

	 return result;
}
 GReturn WindowAppSound::IncrementCount()
{
	 GReturn result = FAILURE;
	 //Results in Failure if increment would overflow
	 if (SoundCounter == UINT_MAX)
		 return result;
	 SoundCounter++;
	 result = SUCCESS;
	 return result;
}
 GReturn WindowAppSound::DecrementCount()
{
	 GReturn result = FAILURE;
	 //Results in Failure if decrement would underflow
	 if (SoundCounter == 0)
		 return result;
	 SoundCounter--;
	 //Here do not need to call "delete this" when the SoundCounter is 0
	 //because in GAudio destructor will do that.
	 result = SUCCESS;
	 return result;
}
 GReturn WindowAppSound::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
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
WindowAppSound::~WindowAppSound()
{
	if (isPlaying)
	{
		StopSound();
	}
	HRESULT theResult;
	theResult = mySourceVoice->FlushSourceBuffers();
	mySubmixVoice->DestroyVoice();
	mySourceVoice->DestroyVoice();
	delete myAudioBuffer.pAudioData;
	myAudioBuffer.pAudioData = nullptr;
	delete myContext;
	myContext = nullptr;
	audio = nullptr;

}
//End of GSound implementation

WindowAppMusic::WindowAppMusic() : MusicCounter(1)
{
}

//Start of GMusic implementation
GReturn WindowAppMusic::Init()
{
	GReturn result = GReturn::FAILURE;
	overlap.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	result = SUCCESS;
	return result;
}
GReturn WindowAppMusic::SetPCMShader(const char* _data)
{
	GReturn result = FAILURE;

	return result;
}
GReturn WindowAppMusic::SetChannelVolumes(float * _values, int _numChannels)
{
	GReturn result = INVALID_ARGUMENT;
	if (_numChannels <= 0)
		return result;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	if (_values == nullptr)
		return result;
	result = FAILURE;
	for (int i = 0; i < _numChannels; i++)
	{
		try
		{
			if (_values[i] > audio->maxVolume)
			{
				_values[i] = audio->maxVolume;
			}
		}
		catch (const std::exception& e)
		{
			return result;
		}

	}
	unsigned int sourceChannels = 0;
	GetStreamSourceChannels(sourceChannels);
	HRESULT theResult = S_OK;


	// can only support up to 6 outputs
	float  matrix[12] = { 0 };
	int trueIndex = 0;
	for (size_t i = 0; i < 12;)
	{
		if (trueIndex < _numChannels)
		{
			matrix[i] = _values[trueIndex];
			matrix[i + 1] = _values[trueIndex];
			trueIndex++;
			i += 2;
		}
		else
		{
			matrix[i] = 0;
			i++;
		}
	}
	if (FAILED(theResult = mySourceVoice->SetOutputMatrix(mySubmixVoice, sourceChannels, audio->numberOfOutputs, matrix)))
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
	
		return result;
	}


	result = SUCCESS;
	return result;
}
GReturn WindowAppMusic::CheckChannelVolumes(const float * _values, int _numChannels)
{
	GReturn result = GReturn::INVALID_ARGUMENT;
	if (_numChannels <= 0)
		return result;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	if (_values == nullptr)
		return result;

	result = FAILURE;
	uint32_t currentChannels;
	result = GetStreamSourceChannels(currentChannels);
	if (result != SUCCESS)
		return result;
	float * currentValues = new float[12];

	mySourceVoice->GetChannelVolumes(_numChannels, currentValues);

	if (currentValues == nullptr)
		return result;

	bool didChange = false;
	for (int i = 0; i < _numChannels; i++)
	{
		try
		{
			if (currentValues[i] > _values[i])
			{
				currentValues[i] = _values[i];
				didChange = true;
			}
		}
		catch (const std::exception& e)
		{
			return result;
		}

	}
	delete[] currentValues;
	HRESULT theResult = S_OK;
	if (didChange == true)
	{
		if (FAILED(theResult = mySourceVoice->SetChannelVolumes(_numChannels, _values)))
		{
			theResult = HRESULT_FROM_WIN32(GetLastError());
			return result;
		}
	}
	result = SUCCESS;
	return result;
}
GReturn WindowAppMusic::GetStreamSourceChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == NULL)
	{
		return result;
	}
	result = INVALID_ARGUMENT;
	
	returnedChannelNum = myWFM.nChannels;
	result = SUCCESS;
	return result;
}
 GReturn WindowAppMusic::GetStreamOutputChannels(unsigned int & returnedChannelNum)
{
	 GReturn result = FAILURE;
	 if (audio == NULL)
	 {
		 return result;
	 }
	 returnedChannelNum = audio->numberOfOutputs;
	 result = SUCCESS;
	 return result;
}
GReturn WindowAppMusic::SetVolume(float _newVolume)
{
	GReturn result = FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	result = INVALID_ARGUMENT;
	if (_newVolume < 0.0f)
		return result;
	result = FAILURE;
	if (_newVolume > audio->maxVolume)
	{
		_newVolume = audio->maxVolume;
	}
	HRESULT theResult = S_OK;
	if (FAILED(theResult = mySourceVoice->SetVolume(_newVolume)))
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		return result;
	}
	volume = _newVolume;
	result = SUCCESS;

	return result;
}
GReturn WindowAppMusic::Stream()
{
	HRESULT theResult = S_OK;
	DWORD errorCheck = 0;
	wchar_t tpath[4096];
	MultiByteToWideChar(CP_ACP, 0, myFile, -1, tpath, 4096);
	//if can't find file for unit tests, use : _wgetcwd to see where to put test file 

	HANDLE theFile =  CreateFile(tpath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);


	if (INVALID_HANDLE_VALUE == theFile)
		theResult = HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(theFile, 0, NULL, FILE_BEGIN))
		theResult = HRESULT_FROM_WIN32(GetLastError());
	
	errorCheck = GetLastError();

	if (errorCheck != NOERROR)
		theResult = HRESULT_FROM_WIN32(GetLastError());


	//DWORD ptrPosition = SetFilePointer(theFile, 0, NULL, FILE_CURRENT);
		
	int CurrentDiskReadBuffer = 0;
	DWORD CurrentPosition = 0;
	DWORD cbWaveSize = 0; //GetFileSize(theFile, 0);

	
	FindStreamData(theFile, cbWaveSize, overlap);

	//TESTING (POPPING DOESN'T GO AWAY)
	//SetFilePointer(theFile, STREAMING_BUFFER_SIZE, NULL, FILE_CURRENT);

	//TESTING (POPPING GOES AWAY)
	//DWORD tst = 0;
	//ReadFile(theFile, buffers[CurrentDiskReadBuffer], STREAMING_BUFFER_SIZE, &tst, &overlap);
	//overlap.Offset += 44;//min(STREAMING_BUFFER_SIZE, cbWaveSize - CurrentPosition);

	while (CurrentPosition < cbWaveSize && stopFlag == false)
	{
		if (!isPaused)
		{
			DWORD dwRead;
			DWORD cbValid = min(STREAMING_BUFFER_SIZE, cbWaveSize - CurrentPosition);

			if (0 == ReadFile(theFile, buffers[CurrentDiskReadBuffer], STREAMING_BUFFER_SIZE, &dwRead, &overlap))
				theResult = HRESULT_FROM_WIN32(GetLastError());
			overlap.Offset += cbValid;

			//update the file position to where it will be once the read finishes
			CurrentPosition += cbValid;

			DWORD NumberBytesTransfered;
			GetOverlappedResult(theFile, &overlap, &NumberBytesTransfered, true);
			XAUDIO2_VOICE_STATE state;

			while (mySourceVoice->GetState(&state), state.BuffersQueued >= MAX_BUFFER_COUNT - 1)
			{
				WaitForSingleObject(myContext->hBufferEndEvent, INFINITE);
			}

			/*
			FOR FUTURE AUDIO DEVELOPER

			PCM FUNTION POINTER GOES HERE

			If I had time I would do something like the following:

			for(int i = 0; i < cbValid; i++)
			{
			dataFunction(buffers[CurrentDiskReadBuffer][i]);
			}
			*/

			
				XAUDIO2_BUFFER buf = { 0 };
				buf.AudioBytes = cbValid;
				buf.pAudioData = buffers[CurrentDiskReadBuffer];

			if (CurrentPosition >= cbWaveSize && !loops)
			{
				buf.Flags = XAUDIO2_END_OF_STREAM;
			
			}
			else if (CurrentPosition >= cbWaveSize && loops)
			{
				mySourceVoice->SubmitSourceBuffer(&buf);
				CurrentDiskReadBuffer++;
				CurrentDiskReadBuffer %= MAX_BUFFER_COUNT;
				if (INVALID_SET_FILE_POINTER == SetFilePointer(theFile, 0, NULL, FILE_BEGIN))
					theResult = HRESULT_FROM_WIN32(GetLastError());
				CurrentPosition = 0;
				overlap.Offset = 0;
				continue;
			}
		
			mySourceVoice->SubmitSourceBuffer(&buf);
			CurrentDiskReadBuffer++;
			CurrentDiskReadBuffer %= MAX_BUFFER_COUNT;

		}
	}
	XAUDIO2_VOICE_STATE state;
	mySourceVoice->GetState(&state);

	//Waits for last buffers to finish playing
	while (state.BuffersQueued > 0)
	{
		mySourceVoice->GetState(&state);
		WaitForSingleObjectEx(myContext->hBufferEndEvent, INFINITE, TRUE);
	}

	//Stops the voice from producing more sound
	if (FAILED(theResult = mySourceVoice->Stop()))
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		return FAILURE;
	}

	//This needs to be reset so xaudio2 doesn't remeber where it left off playing
	overlap.Offset = 0;
	
	//Closes the Handle since we are no longer using it
	CloseHandle(theFile);
	//Updates information about playback state
	isPlaying = false;
	isPaused = true;
}
GReturn WindowAppMusic::StreamStart(bool _loop)
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;

	HRESULT theResult = S_OK;
	loops = _loop;
	if (!isPlaying)
	{
		stopFlag = false;
		if (FAILED(theResult = mySourceVoice->Start(0, 0)))
		{
			theResult = HRESULT_FROM_WIN32(GetLastError());
			return result;
		}
		isPlaying = true;
		isPaused = false;

		streamThread = new std::thread(&WindowAppMusic::Stream, this);


	}
	result = SUCCESS;
	return result;
}
GReturn WindowAppMusic::PauseStream()
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	HRESULT theResult = S_OK;
	if (!isPaused)
	{
		if (FAILED(theResult = mySourceVoice->Stop()))
		{
			theResult = HRESULT_FROM_WIN32(GetLastError());
			return result;
		}
		isPlaying = false;
		isPaused = true;
	}
	result = SUCCESS;
	return result;
}
GReturn WindowAppMusic::ResumeStream()
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;
	HRESULT theResult = S_OK;
	if (!isPlaying)
	{
		if (FAILED(theResult = mySourceVoice->Start()))
		{
			theResult = HRESULT_FROM_WIN32(GetLastError());
			return result;
		}
		isPlaying = true;
		isPaused = false;
	}

	result = SUCCESS;
	return result;
}
GReturn WindowAppMusic::StopStream()
{
	GReturn result = GReturn::FAILURE;
	if (audio == NULL)
		return result;
	if (mySourceVoice == NULL)
		return result;

	HRESULT theResult = S_OK;
	stopFlag = true;


	if (isPaused == true)
	{
		isPlaying = false;
		theResult = mySourceVoice->FlushSourceBuffers();
	}
	//For simplicity, if the stream has not been created then stopping should not cause a fatal error

	if (streamThread != nullptr)
	{
		streamThread->join();
		delete streamThread;
	}
    streamThread = nullptr;
	isPlaying = false;
	isPaused = true;
	mySourceVoice->FlushSourceBuffers();
	result = SUCCESS;
	return result;
}
 GReturn WindowAppMusic::isStreamPlaying(bool & _returnedBool)
{
	 _returnedBool = isPlaying;
	 return SUCCESS;
}
 GReturn WindowAppMusic::GetCount(unsigned int & _outCount)
{
	 GReturn result = FAILURE;
	 _outCount = MusicCounter;
	 result = SUCCESS;

	 return result;
}
 GReturn WindowAppMusic::IncrementCount()
{
	 GReturn result = FAILURE;
	 //Results in Failure if increment would overflow
	 if (MusicCounter == UINT_MAX)
		 return result;
	 MusicCounter++;
	 result = SUCCESS;
	 return result;
}
 GReturn WindowAppMusic::DecrementCount()
{
	 GReturn result = FAILURE;
	 //Results in Failure if decrement would underflow
	 if (MusicCounter == 0)
		 return result;
	 MusicCounter--;
	 //Here do not need to call "delete this" when the MusicCounter is 0
	 //because in GAudio destructor will do that.
	 result = SUCCESS;
	 return result;
}
 GReturn WindowAppMusic::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
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
WindowAppMusic::~WindowAppMusic()
{
	if (isPlaying)
	{
		StopStream();
	}
	HRESULT theResult;
	theResult = mySourceVoice->FlushSourceBuffers();
	mySubmixVoice->DestroyVoice();
	mySourceVoice->DestroyVoice();
	delete myAudioBuffer.pAudioData;
	myAudioBuffer.pAudioData = nullptr;
	delete myFile;
	myFile = nullptr;
	delete myContext;
	myContext = nullptr;
	audio = nullptr;

}
WindowAppAudio::WindowAppAudio() : AudioCounter(1)
{
}
//End of GMusic implementation 
//Start of GAudio implementation for Windows
GReturn WindowAppAudio::Init(int _numOfOutputs)
{
	GReturn result = FAILURE;
	HRESULT theResult = CoInitialize(NULL);

	if(_numOfOutputs > 8)
		return result;
	if (FAILED(theResult = XAudio2Create(&myAudio)))
	{
		return result;
	}
	if (FAILED(theResult = myAudio->CreateMasteringVoice(&theMasterVoice)))
	{
		return result;
	}
	numberOfOutputs = _numOfOutputs;
	result = SUCCESS;

	return result;
}
GReturn WindowAppAudio::CreateSound(const char* _path, GSound** _outSound)
{

	GReturn result = FAILURE;
	HRESULT theResult = S_OK;
	if (_outSound == nullptr)
	{
		result = INVALID_ARGUMENT;
		return result;
	}
	WindowAppSound* snd = new WindowAppSound();

	if (snd == nullptr)
	{
		result = FAILURE;
		return result;
	}
	WAVEFORMATEXTENSIBLE wfmx;
	if (LoadWaveData(_path, wfmx, snd->myAudioBuffer) != S_OK)
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		result = FAILURE;
		return result;
	}
	if (wfmx.Format.nChannels > maxChannels)
		maxChannels = wfmx.Format.nChannels;
	snd->myWFM = wfmx.Format;
	if (myAudio->CreateSubmixVoice(&snd->mySubmixVoice, numberOfOutputs, wfmx.Format.nSamplesPerSec) != S_OK)
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		result = FAILURE;
		return result;
	}

	result = CreateVoiceContext(&snd->myContext);
	if (result != SUCCESS)
	{
		return result;
	}
	XAUDIO2_SEND_DESCRIPTOR sndSendDcsp = { 0, snd->mySubmixVoice };
	XAUDIO2_VOICE_SENDS sndSendList = { 1, &sndSendDcsp };
	if (myAudio->CreateSourceVoice(&snd->mySourceVoice, &snd->myWFM, 0, XAUDIO2_DEFAULT_FREQ_RATIO, snd->myContext, &sndSendList) != S_OK)
	{
		theResult= HRESULT_FROM_WIN32(GetLastError());
		result = FAILURE;
		return result;
	}
	snd->myContext->sndUser = snd;
	result = snd->Init();
	activeSounds.push_back(snd);
	snd->audio = this;
	*_outSound = snd;
	if (result == INVALID_ARGUMENT)
	{
		return result;
	}

	return result;
}
GReturn WindowAppAudio::CreateMusicStream(const char* _path, GMusic** _outMusic)
{
	GReturn result = FAILURE;
	HRESULT theResult = S_OK;
	if (_outMusic == nullptr)
	{
		result = INVALID_ARGUMENT;
		return result;
	}
	WindowAppMusic* msc = new WindowAppMusic();

	if (msc == nullptr)
	{
		result = FAILURE;
		return result;
	}
	WAVEFORMATEXTENSIBLE wfmx;
	int size = GetCharSize(_path);
	CreateCharFromConstChar(&msc->myFile, _path, size);
	if (LoadOnlyWaveHeaderData(msc->myFile, wfmx, msc->myAudioBuffer) != S_OK)
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		result = FAILURE;
		return result;
	}
	if (wfmx.Format.nChannels > maxChannels)
		maxChannels = wfmx.Format.nChannels;
	msc->myWFM = wfmx.Format;
	
	if (myAudio->CreateSubmixVoice(&msc->mySubmixVoice, numberOfOutputs ,wfmx.Format.nSamplesPerSec ) != S_OK)
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		result = FAILURE;
		return result;
	}
	result = CreateVoiceContext(&msc->myContext);
	if (result != SUCCESS)
	{
		return result;
	}
	XAUDIO2_SEND_DESCRIPTOR mscSendDcsp = { 0, msc->mySubmixVoice };
	XAUDIO2_VOICE_SENDS mscSendList= { 1, &mscSendDcsp };
	if (theResult = myAudio->CreateSourceVoice(&msc->mySourceVoice, &msc->myWFM,0, XAUDIO2_DEFAULT_FREQ_RATIO, msc->myContext, &mscSendList) != S_OK)
	{
		theResult = HRESULT_FROM_WIN32(GetLastError());
		result = FAILURE;
		return result;
	}
	msc->myContext->mscUser = msc;
	result = msc->Init();
	if (result != SUCCESS)
	{
		return result;
	}
	msc->audio = this;

	activeMusic.push_back(msc);
	*_outMusic = msc;
	if (result == INVALID_ARGUMENT)
		return result;

	return result;
}
GReturn WindowAppAudio::SetMasterVolume(float _value)
{
	GReturn result = INVALID_ARGUMENT;
	if (_value < 0.0)
	{
		return result;
	}
	result = SUCCESS;
	if (_value > 1.0)
	{
		maxVolume = 1.0;
	}
	else
	{
		maxVolume = _value;

	}
	return result;
}
GReturn WindowAppAudio::SetMasterChannelVolumes(const float * _values, int _numChannels)
{

	GReturn result = INVALID_ARGUMENT;
	if (_values == nullptr)
		return result;
	if (_numChannels < 0)
		return result;
	result = FAILURE;
	uint32_t theirChannels;
	for (int i = 0; i < activeSounds.size(); i++)
	{
		result = activeSounds[i]->GetSoundSourceChannels(theirChannels);
		if (result != SUCCESS)
		{
			return result;
		}
		result = activeSounds[i]->CheckChannelVolumes(_values, theirChannels);
		if (result != SUCCESS)
		{
			return result;
		}
	}
	for (int i = 0; i < activeMusic.size(); i++)
	{
		result = activeMusic[i]->GetStreamSourceChannels(theirChannels);
		if (result != SUCCESS)
		{
			return result;
		}
		result = activeMusic[i]->CheckChannelVolumes(_values, _numChannels);
		if (result != SUCCESS)
		{
			return result;
		}
	}
	return result;
}
GReturn WindowAppAudio::PauseAll()
{
	GReturn result = FAILURE;
	for (int i = 0; i < activeSounds.size(); i++)
	{
		result = activeSounds[i]->Pause();
		if (result != SUCCESS)
		{
			return result;
		}
	}
	for (int i = 0; i < activeMusic.size(); i++)
	{
		result = activeMusic[i]->PauseStream();
		if (result != SUCCESS)
		{
			return result;
		}
	}
	return result;
}
GReturn WindowAppAudio::StopAll()
{
	GReturn result = FAILURE;
	for (int i = 0; i < activeSounds.size(); i++)
	{
		result = activeSounds[i]->StopSound();
		if (result != SUCCESS)
		{
			return result;
		}
	}
	for (int i = 0; i < activeMusic.size(); i++)
	{
		result = activeMusic[i]->StopStream();
		if (result != SUCCESS)
		{
			return result;
		}
	}
	return result;
}
GReturn WindowAppAudio::ResumeAll()
{
	GReturn result = FAILURE;
	for (int i = 0; i < activeSounds.size(); i++)
	{
		result = activeSounds[i]->Resume();
		if (result != SUCCESS)
		{
			return result;
		}
	}
	for (int i = 0; i < activeMusic.size(); i++)
	{
		result = activeMusic[i]->ResumeStream();
		if (result != SUCCESS)
		{
			return result;
		}
	}
	return result;
}
GReturn WindowAppAudio::GetCount(unsigned int & _outCount)
{
	GReturn result = FAILURE;
	_outCount = AudioCounter;
	result = SUCCESS;

	return result;
}
GReturn WindowAppAudio::IncrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if increment would overflow
	if (AudioCounter == UINT_MAX)
		return result;
	AudioCounter++;
	result = SUCCESS;
	return result;
}
GReturn WindowAppAudio::DecrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if decrement would underflow
	if (AudioCounter == 0)
		return result;
	AudioCounter--;
	if (AudioCounter == 0)
		delete this;
	result = SUCCESS;
	return result;
}
GReturn WindowAppAudio::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
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
WindowAppAudio::~WindowAppAudio()
{
	while( activeSounds.size()>0)
	{
		delete activeSounds[0];
		activeSounds[0] = nullptr;
		activeSounds.erase(activeSounds.begin());
	}
	while (activeMusic.size()>0)
	{
		delete activeMusic[0];
		activeMusic[0] = nullptr;
		activeMusic.erase(activeMusic.begin());
	}

	theMasterVoice->DestroyVoice();
	myAudio->StopEngine();
	myAudio->Release();

}

GReturn PlatformGetAudio(GAudio ** _outAudio)
{
	GReturn result = FAILURE;
	if (_outAudio == nullptr)
	{
		result = INVALID_ARGUMENT;
		return result;
	}
	WindowAppAudio* audio = new WindowAppAudio();

	if (audio == nullptr)
	{
		result = FAILURE;
		return result;
	}
	audio->Init();

	if (result == INVALID_ARGUMENT)
		return result;

	//initalize Gaudio's maxVolumn, The user can change this using the SetMasterVolumn() func.
	audio->maxVolume = 1.0f;

	*_outAudio = audio;

	result = SUCCESS;
	return result;
}
//End of GMusic implementation for Windows 
