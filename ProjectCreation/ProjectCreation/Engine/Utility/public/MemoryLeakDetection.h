#pragma once
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#if defined _DEBUG
#define ENABLE_LEAK_DETECTION() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)
#else
#define ENABLE_LEAK_DETECTION()
#endif
//
//#ifdef _DEBUG
//#define new new ( _CLIENT_BLOCK , __FILE__ , __LINE__ )
//#else
//#define new new
//#endif

