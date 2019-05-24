#pragma once


#ifndef _WINDEF_
struct HINSTANCE__; // Forward or never
typedef HINSTANCE__* HINSTANCE;

typedef unsigned int   UINT;
typedef unsigned long  ULONG;
typedef unsigned short USHORT;

#if defined(_WIN64)
typedef __int64          INT_PTR, *PINT_PTR;
typedef unsigned __int64 UINT_PTR, *PUINT_PTR;
typedef __int64          LONG_PTR, *PLONG_PTR;
typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;
#else
typedef __w64 int           INT_PTR, *PINT_PTR;
typedef __w64 unsigned int  UINT_PTR, *PUINT_PTR;
typedef __w64 long          LONG_PTR, *PLONG_PTR;
typedef __w64 unsigned long ULONG_PTR, *PULONG_PTR;
#endif

typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
typedef LONG_PTR LRESULT;

struct HWND__;
typedef HWND__* HWND;
#endif