// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"

#include "../../Interface/G_Audio/GAudio.h"
/*
PA_CHANNEL_POSITION_MONO
PA_CHANNEL_POSITION_FRONT_LEFT

Apple, Dolby call this 'Left'.
PA_CHANNEL_POSITION_FRONT_RIGHT

Apple, Dolby call this 'Right'.
PA_CHANNEL_POSITION_FRONT_CENTER

Apple, Dolby call this 'Center'.
PA_CHANNEL_POSITION_REAR_CENTER

Microsoft calls this 'Back Center', Apple calls this 'Center Surround', Dolby calls this 'Surround Rear Center'.
PA_CHANNEL_POSITION_REAR_LEFT

Microsoft calls this 'Back Left', Apple calls this 'Left Surround' (!), Dolby calls this 'Surround Rear Left'.
PA_CHANNEL_POSITION_REAR_RIGHT
*/
using namespace GW;
using namespace AUDIO;
#include <iostream>
#include <time.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <unistd.h>
#include <stdio.h>
#include <functional>
#include <string.h>
#include <pulse/mainloop.h>
#include <pulse/mainloop-api.h>
#include <pulse/channelmap.h>
#include <pulse/context.h>
#include <pulse/volume.h>
#include <pulse/stream.h>
#include <pulse/error.h>
#include <pulse/scache.h>
#include <pulse/introspect.h>

const unsigned long fourRIFFcc = 'FFIR';
const unsigned long fourDATAcc = 'atad';
const unsigned long fourFMTcc = ' tmf';
const unsigned long fourWAVEcc = 'EVAW';
const unsigned long fourXWMAcc = 'AMWX';
const unsigned long fourDPDScc = 'sdpd';
using std::atomic;
#define STREAMING_BUFFER_SIZE 65536
#define MAX_BUFFER_COUNT 3

struct PCM_FORMAT_INFO
{
    unsigned short mFormatTag = 0;
    unsigned short mNumChannels = 0;
    unsigned int mSamples = 0;
    unsigned int mAvgBytesPerSec = 0;
    unsigned short mBlockAlign= 0;
    unsigned short mBitsPerSample= 0;
    unsigned short mCbSize= 0;
};
struct PCM_BUFFER
{

    uint32_t byteSize = 0;
    uint8_t * bytes = nullptr;
};
struct WAVE_FILE
{
    PCM_FORMAT_INFO myFormat ;
    PCM_BUFFER myBuffer;
    bool isSigned = false;
};
enum TJState
{
    Wait = 0,
     Prepare,
     Poll,
    Dispatch
};
struct TJCALLBACK
{
int didFinish = -1;
void(*streamOperationSucceed)(pa_stream*,int,void*);
void(*contextOperationSucceed)(pa_context*,int,void*);
pa_stream_success_cb_t cbSucceed;
pa_context_success_cb_t cbContextSucceed;
pa_operation * myOperation = nullptr;
};
void FinishedContextGeneral(pa_context *c, int success, void *userdata)
{
TJCALLBACK * theCallback = reinterpret_cast<TJCALLBACK*>(userdata);
theCallback->didFinish =1;

}
void FinishedDrain(pa_stream *s, int success, void *userdata)
{
TJCALLBACK * theCallback = reinterpret_cast<TJCALLBACK*>(userdata);
theCallback->didFinish =1;

}
int LoadWavFormatOnly(const char * path, PCM_FORMAT_INFO & returnedInfo, long & _fileSize)
{
int result = 0; //zero is good
unsigned long dwChunktype = 0;
unsigned long dwChunkDataSize = 0;
unsigned long dwRiffDataSize = 0;
unsigned long dwFileType = 0;
unsigned long bytesRead = 0;
unsigned long dwOffset = 0;
unsigned long throwAway;

FILE * someWaveFile = NULL;
someWaveFile = fopen(path, "r");
   if(someWaveFile == NULL)
   {
       std::cout << "\n\n someWaveFile == NULL \n";
       return 1;
   }
    if(someWaveFile != NULL)
    {
        while(result == 0)
        {
        unsigned long dwRead = 0;
        dwRead = fread(&dwChunktype,1,4,someWaveFile);
            if(dwRead!= 4)
            {
                result = -1;
                std::cout << "\n\n dwRead != 4 \n";
                break;
            }
            bytesRead += dwRead;

             dwRead = fread(&dwChunkDataSize,1,4,someWaveFile);
            if(dwRead!= 4)
            {
               result = -1;
               std::cout << "\n\n dwRead2 != 4 \n";
                break;
            }
            bytesRead += dwRead;

            switch(dwChunktype)
            {
                case fourRIFFcc:
                {
                dwRiffDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;
                _fileSize = dwRiffDataSize;
                dwRead = fread(&dwFileType,1,4,someWaveFile);
                    if(dwRead!= 4)
                    {
                        result = -1;
                         std::cout << "\n\n dwRead3 != 4 \n";
                        break;
                    }
                bytesRead += dwRead;
                 break;
                }
                case fourWAVEcc:
                {

                    dwRead = fread(&throwAway,1,4,someWaveFile);
                    if(dwRead!= 4)
                    {
                        result = -1;
                         std::cout << "\n\n dwRead4 != 4 \n";
                        break;
                    }
                    bytesRead += dwRead;

                    break;
                }
                case fourFMTcc:
                {
                   dwRead = fread(&returnedInfo,1, dwChunkDataSize,someWaveFile);
                  if(dwRead!= dwChunkDataSize)
                    {
                        result = -1;
                         std::cout << "\n\n dwRead != dwChunkDataSize \n";
                        break;
                    }
                    bytesRead += dwRead;
                   result = 1; // break out of loop
                break;
                }


        }

    }
  fclose(someWaveFile);
if(result > 0)
    result = 0;
return result;
    }
}
int LoadWav(const char * path, WAVE_FILE & returnedWave)
{

int result = 0; //zero is good
unsigned long dwChunktype = 0;
unsigned long dwChunkDataSize = 0;
unsigned long dwRiffDataSize = 0;
unsigned long dwFileType = 0;
unsigned long bytesRead = 0;
unsigned long dwOffset = 0;
unsigned long dwIsWave = 0;
unsigned long throwAwayValue = 0;

FILE * someWaveFile = NULL;
someWaveFile = fopen(path, "r");
   if(someWaveFile == NULL)
   {
       return 1;
   }
    if(someWaveFile != NULL)
    {
        while(result == 0)
        {
        unsigned long dwRead = 0;
        dwRead = fread(&dwChunktype,1,4,someWaveFile);
            if(dwRead!= 4)
            {
                //result = -1;
                break;
            }
            bytesRead += dwRead;

             dwRead = fread(&dwChunkDataSize,1,4,someWaveFile);
            if(dwRead!= 4)
            {
               //result = -1;
                break;
            }
            bytesRead += dwRead;

            switch(dwChunktype)
            {
                case fourRIFFcc:
                {
                dwRiffDataSize = dwChunkDataSize;
                dwChunkDataSize = 4;

                dwRead = fread(&dwFileType,1,4,someWaveFile);
                    if(dwRead!= 4)
                    {
                        result = -1;
                        break;
                    }
                bytesRead += dwRead;
                break;
                }
                case fourWAVEcc:
                {

                    dwRead = fread(&dwIsWave,1,4,someWaveFile);
                    if(dwRead!= 4)
                    {
                        result = -1;
                        break;
                    }
                    bytesRead += dwRead;

                    break;
                }
                case fourFMTcc:
                {
                   dwRead = fread(&returnedWave.myFormat,1, dwChunkDataSize,someWaveFile);
                  if(dwRead!= dwChunkDataSize)
                    {
                        result = -1;
                        break;
                    }
                    bytesRead += dwRead;

                break;
                }
                case fourDATAcc:
                {
                returnedWave.myBuffer.bytes = new uint8_t[dwChunkDataSize];
                   dwRead = fread(returnedWave.myBuffer.bytes,1,dwChunkDataSize,someWaveFile);

                     returnedWave.myBuffer.byteSize = dwChunkDataSize;
                  if(dwRead!= dwChunkDataSize)
                    {
                    result = -1;
                    break;
                    }
                bytesRead += dwRead;
                result = 1; // break us out of loop once we've found data

                break;
                }
                default:
                {
                int sizeToRead = sizeof(throwAwayValue);
                int totalChunkData = dwChunkDataSize;

                while(totalChunkData > 0)
                {
                    if(sizeToRead > totalChunkData)
                        sizeToRead = totalChunkData;

                 dwRead = fread(&throwAwayValue,1,sizeToRead,someWaveFile);
                  if(dwRead!= sizeToRead)
                    {
                    result = -1;
                    totalChunkData = 0;
                    }
                    bytesRead += dwRead;
                    totalChunkData -= dwRead;
                }
                break;
                }

            }

            dwOffset +=8;
            dwOffset += dwChunkDataSize;

            if(bytesRead - 8 >= dwRiffDataSize)//excludes first 8 bytes
            {
            result = -2;
            }
        }
        fclose(someWaveFile);
    }
result = 0;
if(returnedWave.myFormat.mBitsPerSample == 8)
{
returnedWave.isSigned = false;
}
else
{
returnedWave.isSigned = true;
}
return result;

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

void OnStateChange(pa_context *_c, void* _data)
{
    pa_context_state_t state;
    std::atomic<int> *pa_ready = (std::atomic<int>*)_data;

   // std::cout << "\n\n IN ONSTATECHANGE FUNCTION \n\n";
    state = pa_context_get_state(_c);

       switch(state)
       {
       case PA_CONTEXT_UNCONNECTED:
        {
          //  std::cout << "\n\n PA_CONTEXT_UNCONNECTED \n\n";
            break;
        }
        case PA_CONTEXT_CONNECTING:
        {
          //  std::cout << "\n\n PA_CONTEXT_CONNECTING \n\n";
            break;
        }
        case PA_CONTEXT_AUTHORIZING:
        {
          //  std::cout << "\n\n PA_CONTEXT_AUTHORIZING \n\n";
            break;
        }
        case PA_CONTEXT_SETTING_NAME:
        {
           // std::cout << "\n\n PA_CONTEXT_SETTING_NAME \n\n";
            break;
        }
        case PA_CONTEXT_FAILED:
        {
          //  std::cout << "\n\n PA_CONTEXT_FAILED \n\n";
            *pa_ready = 2;
            break;
        }
        case PA_CONTEXT_TERMINATED:
        {
          //  std::cout << "\n\n PA_CONTEXT_TERMINATED \n\n";
            *pa_ready = 2;
            break;
        }
        case PA_CONTEXT_READY:
        {
          //  std::cout << "\n\n PA_CONTEXT_READY \n\n";
            *pa_ready = 1;
            break;
        }
       }
}

class LinuxAppAudio;
class LinuxAppSound : public GSound
{
        atomic<unsigned int> SoundCounter;
public:
	char * streamName = "Sound";
    int index = -1;
	int quitVal = 0;
	atomic<bool> isPlaying;
	atomic<bool> isPaused;
	float volume = -1.0f;
	bool stopFlag = false;
	TJCALLBACK theCallback;
    atomic<int> pa_ready;

	LinuxAppAudio * audio = nullptr;
	WAVE_FILE myFile;
	atomic<TJState> myState;
	std::thread* streamThread = nullptr;
	std::thread* loopThread = nullptr;

    pa_channel_map * myMap = nullptr;
    pa_stream * myStream = nullptr;
    pa_mainloop * myMainLoop;
    pa_context * myContext = nullptr;
    pa_cvolume vol;
    pa_sample_format myPulseFormat;

    LinuxAppSound();
    GReturn Init();
    GReturn SetPCMShader(const char* _data);
    GReturn SetChannelVolumes(float *_values, int _numChannels);
    GReturn CheckChannelVolumes(const float *_values, int _numChannels);
    GReturn SetVolume(float _newVolume);

    GReturn Play();
    GReturn Pause();
    GReturn Resume();
    GReturn StopSound();
    GReturn StreamSound();
	GReturn isSoundPlaying(bool & _returnedBool);
	GReturn GetSoundSourceChannels(unsigned int & returnedChannelNum);
	GReturn GetSoundOutputChannels(unsigned int & returnedChannelNum);
	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
    ~LinuxAppSound();

};

class LinuxAppMusic : public GMusic
{
    atomic<unsigned int> MusicCounter;
public:
	char * streamName = "Sound";

    char * myFilePath;
	 long fileSize = 0;
	int quitVal = 0;
	bool loops = false;
	atomic<bool> isPlaying;
	atomic<bool> isPaused;
	float volume = -1.0f;
	bool stopFlag = false;
	TJCALLBACK theCallback;
    atomic<int> pa_ready;

     atomic<TJState> myState;
    LinuxAppAudio * audio = nullptr;
    std::thread* loopThread = nullptr;
    std::thread* streamThread = nullptr;
	PCM_FORMAT_INFO myPCMFormat;
	PCM_BUFFER myBuffers[MAX_BUFFER_COUNT];

	pa_channel_map * myMap = nullptr;
	pa_stream * myStream = nullptr;
    pa_sample_format myPulseFormat;
    pa_mainloop * myMainLoop;
    pa_context * myContext = nullptr;
    pa_cvolume vol;


    LinuxAppMusic();
    GReturn Init();
    GReturn SetPCMShader(const char* _data);
    GReturn SetChannelVolumes(float *_values, int _numChannels);
    GReturn CheckChannelVolumes(const float *_values, int _numChannels);

    GReturn SetVolume(float _newVolume);
    GReturn StreamStart(bool _loop = false);
    GReturn StreamMusic();
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
    ~LinuxAppMusic();



};


class LinuxAppAudio : public GAudio
{
    atomic<unsigned int> AudioCounter;
public:
	float maxVolume = 1;
	int maxChannels = 0;
	int quitVal = 0;
	int numberOfOutputs = 2;

	atomic<TJState> myState;
	atomic<bool> isIterating;
    std::vector<LinuxAppSound *> activeSounds;
    std::vector<LinuxAppMusic *> activeMusic;
    std::thread* streamThread = nullptr;

    LinuxAppAudio();
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
    ~LinuxAppAudio();

};
void RunMainLoop(pa_mainloop * myMainLoop)
{
    if(myMainLoop == nullptr)
        return;

    TJState myState = TJState::Wait;

    int result = 0;
    while(myMainLoop != nullptr)
    {

        {
            switch(myState)
            {
        case TJState::Wait:

            myState = TJState::Prepare;
             usleep(100);
                break;

            case TJState::Prepare:
                if(result >=0)
                {
                  result = pa_mainloop_prepare(myMainLoop, 0);
                    if(result >=0)
                    {
                         myState = TJState::Poll;
                            usleep(100);

                    }

                }

                break;

            case TJState::Poll:
                if(result >= 0)
                {
            result = pa_mainloop_poll(myMainLoop);
              if(result >=0)
              {
               myState = TJState::Dispatch;
                  usleep(100);
              }

                }

                break;

            case TJState::Dispatch:
            if(result >= 0)
            {
            result =  pa_mainloop_dispatch(myMainLoop);
               if(result >=0)
               {


             myState = TJState::Wait;
                usleep(100);
               }

            }

                break;
            }

        }
    }

}
static bool WaitForConnectionEstablished(pa_mainloop * mainLoop, pa_context * aContext, time_t timeOut)
{

//Currently not being used
//Currently we are using a callback function from pusleAudio to determine when the context's state changes.
time_t timeLimit = time(NULL) + timeOut;
//std::cout << "\n\n IN WAITFORCONNECTION FUNCTION \n\n";



    while(timeLimit >= time (NULL))
    {
       // sleep(1);
        //std::cout << "\n\n" << " Current Context: " << pa_context_get_state(aContext) << "\n\n";
       // std::this_thread::yield();


        if(PA_CONTEXT_READY == pa_context_get_state(aContext))
        {
        return true;
        }

    }

   // std::cout << "\n\n DONE WAITING \n\n";
    return false;
}
GReturn createMainLoopAndContext(pa_mainloop ** myMainLoop, pa_context ** myContext)
{
    GReturn result = FAILURE;
    *myMainLoop = nullptr;
    *myContext = nullptr;

    pa_mainloop * newLoop = pa_mainloop_new();

    if(NULL == newLoop)
    {
        return result;
    }

    *myMainLoop = newLoop;
    pa_context * newContext = pa_context_new(pa_mainloop_get_api(newLoop),"StartAudio");
   *myContext = newContext;

    if(NULL == newContext)
    {
        return result;
    }
    result = SUCCESS;



  return result;
}


//Start of GSound implementation
LinuxAppSound::LinuxAppSound() : SoundCounter(1){}
GReturn LinuxAppSound::Init()
{
    GReturn result = GReturn::FAILURE;
   // std::cout << "\n IN SOUND INIT()  \n\n";

    if(audio == nullptr)
    {
      //  std::cout << "\n AUDIO == NULLPTR \n";
        return result;
    }

     result = REDUNDANT_OPERATION;
    if(myStream != nullptr)
    {
      //  std::cout << "\n MYSTREAM == NULLPTR \n";
        return result;
    }
    if(myMap != nullptr)
    {
      //  std::cout << "\n MYMAP == NULLPTR \n";
        return result;
    }

    result = createMainLoopAndContext(&myMainLoop,&myContext);
    if(result != SUCCESS)
    {
      //  std::cout << "\n RESULT != SUCCESS \n";
        return result;
    }

    result = FAILURE;
    if(myMainLoop == nullptr)
    {
      //  std::cout << "\n MYMAINLOOP == NULLPTR \n";
        return result;
    }
    if(myContext == nullptr)
    {
      //  std::cout << "\n MYCONTEXT == NULLPTR \n";
        return result;
    }

    //This thread iterates the mainloop.
   // std::cout << "\n\n BEFORE THREAD \n\n";
   // loopThread = new std::thread(RunMainLoop, myMainLoop);
   // std::cout << "\n\n PASSED THROUGH THREAD \n\n";
    pa_ready = 0;
  //  std::cout << "\n\n BEFORE PA_CONTEXT_CONNECT \n\n";
    pa_context_connect(myContext, NULL, (pa_context_flags_t)0,NULL);
  //  std::cout << "\n\n PASSED THROUGH PA_CONTEXT_CONNECT \n\n";

    pa_context_set_state_callback(myContext, OnStateChange, &pa_ready);

    //Had to move this thread creation below the pa_context_connect()
    //so it starts searching for the contexts state at the start of the mainloop thread loop.
    //Instead of somewhere random within it.
    loopThread = new std::thread(RunMainLoop, myMainLoop);
  //  bool connected = WaitForConnectionEstablished(myMainLoop, myContext, 120);
  while(pa_ready == 0)
   {
      // pa_mainloop_iterate(myMainLoop, 1, NULL);
      std::this_thread::yield();

   }
    if(pa_ready == 1)
    {
        result = SUCCESS;
    }
    else
    {
       // std::cout << "\n NOT CONNECTED \n";
        return result;
    }
     result = FAILURE;
    switch(myFile.myFormat.mBitsPerSample)
    {
    case 8:
    myPulseFormat = PA_SAMPLE_U8;
        break;
    case 16:
    myPulseFormat = PA_SAMPLE_S16LE;
        break;
    default:
    myPulseFormat = PA_SAMPLE_S16LE;
        break;
    }
    int rate = myFile.myFormat.mSamples;
    int Channels = myFile.myFormat.mNumChannels;

    pa_sample_spec mySampleSpec = {myPulseFormat,rate,Channels};
    int paCheck = 0;
    paCheck = pa_channels_valid(mySampleSpec.channels);
    myMap = new pa_channel_map();
    myMap = pa_channel_map_init_extend(myMap, mySampleSpec.channels,PA_CHANNEL_MAP_DEFAULT);
    if(myMap == nullptr)
    {
       // std::cout << "\n MYMAP == NULLPTR 2 \n";
        return result;
    }

    myStream = pa_stream_new(myContext,"Sound",&mySampleSpec,nullptr);
    if(myStream == nullptr)
    {
       // std::cout << "\n MYSTREAM == NULLPTR \n";
        return result;
    }

    int pcheck = pa_stream_connect_playback(myStream,NULL,NULL,(pa_stream_flags_t)0,NULL,NULL);

    if(pa_stream_get_context(myStream) != myContext)
    {
       //  std::cout << "\n CONTEXT MISMATCH \n";
        return result;
    }
    isPlaying = false;
    result = SUCCESS;
    return result;
}
GReturn LinuxAppSound::SetPCMShader(const char* _data)
{
    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;

    return result;
}
GReturn LinuxAppSound::SetChannelVolumes(float * _values, int _numChannels)
{
//Sets the current volume channels to the values passed in.
//_values = volume values from 0 -> 1.0
//_numChannels = the size of _values data by index
//Loops from index 0 -> index _numChannels

     GReturn result = INVALID_ARGUMENT;

    if (_numChannels <= 0)
        return result;
    if (audio == nullptr)
        return result;
    if (_values == nullptr)
        return result;

    result = FAILURE;

    vol.channels = _numChannels;

    for (int i = 0; i < _numChannels; i++)
    {
        //Make sure the passed in values are not over the maxVolume if yes, then set it to the maxVolume.
            if (_values[i] > audio->maxVolume)
            {
                _values[i] = audio->maxVolume;
            }

        //Convert a linear factor to a volume value.
            vol.values[i] =   pa_sw_volume_from_linear(_values[i]);
    }

    //TJCALLBACK theCallback;
    theCallback.contextOperationSucceed = FinishedContextGeneral;
    theCallback.cbContextSucceed = theCallback.contextOperationSucceed;

    //Returns the sink resp. source output index this stream is identified in the server with
    uint32_t index = pa_stream_get_index(myStream);

    //Set the volume of a sink input stream. And catch the returned operation in theCallback struct to let the rest of the system know whats going on
    theCallback.myOperation = pa_context_set_sink_input_volume(myContext, index, &vol, theCallback.cbContextSucceed, &theCallback);

    pa_operation_unref(theCallback.myOperation);

    result = SUCCESS;
    return result;




    /*
    pa_volume_t * newValues = new pa_volume_t[_numChannels];

    for (int i = 0; i < _numChannels; i++)
    {
         newValues[i] = 0;
    }
    pa_cvolume theVolume;
    theVolume.channels = _numChannels;
    result = FAILURE;

    for (int i = 0; i < _numChannels; i++)
    {
            if (_values[i] > audio->maxVolume)
            {
                _values[i] = audio->maxVolume;
            }
            newValues[i] =   pa_sw_volume_from_linear(_values[i]);
            theVolume.values[i] = newValues[i];
    }

    //TJCALLBACK theCallback;
    theCallback.contextOperationSucceed = FinishedContextGeneral;
    theCallback.cbContextSucceed = theCallback.contextOperationSucceed;

     uint32_t index2 = pa_stream_get_index(myStream);
     pa_context_set_sink_input_volume(myContext,index2,&theVolume, theCallback.cbContextSucceed, &theCallback);

   // delete newValues;
    delete[] newValues;

    result = SUCCESS;
    return result;
  */
}
GReturn LinuxAppSound::CheckChannelVolumes(const float * _values, int _numChannels)
{
    //Checks the current volumns are higher than the passed in values and will set them
    //to the new master volumns if they are above.

    GReturn result = GReturn::FAILURE;
    bool didChange = false;

    if (_numChannels <= 0)
        return result;
    if (audio == nullptr)
        return result;
    if (_values == nullptr)
        return result;

    unsigned int currentChannels;
    result = GetSoundSourceChannels(currentChannels);
    if (result != SUCCESS)
        return result;

    float *newValues = new float[_numChannels]; //Used to be passed into the SetChannelVolumns()

    //Loop through the number of channels you want to check against.
    for(int i = 0; i < _numChannels; i++)
    {
        if(vol.values[i] > _values[i])
        {
            //If current volumn is greater than passed in volumn, set it to the passed in volumn.
            vol.values[i] = _values[i];
            newValues[i] = _values[i];
            didChange = true;
        }
        else
        {
            //Fill out float array with the current volumn values.
            newValues[i] = vol.values[i];
        }
    }

    if(didChange == true)
    {
        SetChannelVolumes(newValues, currentChannels);
    }

    //Free any memory that is used
    delete[] newValues;

    /*
    float * currentValues = new float[currentChannels];

    if (currentValues == nullptr)
        return result;

    bool didChange = false;
    for (int i = 0; i < _numChannels; i++)
    {
        if (currentValues[i] > _values[i])
        {
           currentValues[i] = _values[i];
            didChange = true;
        }
    }
    if (didChange == true)
    {
    float * newVals = new float[currentChannels];
    for (int i = 0; i < _numChannels; i++)
    {
        newVals[i] = _values[i];
    }
    SetChannelVolumes(newVals,currentChannels);
    delete newVals;
    }
*/

result = SUCCESS;
    return result;
}
GReturn LinuxAppSound::GetSoundSourceChannels(unsigned int & returnedChannelNum)
{
    GReturn result = FAILURE;
    if (audio == nullptr)
    {
        return result;
    }
    returnedChannelNum = myFile.myFormat.mNumChannels;
    result = SUCCESS;
    return result;
}
GReturn LinuxAppSound::GetSoundOutputChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == nullptr)
	{
		return result;
	}
	returnedChannelNum = audio->numberOfOutputs;
	result = SUCCESS;
	return result;
}
GReturn LinuxAppSound::SetVolume(float _newVolume)
{
    //Sets all channels volume to the passed in value.
    GReturn result = INVALID_ARGUMENT;

    if(_newVolume < 0.0f)
        return result;
    if(audio == nullptr)
        return result;

    result = SUCCESS;

    theCallback.contextOperationSucceed = FinishedContextGeneral;
    theCallback.cbContextSucceed = theCallback.contextOperationSucceed;

    //Make sure the passed in value is not over the maxVolume if yes, then set it to the maxVolume.
    if(_newVolume > audio->maxVolume)
    {
        _newVolume = audio->maxVolume;
    }

     unsigned int channelNum = 0;
    result = GetSoundSourceChannels(channelNum);

     if(result!= SUCCESS)
        return result;

     vol.channels = channelNum;

   for(int i = 0; i < channelNum; i++)
   {
        //Convert a linear factor to a volume value.
       vol.values[i] = pa_sw_volume_from_linear(_newVolume);
   }

    //Returns the sink resp. source output index this stream is identified in the server with
    uint32_t index = pa_stream_get_index(myStream);

    //Set the volume of a sink input stream. And catch the returned operation in theCallback struct to let the rest of the system know whats going on
   theCallback.myOperation = pa_context_set_sink_volume_by_index(myContext,index,&vol,theCallback.cbContextSucceed, &theCallback);

   pa_operation_unref(theCallback.myOperation);

    return result;



    /*
    GReturn result = INVALID_ARGUMENT;

    if (_newVolume < 0.0f)
        return result;

    result = SUCCESS;

    if (audio == nullptr)
        return result;

   //TJCALLBACK theCallback;
    theCallback.contextOperationSucceed = FinishedContextGeneral;
    theCallback.cbContextSucceed = theCallback.contextOperationSucceed;
    if (_newVolume > audio->maxVolume)
    {
        _newVolume = audio->maxVolume;
    }
    unsigned int channelNum = 0;
    pa_cvolume theVolume;
    result = GetSoundSourceChannels(channelNum);

     if(result!= SUCCESS)
        return result;

     theVolume.channels = channelNum;

    uint32_t index = pa_stream_get_index(myStream);
    pa_context_set_sink_volume_by_index(myContext,index,&theVolume,theCallback.cbContextSucceed, &theCallback);


    return result;
    */
}
GReturn LinuxAppSound::StreamSound()
{
    GReturn theResult = SUCCESS;

//TJCALLBACK myCallback;
theCallback.streamOperationSucceed = FinishedDrain;
theCallback.cbSucceed = theCallback.streamOperationSucceed;
const time_t t0 = time(nullptr);
unsigned int playBackPt = 0;
 pa_stream_state_t state;
time_t prevT = time(nullptr) -1;

        isPlaying = true;
        isPaused = false;
        bool writeSizeWasZero = false;

        while(true)
        {
            if(stopFlag == true)
            {
                pa_stream_cancel_write((myStream));
                break;
            }
            if(isPaused != true)
            {
                if(time(nullptr) != prevT)
                {
                    prevT = time(nullptr);
                }

                state =  pa_stream_get_state(myStream);
                //std::cout << "my current state: " << state << "\n"; //TESTING

            if(PA_STREAM_READY == state)
                {

                    const size_t writeableSize = pa_stream_writable_size(myStream);
                    const size_t sizeRemain = myFile.myBuffer.byteSize - playBackPt;
                    const size_t writeSize = (sizeRemain < writeableSize ? sizeRemain : writeableSize);

                    if(writeSize > 0)
                    {
                       // void* dataPTR = myFile.myBuffer.bytes + playBackPt; //TESTING
                       // pa_stream_begin_write(myStream, &dataPTR, (size_t*)writeSize); //TESTING

                        pa_stream_write(myStream, myFile.myBuffer.bytes + playBackPt , writeSize,nullptr, 0, PA_SEEK_RELATIVE);
                        playBackPt +=writeSize;

                }
                else if (writeableSize > 0 && theCallback.didFinish != 1)
                {
                      //delete theCallback.myOperation;
                      theCallback.myOperation = pa_stream_drain(myStream,theCallback.cbSucceed,&theCallback);

                      pa_operation_unref(theCallback.myOperation);

                       break;
                    }


                }


            }
        }
    if(stopFlag == false)
    {

        while(true)
        {
            std::this_thread::yield();
            if(theCallback.didFinish == 1)
            {
            isPlaying = false;
            isPaused = true;
            break;
            }


        }
    }
    return theResult;
}
GReturn LinuxAppSound::Play()
{
    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;
    if(myStream == nullptr)
        return result;

if (!isPlaying)
    {
        stopFlag = false;
       // isPlaying = true;
        isPaused = false;
        streamThread = new std::thread(&LinuxAppSound::StreamSound, this);
        result = SUCCESS;
    }
else if(streamThread != nullptr)
{
    result = SUCCESS;
}

    return result;

}
GReturn LinuxAppSound::Pause()
{
    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;
   // TJCALLBACK myCallback;
    if(isPlaying)
    {
        int value = pa_stream_is_corked(myStream);// 1 = paused, 0 = resumed


        if(value == 0)
        {
            theCallback.myOperation =  pa_stream_cork(myStream, 1, theCallback.cbSucceed,&theCallback );
            pa_operation_unref(theCallback.myOperation);
        }

            isPaused = true;
            isPlaying = false;


    }
    if (!isPaused)
    {
        isPlaying = false;
        isPaused = true;
    }
    result = SUCCESS;
    return result;

}
GReturn LinuxAppSound::Resume()
{

    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;
   // TJCALLBACK myCallback;
    if(!isPlaying)
    {
        int value = pa_stream_is_corked(myStream); //1 = paused, 0 = resumed


        if(value == 1)
        {
             theCallback.myOperation = pa_stream_cork(myStream, 0, theCallback.cbSucceed,&theCallback );
             pa_operation_unref(theCallback.myOperation);
        }
           isPaused = false;
           isPlaying = true;
    }
    if (isPaused)
    {
        isPlaying = true;
        isPaused = false;
    }
    result = SUCCESS;
    return result;

}
GReturn LinuxAppSound::StopSound()
{
    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;

if (streamThread != nullptr)
  {

    if(stopFlag != true)
       {
    stopFlag = true;
    streamThread->join();
    delete streamThread;
    streamThread = nullptr;
       }
  }

    isPlaying = false;
    isPaused = true;


    result = SUCCESS;
    return result;
}
LinuxAppSound::~LinuxAppSound()
{
pa_stream_disconnect(myStream);
pa_stream_unref(myStream);
pa_context_disconnect(myContext);
pa_context_unref(myContext);
pa_mainloop_free(myMainLoop);
//pa_operation_unref(theCallback.myOperation);
}
GReturn LinuxAppSound::isSoundPlaying(bool & _returnedBool)
{
	_returnedBool = isPlaying;
	return SUCCESS;
}
GReturn LinuxAppSound::GetCount(unsigned int & _outCount)
{
	GReturn result = FAILURE;
	_outCount = SoundCounter;
	result = SUCCESS;

	return result;
}
GReturn LinuxAppSound::IncrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if increment would overflow
	if (SoundCounter == UINT_MAX)
		return result;
	SoundCounter++;
	result = SUCCESS;
	return result;
}
GReturn LinuxAppSound::DecrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if decrement would underflow
	if (SoundCounter == 0)
		return result;
	SoundCounter--;
	result = SUCCESS;
	return result;
}
GReturn LinuxAppSound::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
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
//End of GSound implementation

//Start of GMusic implementation
LinuxAppMusic::LinuxAppMusic() : MusicCounter(1){}
GReturn LinuxAppMusic::Init()
{
GReturn result = GReturn::INVALID_ARGUMENT;
//std::cout << "\n IN MUSIC INIT()  \n\n";
    if(audio == nullptr)
    {
       // std::cout << "\n AUDIO == NULLPTR \n";
        return result;
    }


    result = REDUNDANT_OPERATION;

    if(myStream != nullptr)
       {
          // std::cout << "\n MYSTREAM == NULLPTR \n";
           return result;
       }

    if(myMap != nullptr)
    {
       // std::cout << "\n MYMAP == NULLPTR \n";
        return result;
    }

    result = createMainLoopAndContext(&myMainLoop,&myContext);

    if(result != SUCCESS)
    {
       // std::cout << "\n RESULT != SUCCESS \n";
        return result;
    }

    result = FAILURE;

    if(myMainLoop == nullptr)
    {
       // std::cout << "\n MYMAINLOOP == NULLPTR \n";
        return result;
    }

    if(myContext == nullptr)
    {
       // std::cout << "\n MYCONTEXT == NULLPTR \n";
        return result;
    }

     //This thread iterates the mainloop.
   // std::cout << "\n\n BEFORE THREAD \n\n";
  //  loopThread = new std::thread(RunMainLoop, myMainLoop);
 //   std::cout << "\n\n PASSED THROUGH THREAD \n\n";
    pa_ready = 0;
   // std::cout << "\n\n BEFORE PA_CONTEXT_CONNECT \n\n";
    pa_context_connect(myContext, NULL, (pa_context_flags_t)0,NULL);
  //  std::cout << "\n\n PASSED THROUGH PA_CONTEXT_CONNECT \n\n";

    pa_context_set_state_callback(myContext, OnStateChange, &pa_ready);

      //Had to move this thread creation below the pa_context_connect()
    //so it starts searching for the contexts state at the start of the mainloop thread loop.
    //Instead of somewhere random within it.
    loopThread = new std::thread(RunMainLoop, myMainLoop);
  //  bool connected = WaitForConnectionEstablished(myMainLoop, myContext, 120);
  while(pa_ready == 0)
   {
      // pa_mainloop_iterate(myMainLoop, 1, NULL);
      std::this_thread::yield();

   }
    if(pa_ready == 1)
    {
        result = SUCCESS;
    }
    else
    {
      //  std::cout << "\n NOT CONNECTED \n";
        return result;
    }
     result = FAILURE;
    switch(myPCMFormat.mBitsPerSample)
    {
    case 8:
    myPulseFormat = PA_SAMPLE_U8;
        break;
    case 16:
    myPulseFormat = PA_SAMPLE_S16LE;
        break;
    default:
    myPulseFormat = PA_SAMPLE_S16LE;
        break;
    }
    int rate = myPCMFormat.mSamples;
    int Channels = myPCMFormat.mNumChannels;

    pa_sample_spec mySampleSpec = {myPulseFormat,rate,Channels};
    int paCheck = 0;
    paCheck = pa_channels_valid(mySampleSpec.channels);
    myMap = new pa_channel_map();
    myMap = pa_channel_map_init_extend(myMap, mySampleSpec.channels,PA_CHANNEL_MAP_WAVEEX);
    if(myMap == nullptr)
    {
       // std::cout << "\n MYMAP == NULLPTR 2 \n";
        return result;
    }

        myBuffers[0].bytes = new uint8_t[STREAMING_BUFFER_SIZE];
        myBuffers[1].bytes = new uint8_t[STREAMING_BUFFER_SIZE];
        myBuffers[2].bytes = new uint8_t[STREAMING_BUFFER_SIZE];

        myBuffers[0].byteSize = fileSize;
        myBuffers[1].byteSize = fileSize;
        myBuffers[2].byteSize = fileSize;

    myStream = pa_stream_new(myContext,"Music",&mySampleSpec,nullptr);
    if(myStream == nullptr)
    {
       // std::cout << "\n MYSTREAM == NULLPTR \n";
        return result;
    }
    int pcheck = pa_stream_connect_playback(myStream,NULL,NULL,(pa_stream_flags_t)0,NULL,NULL);
     pa_stream_state_t state =  pa_stream_get_state(myStream);
    if(pa_stream_get_context(myStream) != myContext)
    {
      //  std::cout << "\n CONTEXT MISMATCH \n";
        return result;
    }
    isPlaying = false;
    result = SUCCESS;
    return result;
}
GReturn LinuxAppMusic::SetPCMShader(const char* _data)
{
    GReturn result = FAILURE;

    return result;
}
GReturn LinuxAppMusic::SetChannelVolumes(float * _values, int _numChannels)
{
//Sets the current volume channels to the values passed in.
//_values = volume values from 0 -> 1.0
//_numChannels = the size of _values data by index

     GReturn result = INVALID_ARGUMENT;

    if (_numChannels <= 0)
        return result;
    if (audio == nullptr)
        return result;
    if (_values == nullptr)
        return result;

    result = FAILURE;

    vol.channels = _numChannels;

    for (int i = 0; i < _numChannels; i++)
    {
        //Make sure the passed in values are not over the maxVolume if yes, then set it to the maxVolume.
            if (_values[i] > audio->maxVolume)
            {
                _values[i] = audio->maxVolume;
            }

        //Convert a linear factor to a volume value.
            vol.values[i] =   pa_sw_volume_from_linear(_values[i]);
    }

    //TJCALLBACK theCallback;
    theCallback.contextOperationSucceed = FinishedContextGeneral;
    theCallback.cbContextSucceed = theCallback.contextOperationSucceed;

    //Returns the sink resp. source output index this stream is identified in the server with
    uint32_t index = pa_stream_get_index(myStream);

    //Set the volume of a sink input stream. And catch the returned operation in theCallback struct to let the rest of the system know whats going on
    theCallback.myOperation = pa_context_set_sink_input_volume(myContext,index,&vol, theCallback.cbContextSucceed, &theCallback);

    pa_operation_unref(theCallback.myOperation);

    result = SUCCESS;
    return result;




    //GReturn result = INVALID_ARGUMENT;
    //if (_numChannels <= 0)
    //    return result;
   // if (audio == nullptr)
   //     return result;

  //  if (_values == nullptr)
  //      return result;

  //  pa_volume_t * newValues = new pa_volume_t[_numChannels];

  //  for (int i = 0; i < _numChannels; i++)
 //   {
  //       newValues[i] = 0;
  //  }
 //   pa_cvolume theVolume;
 //   theVolume.channels = _numChannels;
 //   result = FAILURE;

 //   for (int i = 0; i < _numChannels; i++)
   // {
 //           if (_values[i] > audio->maxVolume)
  //          {
   //             _values[i] = audio->maxVolume;
   //         }
  //          newValues[i] =   pa_sw_volume_from_linear(_values[i]);
  //          theVolume.values[i] = newValues[i];
 //   }

    //TJCALLBACK theCallback;
 //   theCallback.contextOperationSucceed = FinishedContextGeneral;
 //   theCallback.cbContextSucceed = theCallback.contextOperationSucceed;
 //   uint32_t index = pa_stream_get_device_index(myStream);
//     uint32_t index2 = pa_stream_get_index(myStream);
 //    pa_context_set_sink_input_volume(myContext,index2,&theVolume, theCallback.cbContextSucceed, &theCallback);

   // delete newValues;
//    delete[] newValues;

 //   result = SUCCESS;
 //   return result;

}
GReturn LinuxAppMusic::CheckChannelVolumes(const float * _values, int _numChannels)
{
    //Checks the current volumns are higher than the passed in values and will set them
    //to the new master volumns if they are above.
    GReturn result = GReturn::INVALID_ARGUMENT;
    bool didChange = false;

    if (_numChannels <= 0)
        return result;
    if (audio == nullptr)
        return result;

    if (_values == nullptr)
        return result;

    result = FAILURE;
    unsigned int currentChannels;
    result = GetStreamSourceChannels(currentChannels);
    if (result != SUCCESS)
        return result;

 float *newValues = new float[_numChannels]; //Used to be passed into the SetChannelVolumns()

    //Loop through the number of channels you want to check against.
    for(int i = 0; i < _numChannels; i++)
    {
        if(vol.values[i] > _values[i])
        {
            //If current volumn is greater than passed in volumn, set it to the passed in volumn.
            vol.values[i] = _values[i];
            newValues[i] = _values[i];
            didChange = true;
        }
        else
        {
            //Fill out float array with the current volumn values.
            newValues[i] = vol.values[i];
        }
    }

    if(didChange == true)
    {
        SetChannelVolumes(newValues, currentChannels);
    }

    //Free any memory that is used
    delete[] newValues;

   // float * currentValues = new float [currentChannels];

  //  if (currentValues == nullptr)
  //      return result;

  //  bool didChange = false;
  //  for (int i = 0; i < _numChannels; i++)
  //  {
  //          if (currentValues[i] > _values[i])
  //          {
  //              currentValues[i] = _values[i];
  //              didChange = true;
  //          }
   // }
  // if (didChange == true)
  //  {
  //  float * newVals = new float[currentChannels];
  //  for (int i = 0; i < _numChannels; i++)
 //   {
  //      newVals[i] = _values[i];
 //   }
 //   SetChannelVolumes(newVals,currentChannels);
   // delete newVals;
  //  }


result = SUCCESS;
    return result;
}
GReturn LinuxAppMusic::GetStreamSourceChannels(unsigned int & returnedChannelNum)
{
    GReturn result = FAILURE;
    if (audio == nullptr)
    {
        return result;
    }
    if(myFilePath == nullptr)
    {
        return result;
    }

    returnedChannelNum = myPCMFormat.mNumChannels;
    result = SUCCESS;
    return result;
}
GReturn LinuxAppMusic::GetStreamOutputChannels(unsigned int & returnedChannelNum)
{
	GReturn result = FAILURE;
	if (audio == nullptr)
	{
		return result;
	}
	if (myFilePath == nullptr)
	{
		return result;
	}

	returnedChannelNum = audio->numberOfOutputs;
	result = SUCCESS;
	return result;
}
GReturn LinuxAppMusic::SetVolume(float _newVolume)
{
    //Sets all channels volume to the passed in value.
    GReturn result = INVALID_ARGUMENT;

    if(_newVolume < 0.0f)
        return result;
    if(audio == nullptr)
        return result;

    result = SUCCESS;

    theCallback.contextOperationSucceed = FinishedContextGeneral;
    theCallback.cbContextSucceed = theCallback.contextOperationSucceed;

    //Make sure the passed in value is not over the maxVolume if yes, then set it to the maxVolume.
    if(_newVolume > audio->maxVolume)
    {
        _newVolume = audio->maxVolume;
    }

     unsigned int channelNum = 0;
    result = GetStreamSourceChannels(channelNum);

     if(result!= SUCCESS)
        return result;

     vol.channels = channelNum;

   for(int i = 0; i < channelNum; i++)
   {
        //Convert a linear factor to a volume value.
       vol.values[i] = pa_sw_volume_from_linear(_newVolume);
   }

    //Returns the sink resp. source output index this stream is identified in the server with
    uint32_t index = pa_stream_get_index(myStream);

    //Set the volume of a sink input stream. And catch the returned operation in theCallback struct to let the rest of the system know whats going on
   theCallback.myOperation = pa_context_set_sink_volume_by_index(myContext,index,&vol,theCallback.cbContextSucceed, &theCallback);

   pa_operation_unref(theCallback.myOperation);

    return result;



    /*
    GReturn result = FAILURE;
    if (audio == nullptr)
        return result;

    result = INVALID_ARGUMENT;

    if (_newVolume < 0.0f)
        return result;

    result = FAILURE;

 //TJCALLBACK theCallback;
    theCallback.contextOperationSucceed = FinishedContextGeneral;
    theCallback.cbContextSucceed = theCallback.contextOperationSucceed;

    if (_newVolume > audio->maxVolume)
    {
        _newVolume = audio->maxVolume;
    }
    unsigned int channelNum = 0;
     pa_cvolume theVolume;
     result = GetStreamSourceChannels(channelNum);
     if(result!= SUCCESS)
        return result;
     theVolume.channels = channelNum;

    uint32_t index2 = pa_stream_get_index(myStream);
    pa_context_set_sink_volume_by_index(myContext,index2,&theVolume,theCallback.cbContextSucceed, &theCallback);

    result = SUCCESS;
    return result;
    */
}
GReturn LinuxAppMusic::StreamMusic()
{
GReturn theResult = SUCCESS;

//TJCALLBACK myCallback;
theCallback.streamOperationSucceed = FinishedDrain;
theCallback.cbSucceed = theCallback.streamOperationSucceed;
const time_t t0 = time(nullptr);
 long playBackPt = 0;
unsigned int currplayBackPt = 0;

        FILE * someWaveFile = NULL;
someWaveFile = fopen(myFilePath, "r");

    if(someWaveFile == NULL)
   {
       return FAILURE;
   }
    int yo = 0;
   unsigned long dwChunktype = 0;
unsigned long dwChunkDataSize = 0;
unsigned long bytesRead = 0;
unsigned long throwAway = 0;
   unsigned long dwRead = 0;
   unsigned int breakOut = -1;
PCM_FORMAT_INFO throwAwayInfo;


    // read the first 12 bytes for the file for RIFF data
   if(someWaveFile != NULL)
    {
        while(breakOut == -1)
        {
        unsigned long dwRead = 0;
        dwRead = fread(&dwChunktype,1,4,someWaveFile);
            if(dwRead!= 4)
            {
                return FAILURE;
            }
            bytesRead += dwRead;

             dwRead = fread(&dwChunkDataSize,1,4,someWaveFile);
            if(dwRead!= 4)
            {
                  return FAILURE;
            }
            bytesRead += dwRead;

            switch(dwChunktype)
            {
                case fourRIFFcc:
                {

                dwChunkDataSize = 4;

                dwRead = fread(&throwAway,1,4,someWaveFile);
                    if(dwRead!= 4)
                    {
                           return FAILURE;
                    }
                bytesRead += dwRead;
                 break;
                }
                case fourWAVEcc:
                {

                    dwRead = fread(&throwAway,1,4,someWaveFile);
                    if(dwRead!= 4)
                    {
                           return FAILURE;
                    }
                    bytesRead += dwRead;

                    break;
                }
                case fourFMTcc:
                {
                   dwRead = fread(&throwAwayInfo,1, dwChunkDataSize,someWaveFile);
                  if(dwRead!= dwChunkDataSize)
                    {
                          return FAILURE;
                    }
                    bytesRead += dwRead;
                    breakOut = 1;
                break;
                }
        }

    }
      fread(myBuffers[0].bytes,1,STREAMING_BUFFER_SIZE,someWaveFile);
            fread(myBuffers[1].bytes,1,STREAMING_BUFFER_SIZE,someWaveFile);


    }
playBackPt += bytesRead;
int prevbuffer = 2;
int nextBuffer = 1;
unsigned int readSize = (STREAMING_BUFFER_SIZE * 3 < fileSize-playBackPt? STREAMING_BUFFER_SIZE : fileSize - playBackPt);
time_t prevT = time(nullptr) -1;
unsigned int currentBuffer = 0;
isPlaying = true;
isPaused = false;

bool ready = false;
 pa_stream_state_t state;
  int errCheck = 0;
        while(true)
        {
            if(stopFlag == true)
            {
                pa_stream_cancel_write((myStream));

                break;
            }

        if(isPaused != true)
        {
            if(time(nullptr) != prevT)
            {
                prevT = time(nullptr);
            }

         state  =  pa_stream_get_state(myStream);

         if(PA_STREAM_READY == state)
            {


                const size_t writeableSize = pa_stream_writable_size(myStream);
                const size_t sizeRemain = STREAMING_BUFFER_SIZE - currplayBackPt;
                const size_t writeSize = (sizeRemain < writeableSize ? sizeRemain : writeableSize);
                readSize = (STREAMING_BUFFER_SIZE < fileSize-playBackPt? STREAMING_BUFFER_SIZE : fileSize - playBackPt);
                if(writeSize > 0)
                {
                    pa_stream_write(myStream, myBuffers[currentBuffer].bytes + currplayBackPt , writeSize,nullptr, 0, PA_SEEK_RELATIVE);
                    playBackPt +=writeSize;
                    currplayBackPt += writeSize;

                }
                else if(writeableSize > 0  )
                {
                    if((fileSize - playBackPt) > 0)
                    {
                    ready = true;
                    currplayBackPt = 0;
                    }
                    else if (theCallback.didFinish != 1 )
                    {
                        if(loops)
                        {
                    errCheck  = fseek(someWaveFile,0,SEEK_SET);
                       playBackPt = 0;
                        }
                        else
                        {
                      theCallback.myOperation = pa_stream_drain(myStream,theCallback.cbSucceed,&theCallback);
                       break;
                        }


                    }
                }



            }

            if(PA_STREAM_FAILED == state)
            {
                //Used to debug the error
                const char * err = pa_strerror(pa_context_errno(myContext));

            }

    if(ready)
    {
                      currentBuffer++;
                    if(currentBuffer >= MAX_BUFFER_COUNT)
                        currentBuffer = 0;

                           prevbuffer++;
                    if(prevbuffer >= MAX_BUFFER_COUNT)
                        prevbuffer = 0;

                                      nextBuffer++;
                    if(nextBuffer >= MAX_BUFFER_COUNT)
                        nextBuffer = 0;
                    fread(myBuffers[nextBuffer].bytes,1,readSize,someWaveFile);
                    ready = false;
    }




        }


        }
    if(stopFlag == false)
    {


        while(true)
        {
            if(theCallback.didFinish == 1)
            {
            isPlaying = false;
            isPaused = true;
            break;
            }



        }
    }
    fclose(someWaveFile);
    return theResult;
}
GReturn LinuxAppMusic::StreamStart(bool _loop)
{
    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;
    if(myStream == nullptr)
        return result;

if (!isPlaying)
    {
	loops = _loop;
    stopFlag = false;

    isPlaying = true;
    isPaused = false;
    streamThread = new std::thread(&LinuxAppMusic::StreamMusic,this);
    result = SUCCESS;
    }


    return result;
}
GReturn LinuxAppMusic::PauseStream()
{
    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;
    //TJCALLBACK myCallback;
    if(isPlaying)
    {

            isPaused = true;
            isPlaying = false;
                result = SUCCESS;
    }
    else
    {

        result = REDUNDANT_OPERATION;
    }


    return result;
}
GReturn LinuxAppMusic::ResumeStream()
{
    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;
   // TJCALLBACK myCallback;

    if (!isPlaying)
    {


            isPaused = false;
            isPlaying = true;
               result = SUCCESS;
    }
    else
    {
        result = REDUNDANT_OPERATION;
    }
    return result;
}
GReturn LinuxAppMusic::StopStream()
{
    GReturn result = GReturn::FAILURE;
    if (audio == nullptr)
        return result;

    if (streamThread != nullptr)
    {

       if(stopFlag != true)
       {
    stopFlag = true;
    streamThread->join();

    delete streamThread;
    streamThread = nullptr;
       }
    }
    isPlaying = false;
    isPaused = true;

    result = SUCCESS;
    return result;
}
GReturn LinuxAppMusic::isStreamPlaying(bool & _returnedBool)
{
	_returnedBool = isPlaying;
	return SUCCESS;
}
GReturn LinuxAppMusic::GetCount(unsigned int & _outCount)
{
	GReturn result = FAILURE;
	_outCount = MusicCounter;
	result = SUCCESS;

	return result;
}
GReturn LinuxAppMusic::IncrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if increment would overflow
	if (MusicCounter == UINT_MAX)
		return result;
	MusicCounter++;
	result = SUCCESS;
	return result;
}
GReturn LinuxAppMusic::DecrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if decrement would underflow
	if (MusicCounter == 0)
		return result;
	MusicCounter--;
	result = SUCCESS;
	return result;
}
GReturn LinuxAppMusic::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
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
LinuxAppMusic::~LinuxAppMusic()
{
	pa_stream_disconnect(myStream);
	pa_stream_unref(myStream);
	pa_context_disconnect(myContext);
	pa_context_unref(myContext);
	pa_mainloop_free(myMainLoop);
}
//End of GMusic implementation
LinuxAppAudio::LinuxAppAudio() : AudioCounter(1){}

GReturn LinuxAppAudio::Init(int _numOfOutputs)
{
    GReturn result = SUCCESS;
	//number of outputs currently only working and tested on Windows and is not used on Linux yet

    return result;
}
GReturn LinuxAppAudio::CreateSound(const char* _path, GSound** _outSound)
{

    GReturn result = FAILURE;
    if (_outSound == nullptr)
    {
        result = INVALID_ARGUMENT;
        return result;
    }
    LinuxAppSound* snd = new LinuxAppSound();

    if (snd == nullptr)
    {
       // std::cout << "\n\n" << " msc = new LinuxAppSound() didn't work" << "\n";
        result = FAILURE;
        return result;
    }

    int check = LoadWav(_path,snd->myFile);
    if(check != 0)
    {
       // std::cout << "\n\n" << " check != 0" << "\n";
        if(result > 0)
        {
            result = INVALID_ARGUMENT;
        }
        else
        {
            result = FAILURE;
        }
        return result;
    }
    snd->audio = this;
    result = snd->Init();
    if(result != SUCCESS)
    {
       // std::cout << "\n\n" << " snd->Init() failed" << "\n";
        return result;
    }
    activeSounds.push_back(snd);
    *_outSound = snd;

    result = SUCCESS;
    return result;
}
GReturn LinuxAppAudio::CreateMusicStream(const char* _path, GMusic** _outMusic)
{
    GReturn result = FAILURE;
    if (_outMusic == nullptr)
    {
        //std::cout << "\n" << "_outMusic was null" << "\n";
        result = INVALID_ARGUMENT;
        return result;
    }
    LinuxAppMusic* msc = new LinuxAppMusic();

    if (msc == nullptr)
    {
        //std::cout << "\n\n" << "msc = new LinuxAppMusic() didn't work" << "\n";
        result = FAILURE;
        return result;
    }
    int check = LoadWavFormatOnly(_path,msc->myPCMFormat, msc->fileSize);
    if(check != 0)
    {
       // std::cout << "\n\n" << "LoadWavFormatOnly didn't work" << "\n";
        if(result > 0)
        {
            result = INVALID_ARGUMENT;
        }
        else
        {
            result = FAILURE;
        }
        return result;
    }
    msc->audio = this;
    int len = strlen(_path) + 1;
    msc->myFilePath = new char[len];
    strncpy(msc->myFilePath, _path, len);

    result = msc->Init();
    if (result != SUCCESS)
    {
       // std::cout << "\n\n" << " msc->init() didn't work" << "\n";
        return result;
    }
    result = SUCCESS;
    activeMusic.push_back(msc);
    *_outMusic = msc;

    return result;
}
GReturn LinuxAppAudio::SetMasterVolume(float _value)
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
GReturn LinuxAppAudio::SetMasterChannelVolumes(const float * _values, int _numChannels)
{
 GReturn result = INVALID_ARGUMENT;

    if (_values == nullptr)
        return result;
    if (_numChannels < 0)
        return result;

    result = FAILURE;

    for (int i = 0; i < activeSounds.size(); i++)
    {
        result = activeSounds[i]->CheckChannelVolumes(_values, _numChannels);
    }
    for (int i = 0; i < activeMusic.size(); i++)
    {
        result = activeMusic[i]->CheckChannelVolumes(_values, _numChannels);
    }

    return result;



/*
    GReturn result = INVALID_ARGUMENT;

    if (_values == nullptr)
        return result;
    if (_numChannels < 0)
        return result;

    result = FAILURE;
    unsigned int theirChannels;
    for (int i = 0; i < activeSounds.size(); i++)
    {
        result = activeSounds[i]->GetSoundSourceChannels(theirChannels);
        if (result != SUCCESS)
        {
          return result;
        }

        for (int k = 0; k < theirChannels; k++)
        {
            result = activeSounds[i]->CheckChannelVolumes(_values, theirChannels);
        }

        if (result != SUCCESS)
        {

        }
    }
    for (int i = 0; i < activeMusic.size(); i++)
    {
        result = activeMusic[i]->GetStreamSourceChannels(theirChannels);
        if (result != SUCCESS)
        {
                result = activeMusic[i]->CheckChannelVolumes(_values, _numChannels);
        }

        if (result != SUCCESS)
        {
                result = activeMusic[i]->CheckChannelVolumes(_values, _numChannels);
        }
    }

    return result;
    */
}
GReturn LinuxAppAudio::PauseAll()
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
GReturn LinuxAppAudio::StopAll()
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
   result = SUCCESS;
    return result;
}
GReturn LinuxAppAudio::ResumeAll()
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
result = SUCCESS;
    return result;
}
GReturn LinuxAppAudio::GetCount(unsigned int & _outCount)
{
	GReturn result = FAILURE;
	_outCount = AudioCounter;
	result = SUCCESS;

	return result;
}
GReturn LinuxAppAudio::IncrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if increment would overflow
	if (AudioCounter == UINT_MAX)
		return result;
	AudioCounter++;
	result = SUCCESS;
	return result;
}
GReturn LinuxAppAudio::DecrementCount()
{
	GReturn result = FAILURE;
	//Results in Failure if decrement would underflow
	if (AudioCounter == 0)
		return result;
	AudioCounter--;
	result = SUCCESS;
	return result;
}
GReturn LinuxAppAudio::RequestInterface(const GUUIID & _interfaceID, void ** _outputInterface)
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
LinuxAppAudio::~LinuxAppAudio()
{
	while (activeSounds.size() > 0)
	{
		activeSounds.erase(activeSounds.begin());
	}
	while (activeMusic.size() > 0)
	{
		activeMusic.erase(activeMusic.begin());
	}
}





//Start of GAudio implementation for Windows
GReturn PlatformGetAudio(GAudio ** _outAudio)
{
    GReturn result = FAILURE;
    if (_outAudio == nullptr)
    {
        result = INVALID_ARGUMENT;
        return result;
    }
    LinuxAppAudio* audio = new LinuxAppAudio();

    if (audio == nullptr)
    {
        result = FAILURE;
        return result;
    }
    audio->Init();

    if (result == INVALID_ARGUMENT)
        return result;

	//Initalize GAudio's maxVolumn
	audio->maxVolume = 1.0f;

    *_outAudio = audio;
    result = SUCCESS;
    return result;
}
//End of GMusic implementation for Windows
