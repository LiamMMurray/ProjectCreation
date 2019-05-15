#include "../../Interface/G_System/GKeyDefines.h"
#include <map>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#elif __APPPLE__

#endif

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define TURNON_BIT(var,pos) ((var) |=  (1<<(pos)))
#define TURNOFF_BIT(var,pos) ((var) &= ~(1<<(pos)))
#define TOGGLE_BIT(var, pos) (CHECK_BIT(var, pos) ? (TURNOFF_BIT(var,pos)) : (TURNON_BIT(var,pos)))

static const unsigned int Keycodes[][3] = {
	//Windows Scancodes		//Linux                 //Apple
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_A}, // 0
	{ G_KEY_ESCAPE,			G_KEY_UNKNOWN,          G_KEY_S}, // 1
	{ G_KEY_1,				G_KEY_UNKNOWN,          G_KEY_D}, // 2
	{ G_KEY_2,				G_KEY_UNKNOWN,          G_KEY_F}, // 3
	{ G_KEY_3,				G_KEY_UNKNOWN,          G_KEY_H}, // 4
	{ G_KEY_4,				G_KEY_UNKNOWN,          G_KEY_G}, // 5
	{ G_KEY_5,				G_KEY_UNKNOWN,          G_KEY_Z}, // 6
	{ G_KEY_6,				G_KEY_UNKNOWN,          G_KEY_X}, // 7
	{ G_KEY_7,				G_KEY_UNKNOWN,          G_KEY_C}, // 8
	{ G_KEY_8,				G_KEY_ESCAPE,           G_KEY_V}, // 9
	{ G_KEY_9,				G_KEY_1,                G_KEY_UNKNOWN}, // 10
	{ G_KEY_0,				G_KEY_2,                G_KEY_B}, // 11
	{ G_KEY_MINUS,			G_KEY_3,                G_KEY_Q}, // 12
	{ G_KEY_EQUALS,			G_KEY_4,                G_KEY_W}, // 13
	{ G_KEY_BACKSPACE,		G_KEY_5,                G_KEY_E}, // 14
	{ G_KEY_TAB,			G_KEY_6,                G_KEY_R}, // 15
	{ G_KEY_Q,				G_KEY_7,                G_KEY_Y}, // 16
	{ G_KEY_W,				G_KEY_8,                G_KEY_T}, // 17
	{ G_KEY_E,				G_KEY_9,                G_KEY_1}, // 18
    { G_KEY_R,				G_KEY_0,                G_KEY_2}, // 19
	{ G_KEY_T,				G_KEY_MINUS,            G_KEY_3}, // 20
	{ G_KEY_Y,				G_KEY_EQUALS,           G_KEY_4}, // 21
	{ G_KEY_U,				G_KEY_BACKSPACE,        G_KEY_6}, // 22
	{ G_KEY_I,				G_KEY_TAB,              G_KEY_5}, // 23
	{ G_KEY_O,				G_KEY_Q,                G_KEY_EQUALS}, // 24
	{ G_KEY_P,				G_KEY_W,                G_KEY_9}, // 25
	{ G_KEY_BRACKET_OPEN,	G_KEY_E,                G_KEY_7}, // 26
	{ G_KEY_BRACKET_CLOSE,	G_KEY_R,                G_KEY_MINUS}, // 27
	{ G_KEY_ENTER,			G_KEY_T,                G_KEY_8}, // 28
	{ G_KEY_CONTROL,		G_KEY_Y,                G_KEY_0}, // 29
	{ G_KEY_A,				G_KEY_U,                G_KEY_BRACKET_CLOSE}, // 30
	{ G_KEY_S,				G_KEY_I,                G_KEY_O}, // 31
	{ G_KEY_D,				G_KEY_O,                G_KEY_U}, // 32
	{ G_KEY_F,				G_KEY_P,                G_KEY_BRACKET_OPEN}, // 33
	{ G_KEY_G,				G_KEY_BRACKET_OPEN,     G_KEY_I}, // 34
	{ G_KEY_H,				G_KEY_BRACKET_CLOSE,    G_KEY_P}, // 35
	{ G_KEY_J,				G_KEY_ENTER,            G_KEY_ENTER}, // 36
	{ G_KEY_K,				G_KEY_CONTROL,          G_KEY_L}, // 37
	{ G_KEY_L,				G_KEY_A,                G_KEY_J}, // 38
	{ G_KEY_COLON,			G_KEY_S,                G_KEY_QUOTE}, // 39
	{ G_KEY_QUOTE,			G_KEY_D,                G_KEY_K}, // 40
	{ G_KEY_TILDE,			G_KEY_F,                G_KEY_COLON}, // 41
	{ G_KEY_LEFTSHIFT,		G_KEY_G,                G_KEY_BACKSLASH}, // 42
	{ G_KEY_BACKSLASH,		G_KEY_H,                G_KEY_COMMA}, // 43
	{ G_KEY_Z,				G_KEY_J,                G_KEY_FOWARDSLASH}, // 44
	{ G_KEY_X,				G_KEY_K,                G_KEY_N}, // 45
	{ G_KEY_C,				G_KEY_L,                G_KEY_M}, // 46
	{ G_KEY_V,				G_KEY_COLON,            G_KEY_PERIOD}, // 47
	{ G_KEY_B,				G_KEY_QUOTE,            G_KEY_TAB}, // 48
	{ G_KEY_N,				G_KEY_TILDE,            G_KEY_SPACE}, // 49
	{ G_KEY_M,				G_KEY_LEFTSHIFT,        G_KEY_TILDE}, // 50
	{ G_KEY_COMMA,			G_KEY_BACKSLASH,        G_KEY_DELETE}, // 51
	{ G_KEY_PERIOD,			G_KEY_Z,                G_KEY_UNKNOWN}, // 52
	{ G_KEY_FOWARDSLASH,	G_KEY_X,                G_KEY_ESCAPE}, // 53
	{ G_KEY_RIGHTSHIFT,		G_KEY_C,                G_KEY_UNKNOWN}, // 54
    { G_KEY_PRINTSCREEN,	G_KEY_V,                G_KEY_CONTROL}, // 55
	{ G_KEY_LEFTALT,		G_KEY_B,                G_KEY_LEFTSHIFT}, // 56
	{ G_KEY_SPACE,			G_KEY_N,                G_KEY_CAPSLOCK}, // 57
	{ G_KEY_CAPSLOCK,		G_KEY_M,                G_KEY_LEFTALT}, // 58
	{ G_KEY_F1,				G_KEY_COMMA,            G_KEY_CONTROL}, // 59
	{ G_KEY_F2,				G_KEY_PERIOD,           G_KEY_RIGHTSHIFT}, // 60
	{ G_KEY_F3,				G_KEY_FOWARDSLASH,      G_KEY_RIGHTALT}, // 61
	{ G_KEY_F4,				G_KEY_RIGHTSHIFT,       G_KEY_CONTROL}, // 62
	{ G_KEY_F5,				G_KEY_MULTIPLY,         G_KEY_UNKNOWN}, // 63
	{ G_KEY_F6,				G_KEY_LEFTALT,          G_KEY_UNKNOWN}, // 64
	{ G_KEY_F7,				G_KEY_SPACE,            G_KEY_DELETE}, // 65
	{ G_KEY_F8,				G_KEY_UNKNOWN,          G_KEY_UNKNOWN}, // 66
	{ G_KEY_F9,				G_KEY_F1,               G_KEY_MULTIPLY}, // 67
	{ G_KEY_F10,			G_KEY_F2,               G_KEY_UNKNOWN}, // 68
	{ G_KEY_NUMLOCK,		G_KEY_F3,               G_KEY_NUMPAD_PLUS}, // 69
	{ G_KEY_SCROLL_LOCK,	G_KEY_F4,               G_KEY_UNKNOWN}, // 70
	{ G_KEY_HOME,			G_KEY_F5,               G_KEY_UNKNOWN}, // 71
	{ G_KEY_UP,				G_KEY_F6,               G_KEY_UNKNOWN}, // 72
	{ G_KEY_PAGEUP,			G_KEY_F7,               G_KEY_UNKNOWN}, // 73
	{ G_KEY_NUMPAD_MINUS,	G_KEY_F8,               G_KEY_UNKNOWN}, // 74
	{ G_KEY_LEFT,			G_KEY_F9,               G_KEY_DIVIDE}, // 75
	{ G_KEY_CENTER,			G_KEY_F10,              G_KEY_ENTER}, // 76
	{ G_KEY_RIGHT,			G_KEY_NUMLOCK,          G_KEY_UNKNOWN}, // 77
	{ G_KEY_NUMPAD_PLUS,	G_KEY_UNKNOWN,          G_KEY_MINUS}, // 78
	{ G_KEY_END,			G_KEY_HOME,             G_KEY_UNKNOWN}, // 79
	{ G_KEY_DOWN,			G_KEY_UP,               G_KEY_UNKNOWN}, // 80
	{ G_KEY_PAGEDOWN,		G_KEY_PAGEUP,           G_KEY_EQUALS}, // 81
	{ G_KEY_INSERT,			G_KEY_MINUS,            G_KEY_INSERT}, // 82
	{ G_KEY_DELETE,			G_KEY_LEFT,             G_KEY_END}, // 83
	{ G_KEY_UNKNOWN,		G_KEY_CENTER,           G_KEY_DOWN}, // 84
	{ G_KEY_UNKNOWN,		G_KEY_RIGHT,            G_KEY_PAGEDOWN}, // 85
	{ G_KEY_UNKNOWN,		G_KEY_ADD,              G_KEY_LEFT}, // 86
	{ G_KEY_F12,			G_KEY_END,              G_KEY_CENTER}, //87
	{ G_KEY_F12,			G_KEY_DOWN,             G_KEY_RIGHT}, //88
	{ G_KEY_UNKNOWN,		G_KEY_PAGEDOWN,         G_KEY_HOME}, //89
	{ G_KEY_UNKNOWN,		G_KEY_INSERT,           G_KEY_UNKNOWN}, //90
	{ G_KEY_UNKNOWN,		G_KEY_PERIOD,           G_KEY_UP}, //91
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_PAGEUP}, //92
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_UNKNOWN}, //93
	{ G_KEY_UNKNOWN,		G_KEY_F11,              G_KEY_UNKNOWN}, //94
	{ G_KEY_UNKNOWN,		G_KEY_F12,              G_KEY_UNKNOWN}, //95
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F5}, //96
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F6}, //97
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F7}, //98
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F3}, //99
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F8}, //100
    { G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F9}, //101
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_UNKNOWN}, //102
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F11}, //103
	{ G_KEY_UNKNOWN,		G_KEY_ENTER,            G_KEY_UNKNOWN}, //104
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_UNKNOWN}, //105
	{ G_KEY_UNKNOWN,		G_KEY_DIVIDE,           G_KEY_UNKNOWN}, //106
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_UNKNOWN}, //107
	{ G_KEY_UNKNOWN,		G_KEY_RIGHTALT,         G_KEY_UNKNOWN}, //108
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F10}, //109
	{ G_KEY_UNKNOWN,		G_KEY_HOME,             G_KEY_UNKNOWN}, //110
	{ G_KEY_UNKNOWN,		G_KEY_UP,               G_KEY_F12}, //111
	{ G_KEY_UNKNOWN,		G_KEY_PAGEUP,           G_KEY_UNKNOWN}, //112
	{ G_KEY_UNKNOWN,		G_KEY_LEFT,             G_KEY_UNKNOWN}, //113
	{ G_KEY_UNKNOWN,		G_KEY_RIGHT,            G_KEY_UNKNOWN}, //114
	{ G_KEY_UNKNOWN,		G_KEY_END,              G_KEY_HOME}, //115
	{ G_KEY_UNKNOWN,		G_KEY_DOWN,             G_KEY_PAGEUP}, //116
	{ G_KEY_UNKNOWN,		G_KEY_PAGEDOWN,         G_KEY_DELETE}, //117
	{ G_KEY_UNKNOWN,		G_KEY_SCROLL_LOCK,      G_KEY_F4}, //118
	{ G_KEY_UNKNOWN,		G_KEY_DELETE,           G_KEY_END}, //119
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F2}, //120
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_PAGEDOWN}, //121
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_F1}, //122
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_LEFT}, //123
	{ G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_RIGHT}, //124
    { G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_DOWN}, //125
    { G_KEY_UNKNOWN,		G_KEY_UNKNOWN,          G_KEY_UP}, //126
};


