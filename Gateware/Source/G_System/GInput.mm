#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>


//The GIResponder is our interpretation of the NSReponder to recieve events.
@interface GIResponder : NSResponder
{
    NSUInteger flags;
}
//Overide Functions for the Responder
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
-(void)mouseMoved:(NSEvent *)theEvent;
-(void)GetKeyMask:(NSEvent *)theEvent;

@end

#include "../../Source/G_System/GI_Callback.cpp"

@implementation GIResponder

-(bool)acceptFirstResponder{
    return YES;
}

-(bool)acceptsFirstMouse:(NSEvent *)event{
    return YES;
}

-(void)keyDown:(NSEvent *)theEvent{
	//Set the key to pressed after getting it from the static table of keys.
    n_Keys[Keycodes[[theEvent keyCode]][2]] = 1;
	//Get the keymask.
    [self GetKeyMask:theEvent];
    
    if(self.nextResponder != nil)
        [self.nextResponder keyDown:theEvent];
}

-(void)keyUp:(NSEvent *)theEvent{
    n_Keys[Keycodes[[theEvent keyCode]][2]] = 0;
    [self GetKeyMask:theEvent];
    
    if(self.nextResponder != nil)
        [self.nextResponder keyUp:theEvent];
}

-(void)mouseDown:(NSEvent *)theEvent{
    n_Keys[G_BUTTON_LEFT] = 1;
    [self GetKeyMask:theEvent];
    if(self.nextResponder != nil)
        [self.nextResponder mouseDown:theEvent];
}

-(void)mouseUp:(NSEvent *)theEvent{
    n_Keys[G_BUTTON_LEFT] = 0;
    [self GetKeyMask:theEvent];
    if(self.nextResponder != nil)
        [self.nextResponder mouseUp:theEvent];
}

-(void)rightmouseDown:(NSEvent *)theEvent{
    n_Keys[G_BUTTON_RIGHT] = 1;
    [self GetKeyMask:theEvent];
    if(self.nextResponder != nil)
        [self.nextResponder rightMouseDown:theEvent];
}
-(void)rightmouseUp:(NSEvent *)theEvent{
    n_Keys[G_BUTTON_RIGHT] = 0;
    [self GetKeyMask:theEvent];
    if(self.nextResponder != nil)
        [self.nextResponder rightMouseUp:theEvent];
}

-(void)othermouseDown:(NSEvent *)theEvent{
    n_Keys[G_BUTTON_MIDDLE] = 1;
    [self GetKeyMask:theEvent];
    if(self.nextResponder != nil)
        [self.nextResponder otherMouseDown:theEvent];
}

-(void)othermouseUp:(NSEvent *)theEvent{
    n_Keys[G_BUTTON_MIDDLE] = 0;
    [self GetKeyMask:theEvent];
    if(self.nextResponder != nil)
        [self.nextResponder otherMouseUp:theEvent];
}


-(void)mouseMoved:(NSEvent *)theEvent{
    NSPoint mousePosition = [theEvent locationInWindow];
    _mousePositionX = mousePosition.x;
    _mousePositionY = mousePosition.y;
    
    if(self.nextResponder != nil)
        [self.nextResponder mouseMoved:theEvent];
    
}

-(void)GetKeyMask:(NSEvent *)theEvent{
    keyMask = 0;
	//Get the unsigned int of all the modifier flags.
    NSUInteger xflags = [theEvent modifierFlags];
	//Check individual modifier flags and turn them on respectivly to our keymask(unsigned int).
    if (xflags & NSEventModifierFlagShift) {
        TURNON_BIT(keyMask, G_MASK_SHIFT);
    }
    if (xflags & NSEventModifierFlagOption) {
        TURNON_BIT(keyMask, G_MASK_ALT);
    }
    if (xflags & NSEventModifierFlagCommand) {
        TURNON_BIT(keyMask, G_MASK_COMMAND);
    }
    if (xflags & NSEventModifierFlagControl) {
        TURNON_BIT(keyMask, G_MASK_CONTROL);
    }
    if (xflags & NSEventModifierFlagCapsLock) {
        TURNON_BIT(keyMask, G_MASK_CAPS_LOCK);
    }
    if (xflags & NSEventModifierFlagFunction) {
        TURNON_BIT(keyMask, G_MASK_FUNCTION);
    }
    if (xflags & NSEventModifierFlagNumericPad) {
        TURNON_BIT(keyMask, G_MASK_NUM_LOCK);
    }
    
}


#include "../../Source/G_System/GInput.cpp"

@end

