#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <IOKit/hid/IOHIDLib.h>



#include "../../Source/G_System/GController_Callback.hpp"
static void Handle_IOHIDDeviceInputValueCallback(
                                                 
                                                 void *          inContext,      // context from IOHIDDeviceRegisterInputValueCallback
                                                 
                                                 IOReturn        inResult,       // completion result for the input value operation
                                                 
                                                 void *          inSender,       // IOHIDDeviceRef of the device this element is from
                                                 
                                                 IOHIDValueRef   inIOHIDValueRef // the new element value

) {
    double_t scaledValue = IOHIDValueGetScaledValue(inIOHIDValueRef,
                                                    kIOHIDValueScaleTypePhysical);
    
    IOHIDElementRef tIOHIDElementRef = IOHIDValueGetElement(inIOHIDValueRef);
    IOHIDElementType type = IOHIDElementGetType(tIOHIDElementRef);
    uint32_t usage = IOHIDElementGetUsage(tIOHIDElementRef);
    HIDMANAGER* manager = (__bridge HIDMANAGER*)inContext;
    GCONTROLLER_EVENT_DATA eventData;
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter;
    int controllerIndex = 0;
    bool controllerFound = false;
    for(; controllerIndex < MAX_CONTROLLER_INDEX; ++controllerIndex)
    {
        if(manager->controllers[controllerIndex].device == (IOHIDDeviceRef)inSender)
        {
            controllerFound = true;
            break;
        }
    }
    
    if(controllerFound)
    {
         int inputCode = Mac_ControllerCodes[usage][manager->controllers[controllerIndex].controllerID];
         // switch on input type
        if(inputCode == G_UNKOWN_INPUT)
            return;
        
        if(type == kIOHIDElementTypeInput_Button)
        {
           // swap 2 with controller id stored in controllers
           manager->controllersMutex->lock();
           manager->controllers[controllerIndex].controllerInputs[inputCode] = (float)scaledValue;
           eventData.inputCode = inputCode;
           eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[inputCode];
           eventData.isConnected = 1;
           eventData.controllerID = manager->controllers[controllerIndex].controllerID;
           eventData.controllerIndex = controllerIndex;
           manager->controllersMutex->unlock();
        
          manager->listenerMutex->lock();
          for (iter = listeners.begin(); iter != listeners.end(); ++iter)
              iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData,  sizeof(GCONTROLLER_EVENT_DATA));
          manager->listenerMutex->unlock();
        }
        else if( type == kIOHIDElementTypeInput_Misc)
        {
            // get input code from array then switch base on the code to determine how to proccess it
            switch (inputCode) {
                case G_LX_AXIS:
                    if(scaledValue != manager->LX)
                    {
                        manager->controllersMutex->lock();
                        float oldY = manager->controllers[controllerIndex].controllerInputs[G_LY_AXIS];
                        manager->LX = scaledValue;
                        DeadzoneCalculation(manager->LX,
                                            manager->LY,
                                            ControllerAxisRanges[MAX_AXIS][manager->controllers[controllerIndex].controllerID],
                                            ControllerAxisRanges[MIN_AXIS][manager->controllers[controllerIndex].controllerID],
                                            manager->controllers[controllerIndex].controllerInputs[G_LX_AXIS],
                                            manager->controllers[controllerIndex].controllerInputs[G_LY_AXIS],
                                            *(manager->deadzoneType),
                                            *(manager->deadzonePercentage));
                        
                        eventData.inputCode = G_LX_AXIS;
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_LX_AXIS];
                        manager->controllersMutex->unlock();
                        
                        manager->listenerMutex->lock();
                        for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                            iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                        manager->listenerMutex->unlock();
                        
                        manager->controllersMutex->lock();
                        manager->controllers[controllerIndex].controllerInputs[G_LY_AXIS] *= -1.0f; // to fix flipped value
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_LY_AXIS];
                        manager->controllersMutex->unlock();
                        
                        if(oldY != eventData.inputValue)
                        {
                            // Send LY event
                            eventData.inputCode = G_LY_AXIS;
                            
                            manager->listenerMutex->lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            manager->listenerMutex->unlock();
                        }
                        
                    }
                    break;
                case G_LY_AXIS:
                    //leftY
                    if(scaledValue != manager->LY)
                    {
                        manager->controllersMutex->lock();
                        manager->LY = scaledValue;
                        float oldX = manager->controllers[controllerIndex].controllerInputs[G_LX_AXIS];
                        DeadzoneCalculation(manager->LX,
                                            manager->LY,
                                            ControllerAxisRanges[MAX_AXIS][manager->controllers[controllerIndex].controllerID],
                                            ControllerAxisRanges[MIN_AXIS][manager->controllers[controllerIndex].controllerID],
                                            manager->controllers[controllerIndex].controllerInputs[G_LX_AXIS],
                                            manager->controllers[controllerIndex].controllerInputs[G_LY_AXIS],
                                            *(manager->deadzoneType),
                                            *(manager->deadzonePercentage));
                        
                        // Send LY event
                        manager->controllers[controllerIndex].controllerInputs[G_LY_AXIS] *= -1.0f; // to fix flipped value
                        eventData.inputCode = G_LY_AXIS;
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_LY_AXIS];
                        manager->controllersMutex->unlock();
                        
                        manager->listenerMutex->lock();
                        for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                            iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                        manager->listenerMutex->unlock();
                        
                        manager->controllersMutex->lock();
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_LX_AXIS];
                        manager->controllersMutex->unlock();
                        
                        if(oldX != eventData.inputValue)
                        {
                            // Send LX event
                            eventData.inputCode = G_LX_AXIS;
                            manager->listenerMutex->lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            manager->listenerMutex->unlock();
                        }
                        
                    }
                    break;
                case G_RX_AXIS:
                    if(scaledValue != manager->RX)
                    {
                        manager->controllersMutex->lock();
                        float oldY = manager->controllers[controllerIndex].controllerInputs[G_RY_AXIS];
                        manager->RX = scaledValue;
                        DeadzoneCalculation(manager->RX,
                                            manager->RY,
                                            ControllerAxisRanges[MAX_AXIS][manager->controllers[controllerIndex].controllerID],
                                            ControllerAxisRanges[MIN_AXIS][manager->controllers[controllerIndex].controllerID],
                                            manager->controllers[controllerIndex].controllerInputs[G_RX_AXIS],
                                            manager->controllers[controllerIndex].controllerInputs[G_RY_AXIS],
                                            *(manager->deadzoneType),
                                            *(manager->deadzonePercentage));
                        
                        eventData.inputCode = G_RX_AXIS;
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_RX_AXIS];
                        manager->controllersMutex->unlock();
                        
                        manager->listenerMutex->lock();
                        for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                            iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                        manager->listenerMutex->unlock();
                        
                        manager->controllersMutex->lock();
                        manager->controllers[controllerIndex].controllerInputs[G_RY_AXIS] *= -1.0f; // to fix flipped value
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_RY_AXIS];
                        manager->controllersMutex->unlock();
                        
                        if(oldY != eventData.inputValue)
                        {
                            // Send LY event
                            eventData.inputCode = G_RY_AXIS;
                            
                            manager->listenerMutex->lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            manager->listenerMutex->unlock();
                        }
                        
                    }
                    break;
                case G_RY_AXIS:
                    if(scaledValue != manager->RY)
                    {
                        manager->controllersMutex->lock();
                        manager->RY = scaledValue;
                        float oldX = manager->controllers[controllerIndex].controllerInputs[G_RX_AXIS];
                        DeadzoneCalculation(manager->RX,
                                            manager->RY,
                                            ControllerAxisRanges[MAX_AXIS][manager->controllers[controllerIndex].controllerID],
                                            ControllerAxisRanges[MIN_AXIS][manager->controllers[controllerIndex].controllerID],
                                            manager->controllers[controllerIndex].controllerInputs[G_RX_AXIS],
                                            manager->controllers[controllerIndex].controllerInputs[G_RY_AXIS],
                                            *(manager->deadzoneType),
                                            *(manager->deadzonePercentage));
                        
                        // Send RY event
                        manager->controllers[controllerIndex].controllerInputs[G_RY_AXIS] *= -1.0f; // to fix flipped value
                        eventData.inputCode = G_RY_AXIS;
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_RY_AXIS];
                        manager->controllersMutex->unlock();
                        
                        manager->listenerMutex->lock();
                        for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                            iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                        manager->listenerMutex->unlock();
                        
                        manager->controllersMutex->lock();
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_RX_AXIS];
                        manager->controllersMutex->unlock();
                        
                        if(oldX != eventData.inputValue)
                        {
                            // Send RX event
                            eventData.inputCode = G_RX_AXIS;
                            manager->listenerMutex->lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            manager->listenerMutex->unlock();
                        }
                        
                    }
                    break;
                case G_LEFT_TRIGGER_AXIS:
                    if(scaledValue != manager->LZ)
                    {
                        manager->controllersMutex->lock();
                        manager->LZ = scaledValue;
                        float oldAxis = manager->controllers[controllerIndex].controllerInputs[G_LEFT_TRIGGER_AXIS];
                        if(scaledValue > GENENRAL_TRIGGER_THRESHOLD)
                        {
                            
                            manager->controllers[controllerIndex].controllerInputs[G_LEFT_TRIGGER_AXIS] = scaledValue / GENENRAL_TRIGGER_THRESHOLD;
                            
                        }
                        else
                        {
                            manager->controllers[controllerIndex].controllerInputs[G_LEFT_TRIGGER_AXIS] = 0;
                        }
                        
                        
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_LEFT_TRIGGER_AXIS];
                        manager->controllersMutex->unlock();
                        if(oldAxis != eventData.inputValue)
                        {
                            eventData.inputCode = G_LEFT_TRIGGER_AXIS;
                            
                            manager->listenerMutex->lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            manager->listenerMutex->unlock();
                        }
                        
                    }
                    break;
                case G_RIGHT_TRIGGER_AXIS:
                    if(scaledValue != manager->RZ)
                    {
                        manager->controllersMutex->lock();
                        manager->RZ = scaledValue;
                        float oldAxis = manager->controllers[controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS];
                        if(scaledValue > GENENRAL_TRIGGER_THRESHOLD)
                        {
                            
                            manager->controllers[controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS] = scaledValue / GENENRAL_TRIGGER_THRESHOLD;
                            
                        }
                        else
                        {
                            manager->controllers[controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS] = 0;
                        }
                        
                        
                        eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_RIGHT_TRIGGER_AXIS];
                        manager->controllersMutex->unlock();
                        if(oldAxis != eventData.inputValue)
                        {
                            eventData.inputCode = G_RIGHT_TRIGGER_AXIS;
                            
                            manager->listenerMutex->lock();
                            for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                iter->first->OnEvent(GControllerUUIID, CONTROLLERAXISVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                            manager->listenerMutex->unlock();
                        }
                        
                    }
                    break;
                case G_DPAD_LEFT_BTN: // This is used when the Dpad value is reported as a value of 0-360
                {
                    switch((int)scaledValue)
                    {
                        case 0: // UP
                        {
                            manager->controllersMutex->lock();
                            int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                            int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            manager->controllersMutex->unlock();
                            if(upValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 1;
                                eventData.inputCode = G_DPAD_UP_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
     
 
                            if(rightValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 0;
                                eventData.inputCode = G_DPAD_RIGHT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(downValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 0;
                                eventData.inputCode = G_DPAD_DOWN_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }

                            if(leftValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 0;
                                eventData.inputCode = G_DPAD_LEFT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            break;
                        }
                        case 45: // UP-RIGHT
                        {
                            manager->controllersMutex->lock();
                            int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                            int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            manager->controllersMutex->unlock();
                            if(upValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 1;
                                eventData.inputCode = G_DPAD_UP_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
       
                            if(rightValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 1;
                                eventData.inputCode = G_DPAD_RIGHT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(downValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 0;
                                eventData.inputCode = G_DPAD_DOWN_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(leftValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 0;
                                eventData.inputCode = G_DPAD_LEFT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            break;
                        }
                        case 90: // RIGHT
                        {
                            manager->controllersMutex->lock();
                            int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                            int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            manager->controllersMutex->unlock();
                            
                            if(rightValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 1;
                                eventData.inputCode = G_DPAD_RIGHT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(upValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 0;
                                eventData.inputCode = G_DPAD_UP_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(downValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 0;
                                eventData.inputCode = G_DPAD_DOWN_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            
                            if(leftValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 0;
                                eventData.inputCode = G_DPAD_LEFT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            break;
                        }
                        case 135: // RIGHT-DOWN
                        {
                            manager->controllersMutex->lock();
                            int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                            int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            manager->controllersMutex->unlock();
                            if(rightValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 1;
                                eventData.inputCode = G_DPAD_RIGHT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                
                            if(downValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 1;
                                eventData.inputCode = G_DPAD_DOWN_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(upValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 0;
                                eventData.inputCode = G_DPAD_UP_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            
                            if(leftValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 0;
                                eventData.inputCode = G_DPAD_LEFT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            break;
                        }
                        case 180: // DOWN
                        {
                            manager->controllersMutex->lock();
                            int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                            int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            manager->controllersMutex->unlock();
                            if(downValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 1;
                                eventData.inputCode = G_DPAD_DOWN_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(upValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 0;
                                eventData.inputCode = G_DPAD_UP_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(rightValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 0;
                                eventData.inputCode = G_DPAD_RIGHT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            
                            if(leftValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 0;
                                eventData.inputCode = G_DPAD_LEFT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            break;
                        }
                        case 225: // DOWN-LEFT
                        {
                            manager->controllersMutex->lock();
                            int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                            int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            manager->controllersMutex->unlock();
                            
                            if(downValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 1;
                                eventData.inputCode = G_DPAD_DOWN_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(leftValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 1;
                                eventData.inputCode = G_DPAD_LEFT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(upValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 0;
                                eventData.inputCode = G_DPAD_UP_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(rightValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 0;
                                eventData.inputCode = G_DPAD_RIGHT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            

                            break;
                        }
                        case 270: // LEFT
                        {
                            manager->controllersMutex->lock();
                            int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                            int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                            int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                            int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                            manager->controllersMutex->unlock();
                            
                            if(leftValue != 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 1;
                                eventData.inputCode = G_DPAD_LEFT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(upValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 0;
                                eventData.inputCode = G_DPAD_UP_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(rightValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 0;
                                eventData.inputCode = G_DPAD_RIGHT_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            if(downValue == 1)
                            {
                                manager->controllersMutex->lock();
                                manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 0;
                                eventData.inputCode = G_DPAD_DOWN_BTN;
                                eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                manager->controllersMutex->unlock();
                                
                                manager->listenerMutex->lock();
                                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                    iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                manager->listenerMutex->unlock();
                            }
                            
                            
                        }
                        case 315: // LEFT-UP
                            {
                                manager->controllersMutex->lock();
                                int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                if(leftValue != 1)
                                {
                                    manager->controllersMutex->lock();
                                    manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 1;
                                    eventData.inputCode = G_DPAD_LEFT_BTN;
                                    eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                    manager->controllersMutex->unlock();
                                    
                                    manager->listenerMutex->lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    manager->listenerMutex->unlock();
                                }
                                
                                if(upValue != 1)
                                {
                                    manager->controllersMutex->lock();
                                    manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 1;
                                    eventData.inputCode = G_DPAD_UP_BTN;
                                    eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                    manager->controllersMutex->unlock();
                                    
                                    manager->listenerMutex->lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    manager->listenerMutex->unlock();
                                }
                                
                                
                                if(rightValue == 1)
                                {
                                    manager->controllersMutex->lock();
                                    manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 0;
                                    eventData.inputCode = G_DPAD_RIGHT_BTN;
                                    eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                    manager->controllersMutex->unlock();
                                    
                                    manager->listenerMutex->lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    manager->listenerMutex->unlock();
                                }
                                
                                if(downValue == 1)
                                {
                                    manager->controllersMutex->lock();
                                    manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 0;
                                    eventData.inputCode = G_DPAD_DOWN_BTN;
                                    eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                    manager->controllersMutex->unlock();
                                    
                                    manager->listenerMutex->lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    manager->listenerMutex->unlock();
                                }
                                
                                
                            break;
                        }
                        case 360: // Released
                            {
                                manager->controllersMutex->lock();
                                int upValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                int rightValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                int downValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                int leftValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                manager->controllersMutex->unlock();
                                
                                if(upValue == 1)
                                {
                                    manager->controllersMutex->lock();
                                    manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN] = 0;
                                    eventData.inputCode = G_DPAD_UP_BTN;
                                    eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_UP_BTN];
                                    manager->controllersMutex->unlock();
                                    
                                    manager->listenerMutex->lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    manager->listenerMutex->unlock();
                                }
                                
                                if(rightValue == 1)
                                {
                                    manager->controllersMutex->lock();
                                    manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN] = 0;
                                    eventData.inputCode = G_DPAD_RIGHT_BTN;
                                    eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_RIGHT_BTN];
                                    manager->controllersMutex->unlock();
                                    
                                    manager->listenerMutex->lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    manager->listenerMutex->unlock();
                                }

                                if(downValue == 1)
                                {
                                    manager->controllersMutex->lock();
                                    manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN] = 0;
                                    eventData.inputCode = G_DPAD_DOWN_BTN;
                                    eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_DOWN_BTN];
                                    manager->controllersMutex->unlock();
                                    
                                    manager->listenerMutex->lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    manager->listenerMutex->unlock();
                                }
                                
 
                                if(leftValue == 1)
                                {
                                    manager->controllersMutex->lock();
                                    manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN] = 0;
                                    eventData.inputCode = G_DPAD_LEFT_BTN;
                                    eventData.inputValue = manager->controllers[controllerIndex].controllerInputs[G_DPAD_LEFT_BTN];
                                    manager->controllersMutex->unlock();
                                    
                                    manager->listenerMutex->lock();
                                    for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                                        iter->first->OnEvent(GControllerUUIID, CONTROLLERBUTTONVALUECHANGED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                                    manager->listenerMutex->unlock();
                                }
                                break;
                            }
                    }
                    break;
                }
            }
        }
        
    }
    
}

static void gamepadWasAdded(void* inContext, IOReturn inResult, void* inSender, IOHIDDeviceRef device) {
    
    //NSLog(@"Gamepad was plugged in");
   
    HIDMANAGER* manager = (__bridge HIDMANAGER*)inContext;
    if(*(manager->isRunning))
    {
        
        manager->controllersMutex->lock();
    
        int controllerIndex = FindEmptyControllerIndex(MAX_CONTROLLER_INDEX, manager->controllers);
    
        if(controllerIndex != -1)
        {
            IOReturn res = IOHIDDeviceOpen(device,kIOHIDOptionsTypeNone);
            if(kIOReturnSuccess == res)
            {
                
                uint32_t vendorID;
                int controllerID;
                // IOHIDeviceGetProperty Returns a CFTyperef based on the CString passed in. CFNumberGetValue is used to retrieve the value
                CFNumberGetValue((CFNumberRef)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDVendorIDKey)), kCFNumberSInt32Type, &vendorID);
                
                switch (vendorID) {
                    case SONY_VENDOR_ID:
                        controllerID = G_PS4_CONTROLLER;
                        break;
                        
                    case MICROSOFT_VENDOR_ID:
                        controllerID = G_XBOX_CONTROLLER;
                    default:
                       controllerID = G_GENERAL_CONTROLLER;
                        break;
                }
                
                if(manager->supportedControllerID != G_GENERAL_CONTROLLER && controllerID != manager->supportedControllerID)
                {
                    manager->controllersMutex->unlock();
                    return;
                }
                 manager->controllers[controllerIndex].controllerID = controllerID;
                 manager->controllers[controllerIndex].isConnected = 1;
                 manager->controllers[controllerIndex].device = device;
  
                 // send controller connected event
                
                 GCONTROLLER_EVENT_DATA eventData;
                 std::vector<std::pair<GListener*, unsigned long long>>::iterator iter;
                
                 eventData.controllerIndex = controllerIndex;
                 eventData.inputCode = 0;
                 eventData.inputValue = 0;
                 eventData.isConnected = 1;
                 eventData.controllerID = manager->controllers[controllerIndex].controllerID;
        
                 manager->listenerMutex->lock();
                 for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                     iter->first->OnEvent(GControllerUUIID, CONTROLLERCONNECTED, &eventData,        sizeof(GCONTROLLER_EVENT_DATA));
                 manager->listenerMutex->unlock();
                 IOHIDDeviceRegisterInputValueCallback(device, Handle_IOHIDDeviceInputValueCallback, (__bridge void*)manager);
            }
        }
    
        manager->controllersMutex->unlock();
    }
    
}

static void gamepadWasRemoved(void* inContext, IOReturn inResult, void* inSender, IOHIDDeviceRef device)
{
   // NSLog(@"Gamepad was unplugged");
    
    IOHIDDeviceClose(device, kIOHIDOptionsTypeNone);
    HIDMANAGER* manager = (__bridge HIDMANAGER*)inContext;
    if(*(manager->isRunning))
    {
        
        manager->controllersMutex->lock();
        
        int controllerIndex = 0;
        for(; controllerIndex < MAX_CONTROLLER_INDEX; ++controllerIndex)
        {
            if(device == manager->controllers[controllerIndex].device)
            {
                manager->controllers[controllerIndex].isConnected = 0;
                
                // send controller connected event
                
                GCONTROLLER_EVENT_DATA eventData;
                std::vector<std::pair<GListener*, unsigned long long>>::iterator iter;
                
                eventData.controllerIndex = controllerIndex;
                eventData.inputCode = 0;
                eventData.inputValue = 0;
                eventData.isConnected = 0;
                eventData.controllerID = manager->controllers[controllerIndex].controllerID;
                
                manager->listenerMutex->lock();
                for (iter = listeners.begin(); iter != listeners.end(); ++iter)
                    iter->first->OnEvent(GControllerUUIID, CONTROLLERDISCONNECTED, &eventData, sizeof(GCONTROLLER_EVENT_DATA));
                manager->listenerMutex->unlock();
            }
        }
        manager->controllersMutex->unlock();
        }
       
}




#ifdef __APPLE__
IOHIDManagerRef hidManager;
NSThread* runLoopThread;
#endif
@implementation HIDMANAGER : NSObject

-(void) InitManagerAndRunLoop
{


hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDManagerOptionNone);


IOHIDManagerRegisterDeviceMatchingCallback(hidManager, gamepadWasAdded, (__bridge void*)self);
IOHIDManagerRegisterDeviceRemovalCallback(hidManager, gamepadWasRemoved, (__bridge void*)self);


IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

CFMutableArrayRef criteria = CreateHIDManagerCriteria();
IOHIDManagerSetDeviceMatchingMultiple(hidManager, criteria);

CFRelease(criteria);

IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
CFTimeInterval timer = 1;
Boolean runLoopReturn = true;
    
// The run loop will exit once a second to check and see if GController is still running
// The run loop is used for proccessing the events for the IOHIDManager it work similar to [NSAPP run]
while(*isRunning)
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, timer, runLoopReturn);

}


@end

#include "GController.cpp"
