// Override export symbols for DLL builds (must be included before interface code).
#include "../../Source/DLL_Export_Symbols.h"
#include "../../Interface/G_System/GController.h"

#include <mutex>
#include <atomic>
#include <thread>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <xinput.h>
//#include <direct.h>
#pragma comment(lib, "XInput.lib")
#endif // _WIN32



#ifdef __APPLE__
#include <pthread.h>
#endif



#include "../../Source/G_System/GController_Callback.hpp"




using namespace GW;
using namespace CORE;
using namespace SYSTEM;


class GeneralController : public GController
{
private:
        void Linux_InitControllers();
        void Linux_InotifyLoop();
        void Linux_ControllerInputLoop(char* _filePath, unsigned int _controllerIndex, int fd);

#ifdef __linux__
        std::thread* linuxControllerThreads[MAX_CONTROLLER_INDEX];
        std::thread* linuxInotifyThread;
        std::atomic<bool> iscontrollerLoopRunning[MAX_CONTROLLER_INDEX];
        char* controllerFilePaths[MAX_CONTROLLER_INDEX];
#endif

#ifdef __APPLE__
    NSThread* runLoopThread;
    HIDMANAGER * manager;
    pthread_attr_t  runLoopPthread_attr;
    pthread_t       runLoopPthread_ID;
#endif

protected:

	std::atomic<unsigned int> referenceCount;
	std::atomic<bool> isRunning;
    std::mutex controllersMutex;
    std::mutex listenerMutex;
    CONTROLLER_STATE* controllers;
    int  supportedControllerID;

	// Lock before using

	GControllerDeadzoneTypes deadzoneType;
	float deadzonePercentage;

public:
	//GeneralController
	GeneralController();
	virtual ~GeneralController();

    virtual void Init();
    void SetSupportedControllerID (int _controllerID) {supportedControllerID = _controllerID;};

	// GController
	virtual GReturn GetState(int _controllerIndex, int _inputCode, float& _outState);
	virtual GReturn IsConnected(int _controllerIndex, bool& _outIsConnected);
	virtual GReturn GetMaxIndex(int &_outMax);
	virtual GReturn GetNumConnected(int &_outConnectedCount);
	virtual GReturn SetDeadZone(GControllerDeadzoneTypes _type, float _deadzonePercentage);
	virtual GReturn StartVibration(float _pan, float _duration, float _strength, unsigned int _controllerIndex);
	virtual GReturn IsVibrating(unsigned int _controllerIndex, bool& _outIsVibrating);
	virtual GReturn StopVirbration(unsigned int _controllerIndex);
	virtual GReturn StopAllVirbrations();

	// GBroadcasting
	GReturn RegisterListener(GListener* _addListener, unsigned long long _eventMask);
	GReturn DeregisterListener(GListener* _removeListener);

	// GInterface
	GReturn GetCount(unsigned int& _outCount);
	GReturn IncrementCount();
	virtual GReturn DecrementCount();
	GReturn RequestInterface(const GUUIID& _interfaceID, void** _outputInterface);
};

class XboxController : public GeneralController
{
private:

	void XinputLoop();
	float XboxDeadZoneCalc(float _value, bool _isTigger);

    int XControllerSlotIndices[4];
#ifdef _WIN32
	DWORD XControllerLastPacket[4];
#endif

	std::thread* xInputThread;
	std::thread* controllerVibThreads;
public:
	// XboxController
	XboxController();
	~XboxController();

	void XinputVibration();

	// GController
	void Init();
    GReturn GetState(int _controllerIndex, int _inputCode, float& _outState);
	GReturn GetMaxIndex(int &_outMax);
	GReturn GetNumConnected(int &_outConnectedCount);
	GReturn IsConnected(int _controllerIndex, bool& _outIsConnected);
	GReturn StartVibration(float _pan, float _duration, float _strength, unsigned int _controllerIndex);
	GReturn IsVibrating(unsigned int _controllerIndex, bool& _outIsVibrating);
	GReturn StopVirbration(unsigned int _controllerIndex);
	GReturn StopAllVirbrations();
	// GInterface
	GReturn DecrementCount();
};

GATEWARE_EXPORT_IMPLICIT GReturn CreateGController(int _controllerType, GController** _outController)
{
	return GW::SYSTEM::CreateGController(_controllerType, _outController);
}

GReturn GW::SYSTEM::CreateGController(int _controllerType, GController** _outController)
{
	if(_outController == nullptr || _controllerType < 0 || _controllerType > 0xFF)
		return INVALID_ARGUMENT;

	switch (_controllerType)
	{
	case G_GENERAL_CONTROLLER: // not supported on Windows until directinput is added
	{

#ifdef _WIN32
		_outController = nullptr;
		return FEATURE_UNSUPPORTED;
#else
		GeneralController* genController = new GeneralController;
		if (genController == nullptr)
			return FAILURE;
        genController->SetSupportedControllerID(G_GENERAL_CONTROLLER);
        genController->Init();
		(*_outController) = genController;
#endif // !_WIN32


		break;
	}
	case G_XBOX_CONTROLLER:
	{
#ifdef _WIN32
		XboxController* xController = new XboxController;
		if (xController == nullptr)
			return FAILURE;
		xController->Init();
        xController->SetSupportedControllerID(G_XBOX_CONTROLLER);
		(*_outController) = xController;
#else
        GeneralController* genController = new GeneralController;
        if (genController == nullptr)
            return FAILURE;
        genController->SetSupportedControllerID(G_XBOX_CONTROLLER);
        genController->Init();
        (*_outController) = genController;
#endif // !_WIN32
		break;
	}

    case G_PS4_CONTROLLER:
    {
#ifdef _WIN32

        _outController = nullptr;
        return FEATURE_UNSUPPORTED;
#else
        GeneralController* genController = new GeneralController;
		if (genController == nullptr)
			return FAILURE;

        genController->SetSupportedControllerID(G_PS4_CONTROLLER);
        genController->Init();
        (*_outController) = genController;
#endif // !_WIN32
        break;
    }
	default:
	{
		return FEATURE_UNSUPPORTED;
	}
	}

	return SUCCESS;

}

// GeneralController

GeneralController::GeneralController()
{
	referenceCount = 1;
	isRunning = true;
}

GeneralController::~GeneralController()
{
}

void GeneralController::Init()
{
	controllers = new CONTROLLER_STATE[MAX_CONTROLLER_INDEX];
	deadzoneType = DEADZONESQUARE;
	deadzonePercentage = .2f;

	for (unsigned int i = 0; i < MAX_CONTROLLER_INDEX; ++i)
	{
	controllers[i].isConnected = 0;
	controllers[i].isVibrating = 0;
	controllers[i].vibrationDuration = 0;
	controllers[i].vibrationStartTime = new std::chrono::high_resolution_clock::time_point();
	controllers[i].maxInputs = MAX_GENENRAL_INPUTS;
	controllers[i].controllerInputs = new float[MAX_GENENRAL_INPUTS];
	for(unsigned int j = 0; j < MAX_GENENRAL_INPUTS; ++j)
	{
        controllers[i].controllerInputs[j] = 0.0f;
	}
	}


#ifdef __linux__
        for(int i = 0; i < MAX_CONTROLLER_INDEX; ++i)
            iscontrollerLoopRunning[i] = false;
        Linux_InitControllers();
        linuxInotifyThread = new std::thread(&GeneralController::Linux_InotifyLoop, this);

#endif // _linux_

#ifdef __APPLE__
    manager = [HIDMANAGER alloc];
    manager->isRunning = &isRunning;
    manager->controllersMutex = &controllersMutex;
    manager->listenerMutex = &listenerMutex;
    manager->controllers = controllers;
    manager->deadzonePercentage = &deadzonePercentage;
    manager->deadzoneType = &deadzoneType;
    manager->supportedControllerID = supportedControllerID;

    int returnVal;
    returnVal = pthread_attr_init(&runLoopPthread_attr);

    assert(!returnVal);

    returnVal = pthread_attr_setdetachstate(&runLoopPthread_attr, PTHREAD_CREATE_JOINABLE);

    assert(!returnVal);

    int threadError = pthread_create(&runLoopPthread_ID, &runLoopPthread_attr, &RunLoopThreadEntry, manager);

#endif
}

GReturn GeneralController::GetState(int _controllerIndex, int _inputCode, float& _outState)
{
	if (_controllerIndex < 0 || _controllerIndex > MAX_CONTROLLER_INDEX || _inputCode < 0 || _inputCode  >= MAX_GENENRAL_INPUTS)
		return INVALID_ARGUMENT;

    controllersMutex.lock();

	if (controllers[_controllerIndex].isConnected == 0)
	{
        controllersMutex.unlock();
		return FAILURE;
    }

	_outState = controllers[_controllerIndex].controllerInputs[(_inputCode)];

	controllersMutex.unlock();
	return SUCCESS;
}

GReturn GeneralController::IsConnected(int _controllerIndex, bool& _outIsConnected)
{
	if (_controllerIndex < 0 || _controllerIndex >= MAX_CONTROLLER_INDEX)
		return INVALID_ARGUMENT;

	controllersMutex.lock();

	_outIsConnected = controllers[_controllerIndex].isConnected == 0 ? false : true;

	controllersMutex.unlock();

	return SUCCESS;
}

GReturn GeneralController::GetMaxIndex(int &_outMax)
{
	_outMax = MAX_CONTROLLER_INDEX;
	return SUCCESS;
}

GReturn GeneralController::GetNumConnected(int &_outConnectedCount)
{
	_outConnectedCount = 0;
	controllersMutex.lock();

	for (unsigned int i = 0; i < MAX_CONTROLLER_INDEX; ++i)
	{
		if (controllers[i].isConnected)
			++_outConnectedCount;
	}

	controllersMutex.unlock();

	return SUCCESS;
}

GReturn GeneralController::SetDeadZone(GControllerDeadzoneTypes _type, float _deadzonePercentage)
{
	if (_deadzonePercentage > 1.0f || _deadzonePercentage < 0.0f)
		return INVALID_ARGUMENT;
    controllersMutex.lock();
	deadzoneType = _type;
	deadzonePercentage = _deadzonePercentage;
    controllersMutex.unlock();
	return SUCCESS;
}

GReturn GeneralController::StartVibration(float _pan, float _duration, float _strength, unsigned int _controllerIndex)
{
	return FEATURE_UNSUPPORTED;
}

GReturn GeneralController::IsVibrating(unsigned int _controllerIndex, bool& _outIsVibrating)
{
	return FEATURE_UNSUPPORTED;
}

GReturn GeneralController::StopVirbration(unsigned int _controllerIndex)
{
	return FEATURE_UNSUPPORTED;
}

GReturn GeneralController::StopAllVirbrations()
{
	return FEATURE_UNSUPPORTED;
}



GReturn GeneralController::RegisterListener(GListener* _addListener, unsigned long long _eventMask)
{
	if (_addListener == nullptr) {
		return INVALID_ARGUMENT;
	}

	listenerMutex.lock();

	std::pair<GListener*, unsigned long long> search(_addListener, _eventMask);
	std::vector<std::pair<GListener*, unsigned long long>>::const_iterator iter =
		find(listeners.begin(), listeners.end(), search);
	if (iter != listeners.end()) {
		listenerMutex.unlock();
		return REDUNDANT_OPERATION;
	}

	listeners.push_back(search);
	_addListener->IncrementCount();

	listenerMutex.unlock();

	return SUCCESS;
}

GReturn GeneralController::DeregisterListener(GListener* _removeListener)
{
	if (_removeListener == nullptr) {
		return INVALID_ARGUMENT;
	}

	listenerMutex.lock();

	std::pair<GListener*, unsigned long long> search(_removeListener, 0);
	std::vector<std::pair<GListener*, unsigned long long>>::const_iterator iter =
		find_if(listeners.begin(), listeners.end(),
			[&search](std::pair<GListener*, unsigned long long> const& elem) {
		return elem.first == search.first;
	});
	if (iter != listeners.end()) {
		iter->first->DecrementCount();
		listeners.erase(iter);
		listenerMutex.unlock();
	}
	else {
		listenerMutex.unlock();
		return FAILURE;
	}

	return SUCCESS;
}

GReturn GeneralController::GetCount(unsigned int& _outCount)
{
	_outCount = referenceCount;

	return SUCCESS;
}

GReturn GeneralController::IncrementCount()
{
	if (referenceCount == 0xFFFFFFFF)
		return FAILURE;

	referenceCount += 1;

	return SUCCESS;
}

GReturn GeneralController::DecrementCount()
{
	if (referenceCount == 0)
		return FAILURE;

	referenceCount -= 1;

	if (referenceCount == 0)
	{
		isRunning = false;
		#ifdef __linux__
		linuxInotifyThread->join();
		delete linuxInotifyThread;
		#endif
        #ifdef __APPLE__
        //[manager StopRunLoop];
        pthread_join(runLoopPthread_ID, NULL);

        #endif
		// handle destruction
		for (int i = 0; i < MAX_CONTROLLER_INDEX; ++i)
		{
			delete[] controllers[i].controllerInputs;
			delete controllers[i].vibrationStartTime;
		}
		delete[] controllers;

		delete this;
	}

	return SUCCESS;
}

GReturn GeneralController::RequestInterface(const GUUIID& _interfaceID, void** _outputInterface)
{
	if (_outputInterface == nullptr)
		return INVALID_ARGUMENT;

	if (_interfaceID == SYSTEM::GControllerUUIID)
	{
		GController* convert = reinterpret_cast<GController*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GBroadcastingUUIID)
	{
		GBroadcasting* convert = reinterpret_cast<GBroadcasting*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GMultiThreadedUUIID)
	{
		GMultiThreaded* convert = reinterpret_cast<GMultiThreaded*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else if (_interfaceID == GInterfaceUUIID)
	{
		GInterface* convert = reinterpret_cast<GInterface*>(this);
		convert->IncrementCount();
		(*_outputInterface) = convert;
	}
	else
		return INTERFACE_UNSUPPORTED;

	return SUCCESS;
}

void GeneralController::Linux_InitControllers()
{
#ifdef __linux__
    for(int i = 0; i < MAX_CONTROLLER_INDEX; ++i)
    {
        iscontrollerLoopRunning[i] = false;
    }

DIR *dir;
dirent *fileData;
GCONTROLLER_EVENT_DATA eventData;
std::vector<std::pair<GListener*, unsigned long long>>::iterator iter;

if ((dir = opendir("/dev/input")) != NULL) {
  /* print all the files and directories within directory */
  while ((fileData = readdir(dir)) != NULL)
  {
  //  printf ("%s\n", fileData->d_name);

    char newFile[30];
    strcpy(newFile, "/dev/input/");
    strcat(newFile, fileData->d_name);
    // check the type of file
    int evdevCheck = strncmp(fileData->d_name, "event",5);
    // check if file has a vaild number of inputs
    if (evdevCheck == 0)
    {
                        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int event_fd = open(newFile, O_RDONLY | O_NONBLOCK);
        if(event_fd > -1)
        {

            if(isGamepadBitSet(event_fd))
            {
                controllersMutex.lock();
                int controllerIndex = FindEmptyControllerIndex(MAX_CONTROLLER_INDEX, controllers);
                controllersMutex.unlock();
                if(controllerIndex != -1)
                {
                    //printf("Opening file %s\n", fileData->d_name);



                    input_id ID;
                    char name[20];
                    //ioctl(event_fd, EVIOCGID, ID);
                    ioctl(event_fd, EVIOCGNAME(20), name);

                    int controllerID;


                    if(strstr(name,"Sony") != nullptr)
                        controllerID = G_PS4_CONTROLLER;
                    else if (strstr(name, "Microsoft") != nullptr)
                        controllerID = G_XBOX_CONTROLLER;
                    else
                        controllerID = G_GENERAL_CONTROLLER;

                    if(supportedControllerID != G_GENERAL_CONTROLLER && controllerID != supportedControllerID)
                    {
                        continue;
                    }

                    controllersMutex.lock();
                    iscontrollerLoopRunning[controllerIndex] = true;
                    controllerFilePaths[controllerIndex] = fileData->d_name;
                    controllers[controllerIndex].controllerID = controllerID;
                    controllers[controllerIndex].isConnected = 1;
                    eventData.controllerID = controllers[controllerIndex].controllerID;
                    controllersMutex.unlock();
                    linuxControllerThreads[controllerIndex] = new std::thread(
                                                                            &GeneralController::Linux_ControllerInputLoop,
                                                                            this,
                                                                            newFile,
                                                                            controllerIndex,
                                                                            event_fd);
                    eventData.controllerIndex = controllerIndex;
                    eventData.inputCode = 0;
                    eventData.inputValue = 0;
                    eventData.isConnected = 1;

                    listenerMutex.lock();
                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                        iter->first->OnEvent(GControllerUUIID, CONTROLLERCONNECTED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                    listenerMutex.unlock();

                 }
             }

        }
        else
        {
           // printf("Could not open file %s\n", newFile);
        }
    }
  }
  closedir (dir);
}
#endif
}

void GeneralController::Linux_InotifyLoop()
{
#ifdef __linux__
    GCONTROLLER_EVENT_DATA eventData;
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter;
    int fd = 0;
    int wd = 0;
    struct G_inotify_event iev, base;
    int length = sizeof(struct G_inotify_event);
    base.len = 0;
    base.mask = 0;
    //auto lastCheck = std::chrono::high_resolution_clock::now();

    fd = inotify_init1(IN_NONBLOCK);
   // if(fd < 0)
   //     printf("ERROR");

    wd = inotify_add_watch(fd,"/dev/input", IN_CREATE | IN_DELETE);
  //  if(wd < 0)
  //      printf("ERROR");

    char filepath[] = "/dev/input";

    while(isRunning == true)
    {
        iev = base;
        int result = read(fd, &iev, length);
        if(result != -1)
		{

        if(iev.len)
        {
            //printf("new file %s\n", iev.name);
            if(iev.mask & IN_CREATE)
            {
                if(!(iev.mask & IN_ISDIR))
                {
                    char newFile[30];
                    strcpy(newFile, "/dev/input/");
                    strcat(newFile, iev.name);
                    // check the type of file
                    int evdevCheck = strncmp(iev.name, "event",5);
                    // check if file has a vaild number of inputs
              if (evdevCheck == 0)
                    {
                        sleep(1);
                        int event_fd = open(newFile, O_RDONLY | O_NONBLOCK);
                        if(event_fd > -1)
                        {
                            if(isGamepadBitSet(event_fd))
                            {
                                controllersMutex.lock();
                                int controllerIndex = FindEmptyControllerIndex(MAX_CONTROLLER_INDEX, controllers);
                                controllersMutex.unlock();
                                if(controllerIndex != -1)
                                {
                                    //printf("Opening File %s", iev.name);

                                    input_id ID;
                                    char name[20];
                                    //ioctl(event_fd, EVIOCGID, ID);
                                    ioctl(event_fd, EVIOCGNAME(20), name);

                                    int controllerID;


                                    if(strstr(name,"Sony") != nullptr)
                                        controllerID = G_PS4_CONTROLLER;
                                    else if (strstr(name, "Microsoft") != nullptr)
                                        controllerID = G_XBOX_CONTROLLER;
                                    else
                                        controllerID = G_GENERAL_CONTROLLER;

                                    if(supportedControllerID != G_GENERAL_CONTROLLER && controllerID != supportedControllerID)
                                    {
                                        continue;
                                    }

                                    controllersMutex.lock();
                                    controllers[controllerIndex].controllerID = controllerID;
                                    iscontrollerLoopRunning[controllerIndex] = true;
                                    controllerFilePaths[controllerIndex] = iev.name;
                                    controllers[controllerIndex].isConnected = 1;
                                    eventData.controllerID = controllers[controllerIndex].controllerID;
                                    controllersMutex.unlock();
                                    linuxControllerThreads[controllerIndex] = new std::thread(
                                                                                             &GeneralController::Linux_ControllerInputLoop,
                                                                                             this,
                                                                                             newFile,
                                                                                             controllerIndex,
                                                                                             event_fd);


                                    eventData.controllerIndex = controllerIndex;
                                    eventData.inputCode = 0;
                                    eventData.inputValue = 0;
                                    eventData.isConnected = 1;
                                    listenerMutex.lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                            iter->first->OnEvent(GControllerUUIID, CONTROLLERCONNECTED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    listenerMutex.unlock();
                                }
                            }

                        }
                        else
                        {
                           // printf("Could not open file %s", newFile);
                        }
                    }



                }
            }
            else if(iev.mask & IN_DELETE)
            {
                if(!(iev.mask & IN_ISDIR))
                    {
                        for(int controllerIndex = 0; controllerIndex < MAX_CONTROLLER_INDEX; ++controllerIndex)
                        {
                            int result = strncmp(controllerFilePaths[controllerIndex], iev.name, 8);
                            if(result == 0)
                            {
                                iscontrollerLoopRunning[controllerIndex] = false;
                                linuxControllerThreads[controllerIndex]->join();
                                delete linuxControllerThreads[controllerIndex];
                            }
                        }
                    }
            }

        }

        }
        else
        {
            sleep(1);
        }
    }
    close(fd);

    for (int i = 0; i < MAX_CONTROLLER_INDEX; ++i)
    {
        if(iscontrollerLoopRunning[i])
        {
            iscontrollerLoopRunning[i] = false;
            linuxControllerThreads[i]->join();
            delete linuxControllerThreads[i];
        }
    }
#endif


}

void GeneralController::Linux_ControllerInputLoop(char* _filePath, unsigned int _controllerIndex, int fd)
{
#ifdef __linux__
    input_event ev; // time value type code
    input_event base;
    base.value = 0;
    base.type = 0;
    base.code = 0;
    GCONTROLLER_EVENT_DATA eventData;
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter;
    int lastLX = 0, lastLY = 0, lastRX = 0, lastRY = 0, lastLT = 0, lastRT = 0;
    struct input_absinfo axisInfo;
    ioctl(fd, EVIOCGABS(ABS_X), &axisInfo); // reads max and min for abs axes



        while(iscontrollerLoopRunning[_controllerIndex])
        {
          ev = base;
          int result = read(fd, &ev, sizeof(struct input_event));
          if(result != -1)
           {

           switch(ev.type)
           {
           case EV_KEY:
           {
                 if(ev.code < BTN_SOUTH)
                    break;
                int inputCode = Linux_ControllerCodes[ev.code - BTN_SOUTH];

                 if(inputCode == G_UNKOWN_INPUT)
                    break;

                controllersMutex.lock();
                controllers[_controllerIndex].controllerInputs[inputCode] = ev.value;
                eventData.inputCode = inputCode;
                eventData.inputValue = controllers[_controllerIndex].controllerInputs[inputCode];
                eventData.controllerIndex = _controllerIndex;
                eventData.controllerID = controllers[_controllerIndex].controllerID;
                controllersMutex.unlock();

                listenerMutex.lock();
                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                listenerMutex.unlock();

                break;
            }
            case EV_ABS:
            {
                  switch(ev.code)
                  {
                    case ABS_X:
                    {
                        // leftX
                        if(ev.value != lastLX)
                        {
                         controllersMutex.lock();
                         float oldY = controllers[_controllerIndex].controllerInputs[G_LY_AXIS];
                            lastLX = ev.value;
                            DeadzoneCalculation(lastLX,
                                                lastLY,
											axisInfo.maximum,
											axisInfo.minimum,
											controllers[_controllerIndex].controllerInputs[G_LX_AXIS],
											controllers[_controllerIndex].controllerInputs[G_LY_AXIS],
                                            deadzoneType,
                                            deadzonePercentage);

                            eventData.inputCode = G_LX_AXIS;
							eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_LX_AXIS];
							eventData.controllerIndex = _controllerIndex;
							eventData.controllerID = controllers[_controllerIndex].controllerID;
							controllersMutex.unlock();

							listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                            controllersMutex.lock();
                            controllers[_controllerIndex].controllerInputs[G_LY_AXIS] *= -1.0f; // to fix flipped value
                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_LY_AXIS];
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            eventData.controllerIndex = _controllerIndex;
                            controllersMutex.unlock();

                            if(oldY != eventData.inputValue)
                            {
                                // Send LY event
                                eventData.inputCode = G_LY_AXIS;

                                listenerMutex.lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                listenerMutex.unlock();
                            }

                         }
                        break;
                    }
                    case ABS_Y:
                    {
                    //leftY
                        if(ev.value != lastLY)
                         {
                            controllersMutex.lock();
                            lastLY = ev.value; // evdev values for Y are flipped
                            float oldX = controllers[_controllerIndex].controllerInputs[G_LX_AXIS];
                            DeadzoneCalculation(lastLX,
                                                lastLY,
											axisInfo.maximum,
											axisInfo.minimum,
											controllers[_controllerIndex].controllerInputs[G_LX_AXIS],
											controllers[_controllerIndex].controllerInputs[G_LY_AXIS],
                                            deadzoneType,
                                            deadzonePercentage);

							// Send LY event
							controllers[_controllerIndex].controllerInputs[G_LY_AXIS] *= -1.0f; // to fix flipped value
                            eventData.inputCode = G_LY_AXIS;
							eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_LY_AXIS];
							eventData.controllerIndex = _controllerIndex;
							eventData.controllerID = controllers[_controllerIndex].controllerID;
							controllersMutex.unlock();

							listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                            controllersMutex.lock();
                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_LX_AXIS];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();

                            if(oldX != eventData.inputValue)
                            {
                                // Send LX event
                                eventData.inputCode = G_LX_AXIS;
                                listenerMutex.lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                listenerMutex.unlock();
                            }

                         }
                        break;
                    }
                    case ABS_Z:
                    case ABS_BRAKE: // xboxdrv send abs_brake events rather than abs_z's
                    {
                        // left trigger
                        if(ev.value != lastLT)
                        {
                            controllersMutex.lock();
                            lastLT = ev.value;
                            float oldAxis = controllers[_controllerIndex].controllerInputs[G_LEFT_TRIGGER_AXIS];
                            if(ev.value > GENENRAL_TRIGGER_THRESHOLD)
                            {

                                controllers[_controllerIndex].controllerInputs[G_LEFT_TRIGGER_AXIS] = ev.value / GENENRAL_TRIGGER_THRESHOLD;

                            }
                            else
                            {
                                controllers[_controllerIndex].controllerInputs[G_LEFT_TRIGGER_AXIS] = 0;
                            }


                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_LEFT_TRIGGER_AXIS];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();
                            if(oldAxis != eventData.inputValue)
                            {
                                eventData.inputCode = G_LEFT_TRIGGER_AXIS;

                               listenerMutex.lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                listenerMutex.unlock();
                            }

                        }

                        break;
                    }

                    case  ABS_RX:
                    {
                        // RightX
                         if(ev.value != lastRX)
                         {
                         controllersMutex.lock();
                         float oldY = controllers[_controllerIndex].controllerInputs[G_RY_AXIS];
                            lastRX = ev.value;
                            DeadzoneCalculation(lastRX,
                                                lastRY,
											axisInfo.maximum,
											axisInfo.minimum,
											controllers[_controllerIndex].controllerInputs[G_RX_AXIS],
											controllers[_controllerIndex].controllerInputs[G_RY_AXIS],
											deadzoneType,
											deadzonePercentage);

                            eventData.inputCode = G_RX_AXIS;
							eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_RX_AXIS];
							eventData.controllerIndex = _controllerIndex;
							eventData.controllerID = controllers[_controllerIndex].controllerID;
							controllersMutex.unlock();

							listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                            controllersMutex.lock();
                            controllers[_controllerIndex].controllerInputs[G_RY_AXIS] *= -1.0f; // to fix flipped value
                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_RY_AXIS];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();

                            if(oldY != eventData.inputValue)
                            {
                                // Send LY event
                                eventData.inputCode = G_RY_AXIS;
                                listenerMutex.lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                listenerMutex.unlock();
                            }

                         }
                        break;
                    }
                    case ABS_RY:
                    {
                    //leftY
                        if(ev.value != lastRY)
                         {
                            controllersMutex.lock();
                            lastRY = ev.value; // evdev values for y are flipped.
                            float oldX = controllers[_controllerIndex].controllerInputs[G_RX_AXIS];
                            DeadzoneCalculation(lastRX,
                                                lastRY,
											axisInfo.maximum,
											axisInfo.minimum,
											controllers[_controllerIndex].controllerInputs[G_RX_AXIS],
											controllers[_controllerIndex].controllerInputs[G_RY_AXIS],
											deadzoneType,
											deadzonePercentage);

							// Send LY event
							controllers[_controllerIndex].controllerInputs[G_RY_AXIS] *= -1.0f; // to fix flipped value
                            eventData.inputCode = G_RY_AXIS;
							eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_RY_AXIS];
							eventData.controllerIndex = _controllerIndex;
							eventData.controllerID = controllers[_controllerIndex].controllerID;
							controllersMutex.unlock();

							listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                            controllersMutex.lock();
                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_RX_AXIS];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();

                            if(oldX != eventData.inputValue)
                            {
                                // Send LX event
                                eventData.inputCode = G_RX_AXIS;
                                listenerMutex.lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                listenerMutex.unlock();
                            }
                         }
                        break;
                    }
                    case ABS_RZ:
                    case ABS_GAS: // xboxdrv send abs_gas events rather than abs_rz's
                    {
                        // left trigger
                        if(ev.value != lastRT)
                        {
                            controllersMutex.lock();
                            lastLT = ev.value;
                            float oldAxis = controllers[_controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS];
                            if(ev.value > GENENRAL_TRIGGER_THRESHOLD)
                            {

                                controllers[_controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS] = ev.value / GENENRAL_TRIGGER_THRESHOLD;

                            }
                            else
                            {
                                controllers[_controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS] = 0;
                            }

                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();
                            if(oldAxis != controllers[_controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS])
                            {
                                eventData.inputCode = G_RIGHT_TRIGGER_AXIS;

                                listenerMutex.lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                listenerMutex.unlock();
                            }

                        }

                        break;
                    }
                    case ABS_HAT0X:
                    case ABS_HAT3X:
                    {

                        // DPAD HORZONTAL
                        if(ev.value == 1)
                        {
                            controllersMutex.lock();
                            controllers[_controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 1;
                            eventData.inputCode = G_DPAD_RIGHT_BTN;
                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();

                            listenerMutex.lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                        }
                        else if(ev.value == -1)
                        {
                            controllersMutex.lock();
                            controllers[_controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 1;
                            eventData.inputCode = G_DPAD_LEFT_BTN;
                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();

                            listenerMutex.lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                        }
                        else if (ev.value == 0)
                        {
                            controllersMutex.lock();
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            eventData.controllerIndex = _controllerIndex;

                            if(controllers[_controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] != 0)
                            {

                                controllers[_controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 0;
                                eventData.inputCode = G_DPAD_LEFT_BTN;
                                eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            }

                            if(controllers[_controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] != 0)
                            {
                                controllers[_controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 0;
                                eventData.inputCode = G_DPAD_RIGHT_BTN;
                                eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];

                            }
                            controllersMutex.unlock();

                            listenerMutex.lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                        }

                        break;
                    }
                     case ABS_HAT0Y:
                     case ABS_HAT3Y:
                    {

                        // DPAD VERT
                        if(ev.value == 1)
                        {
                            controllersMutex.lock();
                            controllers[_controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 1;
                            eventData.inputCode = G_DPAD_DOWN_BTN;
                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();

                            listenerMutex.lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                        }
                        else if(ev.value == -1)
                        {
                            controllersMutex.lock();
                            controllers[_controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 1;
                            eventData.inputCode = G_DPAD_UP_BTN;
                            eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            eventData.controllerIndex = _controllerIndex;
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            controllersMutex.unlock();

                            listenerMutex.lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                        }
                        else if (ev.value == 0)
                        {
                            controllersMutex.lock();
                            eventData.controllerID = controllers[_controllerIndex].controllerID;
                            eventData.controllerIndex = _controllerIndex;
                            if(controllers[_controllerIndex].controllerInputs[G_DPAD_UP_BTN] != 0)
                            {
                                controllers[_controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 0;
                                eventData.inputCode = G_DPAD_UP_BTN;
                                eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            }

                            if(controllers[_controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] != 0)
                            {
                                controllers[_controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 0;
                                eventData.inputCode = G_DPAD_DOWN_BTN;
                                eventData.inputValue = controllers[_controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            }
                             controllersMutex.unlock();

                                listenerMutex.lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();

                        }

                        break;
                    }
                  }
                break;
            }

            }
            }
            else
            {
                if(errno == EAGAIN)
                    sleep(.001);
                else
                    break;
            }

           }


        close(fd);

        controllersMutex.lock();
        controllers[_controllerIndex].isConnected = 0;
        controllersMutex.unlock();


        eventData.controllerIndex = _controllerIndex;
        eventData.inputCode = 0;
        eventData.inputValue = 0;
        eventData.isConnected = 0;
        listenerMutex.lock();
        for (iter = listeners.begin(); iter != listeners.end(); ++iter)
            iter->first->OnEvent(GControllerUUIID, CONTROLLERDISCONNECTED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
        listenerMutex.unlock();

#endif //__linux__
}


// XboxController

XboxController::XboxController()
{

	for (int i = 0; i < 4; ++i)
		XControllerSlotIndices[i] = -1;
}
XboxController::~XboxController()
{

}

void XboxController::Init()
{
	controllers = new CONTROLLER_STATE[MAX_XBOX_CONTROLLER_INDEX];
	deadzoneType = DEADZONESQUARE;
	deadzonePercentage = .2f;
	for (unsigned int i = 0; i < MAX_XBOX_CONTROLLER_INDEX; ++i)
	{
		controllers[i].isConnected = 0;
		controllers[i].isVibrating = 0;
		controllers[i].vibrationDuration = 0;
		controllers[i].vibrationStartTime = new std::chrono::high_resolution_clock::time_point();
		controllers[i].maxInputs = MAX_XBOX_INPUTS;
		controllers[i].controllerInputs = new float[MAX_XBOX_INPUTS];
	}

    //controllersMutex = std::unique_lock<std::mutex>(controllersMutex, std::defer_lock);
   // listenerMutex = std::unique_lock<std::mutex>(listenerMutex, std::defer_lock);
#ifdef _WIN32
	xInputThread = new std::thread(&XboxController::XinputLoop, this);
	//controllerVibThreads = new std::thread(&XboxController::XinputVibration, this);
#endif // _WIN32
}

GReturn XboxController::GetMaxIndex(int &_outMax)
{
	_outMax = MAX_XBOX_CONTROLLER_INDEX;
	return SUCCESS;
}

GReturn XboxController::GetState(int _controllerIndex, int _inputCode, float& _outState)
{
	if (_controllerIndex < 0 || _controllerIndex > MAX_XBOX_CONTROLLER_INDEX ||  _inputCode < 0 || _inputCode  > 19)
		return INVALID_ARGUMENT;
	if (controllers[_controllerIndex].isConnected == 0)
		return FAILURE;

	_outState = controllers[_controllerIndex].controllerInputs[_inputCode];

	return SUCCESS;
}

GReturn XboxController::GetNumConnected(int &_outConnectedCount)
{
	_outConnectedCount = 0;
	controllersMutex.lock();

	for (unsigned int i = 0; i < MAX_XBOX_CONTROLLER_INDEX; ++i)
	{
		if (controllers[i].isConnected)
			++_outConnectedCount;
	}
	controllersMutex.unlock();
	return SUCCESS;
}

GReturn XboxController::IsConnected(int _controllerIndex, bool& _outIsConnected)
{
	if (_controllerIndex < 0 || _controllerIndex >= MAX_XBOX_CONTROLLER_INDEX)
		return INVALID_ARGUMENT;

	controllersMutex.lock();

	_outIsConnected = controllers[_controllerIndex].isConnected == 0 ? false : true;

	controllersMutex.unlock();

	return SUCCESS;
}

void XboxController::XinputVibration()
{
#ifdef _WIN32
	auto vibStart = std::chrono::high_resolution_clock::now();
	std::chrono::milliseconds deltaTime;

	while (isRunning)
	{
		controllersMutex.lock();
		for (int i = 0; i < 4; ++i)
		{

			if (controllers[XControllerSlotIndices[i]].isConnected && controllers[XControllerSlotIndices[i]].isVibrating)
			{
				//controllers[XControllerSlotIndices[i]].vibrationDuration -= (deltaTime.count() / 1000.f);
				if (controllers[XControllerSlotIndices[i]].vibrationDuration <= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - vibStart).count()/1000.0f)
				{
					XINPUT_VIBRATION vibrationState;
					vibrationState.wLeftMotorSpeed = 0;
					vibrationState.wRightMotorSpeed = 0;
					controllers[XControllerSlotIndices[i]].isVibrating = 0;
					controllers[XControllerSlotIndices[i]].vibrationDuration = 0.0f;
					XInputSetState(i, &vibrationState);
				}
			}
		}
		controllersMutex.unlock();
	}
#endif
}

GReturn XboxController::StartVibration(float _pan, float _duration, float _strength, unsigned int _controllerIndex)
{
	if ((_controllerIndex > MAX_XBOX_CONTROLLER_INDEX || _controllerIndex < 0)
		|| (_pan < -1.0f || _pan > 1.0f)
		|| _duration < 0.0f
		|| (_strength < -1.0f || _strength > 1.0f) )
		return INVALID_ARGUMENT;

	controllersMutex.lock();

	if (controllers[_controllerIndex].isVibrating)
	{
		controllersMutex.unlock();
		return FAILURE;
	}

#ifdef _WIN32
	XINPUT_VIBRATION vibrationState;
	unsigned int vibrationStrength = XINPUT_MAX_VIBRATION * _strength;
	vibrationState.wLeftMotorSpeed  = vibrationStrength * ( .5f + (.5f * (-1 *_pan)));
	vibrationState.wRightMotorSpeed = vibrationStrength * (.5f + (.5f * _pan));

	for (int i = 0; i < 4; ++i)
	{
		if (_controllerIndex == XControllerSlotIndices[i])
		{

			controllers[i].isVibrating = 1;
			controllers[i].vibrationDuration = _duration;
			*controllers[i].vibrationStartTime = std::chrono::high_resolution_clock::now();
			XInputSetState(i, &vibrationState);
			break;
		}
	}
	//controllerVibThreads[i] = new std::thread(&XboxController::XinputVibration, this);




#endif // _WIN32

	controllersMutex.unlock();
	return SUCCESS;
}
GReturn XboxController::IsVibrating(unsigned int _controllerIndex, bool& _outIsVibrating)
{
	if ((_controllerIndex > MAX_XBOX_CONTROLLER_INDEX || _controllerIndex < 0))
		return INVALID_ARGUMENT;

	controllersMutex.lock();

	_outIsVibrating = controllers[_controllerIndex].isVibrating == 0 ? false : true;

	controllersMutex.unlock();

	return SUCCESS;
}
GReturn XboxController::StopVirbration(unsigned int _controllerIndex)
{
	if ((_controllerIndex > MAX_XBOX_CONTROLLER_INDEX || _controllerIndex < 0))
		return INVALID_ARGUMENT;

	controllersMutex.lock();
	if (controllers[_controllerIndex].isVibrating == false)
	{
		controllersMutex.unlock();
		return REDUNDANT_OPERATION;
	}

#ifdef _WIN32
	XINPUT_VIBRATION vibrationState;
	vibrationState.wLeftMotorSpeed = 0;
	vibrationState.wRightMotorSpeed = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (_controllerIndex == XControllerSlotIndices[i])
		{
			controllers[i].isVibrating = 0;
			controllers[i].vibrationDuration = 0.0f;
			XInputSetState(i, &vibrationState);
			break;
		}
	}
#endif // _WIN32

	controllersMutex.unlock();
	return SUCCESS;
}
GReturn XboxController::StopAllVirbrations()
{
#ifdef _WIN32
	XINPUT_VIBRATION vibrationState;
	vibrationState.wLeftMotorSpeed = 0;
	vibrationState.wRightMotorSpeed = 0;

	controllersMutex.lock();

	for (int i = 0; i < MAX_XBOX_CONTROLLER_INDEX; ++i)
	{
		if (controllers[i].isVibrating)
		{
			controllers[i].isVibrating = 0;
			controllers[i].vibrationDuration = 0.0f;
			XInputSetState(i, &vibrationState);
		}
	}
	controllersMutex.unlock();

	return SUCCESS;
#else
    return FAILURE;
#endif // _WIN32
}

float XboxController::XboxDeadZoneCalc(float _value, bool _isTigger)
{
#ifdef _WIN32
	if (_isTigger)
	{
		if (std::abs(_value) > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			_value /= MAX_XBOX_TRIGGER_AXIS;
		else
			_value = 0;
	}
	else
	{
		if (std::abs(_value) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			_value /= MAX_XBOX_THUMB_AXIS;
		else
			_value = 0;
	}
	return _value;
#else
    return 0;
#endif // _WIN32
}

void XboxController::XinputLoop()
{
#ifdef _WIN32
	DWORD result;
	XINPUT_STATE controllerState;
	ZeroMemory(&controllerState, sizeof(XINPUT_STATE));
	GCONTROLLER_EVENT_DATA eventData;
	ZeroMemory(&eventData, sizeof(GCONTROLLER_EVENT_DATA));
	std::vector<std::pair<GListener*, unsigned long long>>::iterator iter;
	auto lastCheck = std::chrono::high_resolution_clock::now();
	bool isFirstLoop = true;
	CONTROLLER_STATE oldState;
	oldState.maxInputs = MAX_XBOX_INPUTS;
	oldState.controllerInputs = new float[MAX_XBOX_INPUTS];
	//std::chrono::milliseconds deltaTime;


	while (isRunning)
	{
		// *UPDATED* loop runs 100 times per second. may want to up this to highest refresh rate possible on a monitor
		if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastCheck).count() >= 10
			|| isFirstLoop)
		{
			lastCheck = std::chrono::high_resolution_clock::now();
			isFirstLoop = false;
			for (int i = 0; i < 4; ++i)
			{
				result = XInputGetState(i, &controllerState);
				if (result == ERROR_SUCCESS)
				{
					if (XControllerSlotIndices[i] < 0)
					{
						controllersMutex.lock();

						XControllerSlotIndices[i] = FindEmptyControllerIndex(MAX_XBOX_CONTROLLER_INDEX, controllers);
						controllers[XControllerSlotIndices[i]].isConnected = 1;

						controllersMutex.unlock();

						eventData.controllerIndex = XControllerSlotIndices[i];
						eventData.inputCode = 0;
						eventData.inputValue = 0;
						eventData.isConnected = 1;
						eventData.controllerID = G_XBOX_CONTROLLER;
                        listenerMutex.lock();
						for (iter = listeners.begin(); iter != listeners.end(); ++iter)
							iter->first->OnEvent(GControllerUUIID, CONTROLLERCONNECTED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                        listenerMutex.unlock();
					}


					if (XControllerSlotIndices[i] >= 0 && controllerState.dwPacketNumber != XControllerLastPacket[i]) // add checks for events
					{
						CopyControllerState(&controllers[XControllerSlotIndices[i]], &oldState);
						eventData.isConnected = 1;
						eventData.controllerIndex = XControllerSlotIndices[i];


						XControllerLastPacket[i] = controllerState.dwPacketNumber;

						// Buttons

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_SOUTH_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_SOUTH_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_SOUTH_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_SOUTH_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_EAST_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_EAST_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_EAST_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_EAST_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}
						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_NORTH_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_NORTH_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_NORTH_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_NORTH_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}
						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_WEST_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_WEST_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_WEST_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_WEST_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_LEFT_SHOULDER_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_LEFT_SHOULDER_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_LEFT_SHOULDER_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_LEFT_SHOULDER_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_RIGHT_SHOULDER_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_RIGHT_SHOULDER_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_RIGHT_SHOULDER_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_RIGHT_SHOULDER_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_DPAD_LEFT_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_DPAD_LEFT_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_DPAD_LEFT_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_DPAD_LEFT_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_DPAD_RIGHT_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_DPAD_RIGHT_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_DPAD_RIGHT_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_DPAD_RIGHT_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_DPAD_UP_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_DPAD_UP_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_DPAD_UP_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_DPAD_UP_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_DPAD_DOWN_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_DPAD_DOWN_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_DPAD_DOWN_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_DPAD_DOWN_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_LEFT_THUMB_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_LEFT_THUMB_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_LEFT_THUMB_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_LEFT_THUMB_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_RIGHT_THUMB_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_RIGHT_THUMB_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_RIGHT_THUMB_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_RIGHT_THUMB_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_START_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_START_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_START_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_START_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0 ? 1.0f : 0.0f) != oldState.controllerInputs[G_SELECT_BTN])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_SELECT_BTN] = (controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0 ? 1.0f : 0.0f;
							eventData.inputCode = G_SELECT_BTN;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_SELECT_BTN];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}
						// AXES
						if (XboxDeadZoneCalc(controllerState.Gamepad.bLeftTrigger, true) != oldState.controllerInputs[G_LEFT_TRIGGER_AXIS])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_LEFT_TRIGGER_AXIS] = XboxDeadZoneCalc(controllerState.Gamepad.bLeftTrigger, true);
							eventData.inputCode = G_LEFT_TRIGGER_AXIS;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_LEFT_TRIGGER_AXIS];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}
						if (XboxDeadZoneCalc(controllerState.Gamepad.bRightTrigger, true) != oldState.controllerInputs[G_RIGHT_TRIGGER_AXIS])
						{
							controllersMutex.lock();
							controllers[XControllerSlotIndices[i]].controllerInputs[G_RIGHT_TRIGGER_AXIS] = XboxDeadZoneCalc(controllerState.Gamepad.bRightTrigger, true);
							eventData.inputCode = G_RIGHT_TRIGGER_AXIS;
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_RIGHT_TRIGGER_AXIS];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}
						controllersMutex.lock();
						DeadzoneCalculation(controllerState.Gamepad.sThumbLX,
											controllerState.Gamepad.sThumbLY,
											MAX_XBOX_THUMB_AXIS,
											MIN_XBOX_THUMB_AXIS,
											controllers[XControllerSlotIndices[i]].controllerInputs[G_LX_AXIS],
											controllers[XControllerSlotIndices[i]].controllerInputs[G_LY_AXIS],
											deadzoneType,
											deadzonePercentage);

						DeadzoneCalculation(controllerState.Gamepad.sThumbRX,
							controllerState.Gamepad.sThumbRY,
							MAX_XBOX_THUMB_AXIS,
							MIN_XBOX_THUMB_AXIS,
							controllers[XControllerSlotIndices[i]].controllerInputs[G_RX_AXIS],
							controllers[XControllerSlotIndices[i]].controllerInputs[G_RY_AXIS],
							deadzoneType,
							deadzonePercentage);

						float newLX = controllers[XControllerSlotIndices[i]].controllerInputs[G_LX_AXIS];
						float newLY = controllers[XControllerSlotIndices[i]].controllerInputs[G_LY_AXIS];
						float newRX = controllers[XControllerSlotIndices[i]].controllerInputs[G_RX_AXIS];
						float newRY = controllers[XControllerSlotIndices[i]].controllerInputs[G_RY_AXIS];

						controllersMutex.unlock();

						if (newLX != oldState.controllerInputs[G_LX_AXIS])
						{
							eventData.inputCode = G_LX_AXIS;
							controllersMutex.lock();
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_LX_AXIS];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}
						if (newLY != oldState.controllerInputs[G_LY_AXIS])
						{
							eventData.inputCode = G_LY_AXIS;
							controllersMutex.lock();
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_LY_AXIS];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (newRX != oldState.controllerInputs[G_RX_AXIS])
						{
							eventData.inputCode = G_RX_AXIS;
							controllersMutex.lock();
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_RX_AXIS];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}

						if (newRY != oldState.controllerInputs[G_RY_AXIS])
						{
							eventData.inputCode = G_RY_AXIS;
							controllersMutex.lock();
							eventData.inputValue = controllers[XControllerSlotIndices[i]].controllerInputs[G_RY_AXIS];
							controllersMutex.unlock();

                            listenerMutex.lock();
							for (iter = listeners.begin(); iter != listeners.end(); ++iter)
								iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            listenerMutex.unlock();
						}


					}
				}
				else // no controller connected
				{
					if (XControllerSlotIndices[i] >= 0)
					{
						//call event
						controllers[XControllerSlotIndices[i]].isConnected = 0;
						controllers[XControllerSlotIndices[i]].isVibrating = 0;
						controllers[XControllerSlotIndices[i]].vibrationDuration = 0.0f;
						eventData.controllerIndex = XControllerSlotIndices[i];
						eventData.inputCode = 0;
						eventData.inputValue = 0;
						eventData.isConnected = 0;
						eventData.controllerID = G_XBOX_CONTROLLER;
                        listenerMutex.lock();
						for (iter = listeners.begin(); iter != listeners.end(); ++iter)
							iter->first->OnEvent(GControllerUUIID, CONTROLLERDISCONNECTED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                        listenerMutex.unlock();

						XControllerSlotIndices[i] = -1;

					}
				}

				if (XControllerSlotIndices[i] >= 0 && controllers[XControllerSlotIndices[i]].isVibrating)
				{
					if (controllers[XControllerSlotIndices[i]].vibrationDuration <=
						(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() -
							*controllers[XControllerSlotIndices[i]].vibrationStartTime).count() *.001f))
					{
						XINPUT_VIBRATION vibrationState;
						vibrationState.wLeftMotorSpeed = 0;
						vibrationState.wRightMotorSpeed = 0;
						controllers[XControllerSlotIndices[i]].isVibrating = 0;
						controllers[XControllerSlotIndices[i]].vibrationDuration = 0.0f;
						XInputSetState(i, &vibrationState);
					}
				}
			}
		}
		else // This thread cannot be left to run rampant
		{
			// The following is not ideal, instead we should consider creating a Gateware thread pool in G_System
			// This thread pool would eliminate spinloop "solutions" and have built in HRZ timing for running ops
			std::this_thread::yield(); // give up time slice
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	delete[] oldState.controllerInputs;
#endif // _WIN32
}

GReturn XboxController::DecrementCount()
{
	if (referenceCount == 0)
		return FAILURE;

	referenceCount -= 1;

	if (referenceCount == 0)
	{
		// handle destruction
		isRunning = false;
		xInputThread->join();
		delete xInputThread;
		//controllerVibThreads->join();
		//delete controllerVibThreads;

		for (int i = 0; i < MAX_XBOX_CONTROLLER_INDEX; ++i)
		{
			delete[] controllers[i].controllerInputs;
			delete controllers[i].vibrationStartTime;

		}
		delete[] controllers;
		delete this;
	}

	return SUCCESS;
}
