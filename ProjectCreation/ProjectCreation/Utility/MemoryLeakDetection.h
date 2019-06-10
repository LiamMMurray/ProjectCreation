#pragma once
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#ifdef _DEBUG
#define DBG_MALLOC(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define DBG_MALLOC malloc
#endif
// call _CrtDumpMemoryLeaks(); before app exit