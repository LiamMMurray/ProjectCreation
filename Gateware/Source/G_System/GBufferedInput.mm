#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>


//The GResponder is our interpretation of the NSReponder to recieve events.
@interface GResponder : NSResponder

-(bool)acceptFirstResponder;

-(bool)acceptsFirstMouse:(NSEvent *)event;

-(void)keyDown:(NSEvent *)theEvent;
-(void)keyUp:(NSEvent *)theEvent;

-(void)mouseDown:(NSEvent *)theEvent;
-(void)mouseUp:(NSEvent *)theEvent;

-(void)rightmouseDown:(NSEvent *)theEvent;
-(void)rightmouseUp:(NSEvent *)theEvent;

-(void)othermouseDown:(NSEvent *)theEvent;
-(void)othermouseUp:(NSEvent *)theEvent;
-(void)scrollWheel:(NSEvent *)theEvent;

-(void)GetKeyMask:(NSEvent *)theEvent;
/*-(void)SendEvent:(unsigned int)Data;*/

@end

#include "../../Source/G_System/GBI_Callback.cpp"

@implementation GResponder

-(bool)acceptFirstResponder{
    return YES;
}

-(bool)acceptsFirstMouse:(NSEvent *)event{
    return YES;
}

-(void)keyDown:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
	//Get the key from the static list of keys.
    _dataStruct.data = Keycodes[[theEvent keyCode]][2];
	//Call the GetKeyMask Function passing the event passed to this functions.
    [self GetKeyMask:theEvent];
	//Set the keymask.
    _dataStruct.keyMask = keyMask;
	//Get the mouse position relative to the window.
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
	//Get the mouse position relative to the screen.
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
	//Send off the event.
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, KEYPRESSED, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
    }
    if(self.nextResponder != nil)
        [self.nextResponder keyDown:theEvent];
}

-(void)keyUp:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
    _dataStruct.data = Keycodes[[theEvent keyCode]][2];
    [self GetKeyMask:theEvent];
    _dataStruct.keyMask = keyMask;
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, KEYRELEASED, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
    }
    if(self.nextResponder != nil)
        [self.nextResponder keyUp:theEvent];
}

-(void)mouseDown:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
    _dataStruct.data = G_BUTTON_LEFT;
    [self GetKeyMask:theEvent];
    _dataStruct.keyMask = keyMask;
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
   std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, BUTTONPRESSED, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
    }
    if(self.nextResponder != nil)
        [self.nextResponder mouseDown:theEvent];
}

-(void)mouseUp:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
    _dataStruct.data = G_BUTTON_LEFT;
    [self GetKeyMask:theEvent];
    _dataStruct.keyMask = keyMask;
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, BUTTONRELEASED, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
    }
    if(self.nextResponder != nil)
        [self.nextResponder mouseUp:theEvent];
}

-(void)rightmouseDown:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
    _dataStruct.data = G_BUTTON_RIGHT;
    [self GetKeyMask:theEvent];
    _dataStruct.keyMask = keyMask;
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
   std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, BUTTONPRESSED, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
    }
    if(self.nextResponder != nil)
        [self.nextResponder rightMouseDown:theEvent];
}
-(void)rightmouseUp:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
    _dataStruct.data = G_BUTTON_RIGHT;
    [self GetKeyMask:theEvent];
    _dataStruct.keyMask = keyMask;
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, BUTTONRELEASED, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
    }
    if(self.nextResponder != nil)
        [self.nextResponder rightMouseUp:theEvent];
}

-(void)othermouseDown:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
    _dataStruct.data = G_BUTTON_MIDDLE;
    [self GetKeyMask:theEvent];
    _dataStruct.keyMask = keyMask;
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, BUTTONPRESSED, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
    }
    if(self.nextResponder != nil)
        [self.nextResponder otherMouseDown:theEvent];
}

-(void)othermouseUp:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
    _dataStruct.data = G_BUTTON_MIDDLE;
    [self GetKeyMask:theEvent];
    _dataStruct.keyMask = keyMask;
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
    std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, BUTTONRELEASED, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
    }
    if(self.nextResponder != nil)
        [self.nextResponder otherMouseUp:theEvent];
}

-(void)scrollWheel:(NSEvent *)theEvent{
    GBUFFEREDINPUT_EVENT_DATA _dataStruct;
	//Check wether the its a scroll up or down event.
    if([theEvent scrollingDeltaY] > 0){
        _dataStruct.data = G_MOUSE_SCROLL_UP;
    }
    if([theEvent scrollingDeltaY] < 0){
        _dataStruct.data = G_MOUSE_SCROLL_DOWN;
    }
	//Get the keymask using the getkeymask function/
    [self GetKeyMask:theEvent];
	//Set the keymask.
    _dataStruct.keyMask = keyMask;
	//Get the mouse position relative to the window.
    NSPoint mousePosition = [theEvent locationInWindow];
    _dataStruct.x = mousePosition.x;
    _dataStruct.y = mousePosition.y;
	//Get the mouse position relative to the screen.
    NSPoint screenMousePosition = [NSEvent mouseLocation];
    _dataStruct.screenX = screenMousePosition.x;
    _dataStruct.screenY = screenMousePosition.y;
	//Make sure theres data to send and send the data to all registered listeners.
    if(_dataStruct.data){
        std::vector<std::pair<GListener*, unsigned long long>>::iterator iter = _listeners.begin();
        for (; iter != _listeners.end(); ++iter) {
            iter->first->OnEvent(GBufferedInputUUIID, MOUSESCROLL, (void*)&_dataStruct, sizeof(GBUFFEREDINPUT_EVENT_DATA));
        }
    }
    if(self.nextResponder != nil)
        [self.nextResponder scrollWheel:theEvent];
}

-(void)GetKeyMask:(NSEvent *)theEvent{
    keyMask = 0;
	//Get the unsigned int of all the modifier flags.
    NSUInteger flags = [theEvent modifierFlags];
	//Check individual modifier flags and turn them on respectivly to our keymask(unsigned int).
    if (flags & NSEventModifierFlagShift) {
        TURNON_BIT(keyMask, G_MASK_SHIFT);
    }
    if (flags & NSEventModifierFlagOption) {
        TURNON_BIT(keyMask, G_MASK_ALT);
    }
    if (flags & NSEventModifierFlagCommand) {
        TURNON_BIT(keyMask, G_MASK_COMMAND);
    }
    if (flags & NSEventModifierFlagControl) {
        TURNON_BIT(keyMask, G_MASK_CONTROL);
    }
    if (flags & NSEventModifierFlagCapsLock) {
        TURNON_BIT(keyMask, G_MASK_CAPS_LOCK);
    }
    if (flags & NSEventModifierFlagFunction) {
        TURNON_BIT(keyMask, G_MASK_FUNCTION);
    }
    if (flags & NSEventModifierFlagNumericPad) {
        TURNON_BIT(keyMask, G_MASK_NUM_LOCK);
    }
    
}

/*-(void)SendEvent:(unsigned int)Type, Data{
    std::map<GListener *, unsigned long long>::iterator iter = _listeners.begin();
    for (; iter != _listeners.end(); ++iter) {
        iter->first->OnEvent(GBufferedInputUUIID, Data, (void*)&_dataStruct, sizeof(G_INPUT_DATA));
    }
}*/

#include "../../Source/G_System/GBufferedInput.cpp"

@end
