#ifndef GCONTROLLER_CALLBACK
#define GCONTROLLER_CALLBACK
#include "../../Interface/G_System/GController.h"
#include "GController_Static.h"
#include "GUtility.h"
#include <mutex>
#include <atomic>
#include <vector>
#include <algorithm>

#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <chrono>
#include <cmath>
#include<dirent.h>
#include <unistd.h>
#endif // __linux__

#ifdef __APPLE__
#include <cmath>
#endif



using namespace GW;
using namespace CORE;
using namespace SYSTEM;



#ifdef __APPLE__
@class HIDMANAGER;
#endif


namespace
{


        //! Map of Listeners to send event information to.
        std::vector<std::pair<GListener *, unsigned long long>> listeners;


        struct CONTROLLER_STATE
        {
            int isConnected;
            int isVibrating;
            float vibrationDuration;
            std::chrono::high_resolution_clock::time_point* vibrationStartTime;
            int maxInputs; // Hold the size of controllerInputs array
            float* controllerInputs; // controllerInputs is used to hold an array for the input values of the controller
            int controllerID;
            #ifdef __APPLE__
            IOHIDDeviceRef device;
            #endif
        };


        // Modified verion of the struct from inotify.h to pass the catch tests
        // Due to the __flexarr hack catch was throwing an error saying the buffer for name was overflowed
        struct G_inotify_event
        {
            int wd;        /* Watch descriptor.  */
            uint32_t mask;    /* Watch mask.  */
            uint32_t cookie;    /* Cookie to synchronize two events.  */
            uint32_t len;        /* Length (including NULs) of name.  */
            char name[16];    /* Name.  */
        };

        // This function does not lock before using _controllers
        unsigned int FindEmptyControllerIndex(unsigned int _maxIndex, const CONTROLLER_STATE* _controllers)
        {
            for (unsigned int i = 0; i < _maxIndex; ++i)
            {
                if (_controllers[i].isConnected == 0)
                    return i;
            }
            return -1;
        }

        // prehaps make return vaule GRETURN
        CONTROLLER_STATE* CopyControllerState(const CONTROLLER_STATE* _stateToCopy, CONTROLLER_STATE* _outCopy)
        {
            if (_stateToCopy->maxInputs == _outCopy->maxInputs)
                for (int i = 0; i < _outCopy->maxInputs; ++i)
                {
                    _outCopy->controllerInputs[i] = _stateToCopy->controllerInputs[i];
                }
            else
                _outCopy = nullptr;

            return _outCopy;
        }

    void DeadzoneCalculation(float _x, float _y, float _axisMax, float _axisMin, float &_outX, float &_outY, int _deadzoneType, float _deadzonePercentage)
    {

        float range = _axisMax - _axisMin;
        _outX = (((_x - _axisMin) * 2) / range) - 1;
        _outY = (((_y - _axisMin) * 2) / range) - 1;
        float liveRange = 1.0f - _deadzonePercentage;
        if (_deadzoneType == DEADZONESQUARE)
        {
            if (std::abs(_outX) <= _deadzonePercentage)
                _outX = 0.0f;
            if (std::abs(_outY) <= _deadzonePercentage)
                _outY = 0.0f;

            if (_outX > 0.0f)
                _outX = (_outX - _deadzonePercentage) / liveRange;
            else if(_outX < 0.0f)
                _outX = (_outX + _deadzonePercentage) / liveRange;
            if (_outY > 0.0f)
                _outY = (_outY - _deadzonePercentage) / liveRange;
            else if (_outY < 0.0f)
                _outY = (_outY + _deadzonePercentage) / liveRange;
        }
        else
        {
            float mag = std::sqrt(_outX * _outX + _outY * _outY);
            mag = (mag - _deadzonePercentage) / liveRange;
            _outX *= mag;
            _outY *= mag;
            
            if(std::abs(_outX) <= _deadzonePercentage)
                _outX = 0.0f;
            if(std::abs(_outY) <= _deadzonePercentage)
                _outY = 0.0f;
        }

    }

#ifdef __linux__


        // This functions gets an array of bits representing the keys supported for the device and check if BTN_GAMEPAD/BTN_SOUTH is set
        bool isGamepadBitSet(int _event_fd)
        {
            // keys' size is based on how many sets of 64 bits it would take to cover all of the diffrent types of keys where each key is a bit
            unsigned long keys[(KEY_CNT + 64 - 1)/64];
            // EXVIOCGBIT gets the keys supported by the device returned as an array of 64 bits per index
            // BTN_GAMEPAD represents the bit we want to check so we find where the bit would be located in the array
            // and in the current 64 bits
            ioctl(_event_fd, EVIOCGBIT(EV_KEY, sizeof(keys)), keys);
            return (keys[BTN_GAMEPAD / 64] & (1LL << (BTN_GAMEPAD % 64))) != 0 ? true : false;
        }
#endif

#ifdef __APPLE__

    // This Function creates and return an array of dictionarys used to match connected devices
        static CFMutableArrayRef CreateHIDManagerCriteria()
        {

            // create a dictionary to add usage page/usages to
            UInt32 usagePage = kHIDPage_GenericDesktop;
            UInt32 joyUsage = kHIDUsage_GD_Joystick;
            UInt32 gamepadUsage = kHIDUsage_GD_GamePad;
            UInt32 mutiaxisUsage = kHIDUsage_GD_MultiAxisController;

            CFMutableDictionaryRef joyDictionary = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

            CFMutableDictionaryRef gamepadDictionary = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

            CFMutableDictionaryRef multiaxisDictionary = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

            CFNumberRef pageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePage);

            CFDictionarySetValue(joyDictionary,CFSTR(kIOHIDDeviceUsagePageKey), pageCFNumberRef);
            CFDictionarySetValue(gamepadDictionary,CFSTR(kIOHIDDeviceUsagePageKey), pageCFNumberRef);
            CFDictionarySetValue(multiaxisDictionary,CFSTR(kIOHIDDeviceUsagePageKey), pageCFNumberRef);
            CFRelease(pageCFNumberRef);

            CFNumberRef usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &joyUsage);
            CFDictionarySetValue(joyDictionary, CFSTR(kIOHIDDeviceUsageKey), usageCFNumberRef);

            usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &gamepadUsage);
            CFDictionarySetValue(gamepadDictionary, CFSTR(kIOHIDDeviceUsageKey), usageCFNumberRef);

            usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &mutiaxisUsage);
            CFDictionarySetValue(multiaxisDictionary, CFSTR(kIOHIDDeviceUsageKey), usageCFNumberRef);
            CFRelease(usageCFNumberRef);

            CFMutableArrayRef dictionaryCFArr = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);

            CFArrayAppendValue(dictionaryCFArr, joyDictionary);
            CFArrayAppendValue(dictionaryCFArr, gamepadDictionary);
            CFArrayAppendValue(dictionaryCFArr, multiaxisDictionary);

            CFRelease(joyDictionary);
            CFRelease(gamepadDictionary);
            CFRelease(multiaxisDictionary);

            return dictionaryCFArr;

        }


    static void* RunLoopThreadEntry(void* manager_ptr)
    {
        HIDMANAGER* manager = static_cast<HIDMANAGER*>(manager_ptr);
        [manager InitManagerAndRunLoop]; // warning due to forward declartion

        return NULL;
    }


#endif




}//end

#ifdef __APPLE__


@interface HIDMANAGER : NSObject // HidManager wrapper class
{
    @public
    // these are to point to the GController varibles
    std::mutex* controllersMutex;
    std::mutex* listenerMutex;
    std::atomic<bool>* isRunning;
    CONTROLLER_STATE* controllers;
    GControllerDeadzoneTypes* deadzoneType;
    float* deadzonePercentage;
    int supportedControllerID;
    CFRunLoopRef managerRunLoop;

    int LX, LY, LZ, RX, RY, RZ;
}
-(void) InitManagerAndRunLoop;

@end

#endif


#endif
