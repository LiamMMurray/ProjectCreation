#ifndef DLL_EXPORT_SYMBOLS
#define DLL_EXPORT_SYMBOLS

/*!
File: DLL_Export_Symbols.h
Purpose: Overrides export symbols for DLL project builds, include in gateware cpp source before including main interface.
Author: Lari H. Norri
Contributors: N/A
Last Modified: 12/14/2016
Copyright: 7thGate Software LLC.
License: MIT
*/

// Define the below symbol in your project if you wish to enable DLL builds
#if defined(GATEWARE_EXPORT_BUILD)

	#if defined(__APPLE__) || defined(__linux__)
		
		#define GATEWARE_EXPORT_IMPLICIT __attribute__((visibility("default")))
		#define GATEWARE_EXPORT_EXPLICIT extern "C" __attribute__((visibility("default"))) 	

	#elif defined(_WIN32)

		#define GATEWARE_EXPORT_IMPLICIT __declspec(dllexport)
		#define GATEWARE_EXPORT_EXPLICIT extern "C" __declspec(dllexport)
		
	#else

		#error DLL builds not currently supported on this platfrom 

	#endif

#endif

#endif // end pre-processor guard