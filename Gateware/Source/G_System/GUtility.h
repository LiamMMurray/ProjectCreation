#ifndef GUTILITY
#define GUTILITY

#include <locale>
#include <codecvt>

namespace INTERNAL
{
	//UINT_MAX is not defined on Mac or Linux.
	//Internal #define for this.
	#define G_UINT_MAX 0xFFFFFFFF

	#if defined(_WIN32)

	//Macro to preform string conversions.
	#define G_TO_UTF16(value) stringConverter.from_bytes(value)
	#define G_TO_UTF8(value) stringConverter.to_bytes(value)

	#elif defined(__APPLE__) || defined(__linux__)

	//These string conversions are not necessary on Linux/Apple.
	//They are designed to be replaced with just the value given to them.
	#define G_TO_UTF16(value) value
	#define G_TO_UTF8(value) value

	//Defined to replace instances of strcpy_s with strlcpy on Linux/Mac.
	//This is because strcpy_s is not supported on GCC.
	#define strcpy_s(...) strlcpy(__VA_ARGS__)

	#else

	//If you get this error then we are currently on a platform not supported by Gateware.
	#error Gateware Libraries are not built for your current system
	#endif

	//All variables and functions below are macroed above. This is so the code wrote out
	//will be the exact same code no matter the system we are writing it for.


	//This is a utf8 to utf16 and utf16 to utf8 string converter.
	//Used like so...
	//to utf16: stringConverter.from_bytes(const char*)  //Is overloaded to accept string as well.
	//to utf8: stringConverter.to_bytes(const wchar_t*) //Is overloaded to accept wstring as well.
	static ::std::wstring_convert < ::std::codecvt_utf8_utf16<wchar_t>> stringConverter;

	//GCC does not support strcpy_s. This is a safe strcpy function for use on Linux.
	static unsigned int strlcpy(char* destination,  unsigned int strLen, const char* const source)
	{
	    return snprintf(destination, strLen, "%s", source);
	}

}//end INTERNAL namespace

#endif //GUTILITY
