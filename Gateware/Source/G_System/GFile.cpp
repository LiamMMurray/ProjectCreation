// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"

#include "../../Interface/G_System/GFile.h"
#include <fstream>  //file streams
#include <string>  //strings
#include <atomic>  //atomic variables
#include <mutex>  //mutex locks
#include "GUtility.h"  //Internal utility functions
#include <stdio.h>

//iOS include, Apple Only.
#if defined(__APPLE__)

#include <TargetConditionals.h>  //iOS preprocessor definitions

#endif

//dirent.h is not native to Windows and is added to the project
//The " " are used for include so the compiler knows to look in the
//project folder first.
//dirent.h is native in Linux and Mac so the < > are used to include.
#if defined(__APPLE__) || defined(__linux__)

//Apple and Linux includes.
#include <dirent.h>  //Directory handling.
#include <sys/stat.h>  //File stats.
#include <string.h>
#include <cstring>

#define DIR_SEPERATOR '/'

#elif defined(_WIN32)

//Windows specific includes.
#include <io.h>  //Included for mode change.
#include <fcntl.h>  //Included for mode change.
#include "direntw.h"

//Windows specific #defines.
//This keeps most of the function's code branchless between platforms.
#define DIR _WDIR
#define dirent _wdirent
#define fstream wfstream
#define string wstring

#define opendir _wopendir
#define readdir _wreaddir
#define closedir _wclosedir
#define rewinddir _wrewinddir

#define DIR_SEPERATOR L'\\'

#else

#error Gateware libraries are not currently supported for your platform

#endif

//The using statements for specifically what we are using.
using namespace GW;
using namespace SYSTEM;
using namespace CORE;
using namespace INTERNAL;
using std::string;
using std::fstream;
using std::ios;
using std::atomic;
using std::mutex;
using std::getline;

class FileIO : public GW::SYSTEM::GFile
{
	DIR* currDirStream;  //Maintains the current directory.
	fstream file;  //Maintains the current file (if one is open).
	FILE* binaryFile = NULL; //for binary read and write
#if defined(_WIN32)
	errno_t err;// for checking file opened in sucure way
#endif
	string currDir;  //A cached directory path for faster fetching.

	atomic<unsigned int> dirSize;  //A cached directory size for faster fetching.

	atomic<unsigned int> refCount;  //Reference counter.

	atomic<unsigned int> mode; //Used to track what open mode the file is in

	mutex lock; //Read/Write lock.

public:
	FileIO();
	virtual ~FileIO();

	GReturn OpenBinaryRead(const char* const _file) override;

	GReturn OpenBinaryWrite(const char* const _file) override;

	GReturn AppendBinaryWrite(const char* const _file) override;

	GReturn OpenTextRead(const char* const _file) override;

	GReturn OpenTextWrite(const char* const _file) override;

	GReturn AppendTextWrite(const char* const _file) override;

	GReturn Write(const char* const _inData, unsigned int _numBytes) override;

	GReturn Read(char* _outData, unsigned int _numBytes) override;

	GReturn WriteLine(const char* const _inData) override;

	GReturn ReadLine(char* _outData, unsigned int _outDataSize, char _delimiter) override;

	GReturn CloseFile() override;

	GReturn FlushFile() override;

	GReturn SetCurrentWorkingDirectory(const char* const _dir) override;

	GReturn GetCurrentWorkingDirectory(char* _dir, unsigned int _dirSize) override;

	GReturn GetDirectorySize(unsigned int& _outSize) override;

	GReturn GetFilesFromDirectory(char* _outFiles[], unsigned int _numFiles, unsigned int _fileNameSize) override;

	GReturn GetFileSize(const char* const _file, unsigned int& _outSize) override;

	GReturn GetCount(unsigned int& _outCount) override;

	GReturn IncrementCount() override;

	GReturn DecrementCount() override;

	GReturn RequestInterface(const GW::GUUIID& _interfaceID, void** _outputInterface) override;

	GReturn Init(); //The init function for this class in order to initialize variables.
};

FileIO::FileIO() : refCount(1)
{
	currDirStream = nullptr;
}

FileIO::~FileIO()
{
	//Close the current directory.
	closedir(currDirStream);

	//Close the file stream.
	CloseFile();
}

GW::GReturn FileIO::Init()
{
	//Set the current working directory to the directory the program was ran from.
#if TARGET_OS_IOS || TARGET_OS_SIMULATOR
	string tempDir = getenv("HOME");
	tempDir += "/Library";
	GW::GReturn rv = SetCurrentWorkingDirectory(tempDir.c_str());
	if (G_FAIL(rv))
		return rv;
#else
	GW::GReturn rv = SetCurrentWorkingDirectory("./");
	if (G_FAIL(rv))
		return rv;
#endif

	//Imbue the file with utf8 if on Windows.
#if defined(_WIN32)

	//Create a UTF8 Locale to imbue the fstream with.
	std::locale utf8Locale(std::locale(), new std::codecvt_utf8<wchar_t>);

	//Imbue the fstream.
	file.imbue(utf8Locale);

#endif

	return GW::SUCCESS;
}

GW::GReturn FileIO::OpenBinaryRead(const char* const _file)
{
	//Check for invalid arguments.
	if (_file == nullptr)
		return GW::INVALID_ARGUMENT;

#if defined(_WIN32)
	char tempDir[260];
	strcpy_s(tempDir, G_TO_UTF8(currDir).c_str());
	strcat_s(tempDir, _file);

	//using fopen_s to securely open the file in binary reading mode
	if ((err = fopen_s(&binaryFile, tempDir, "rb")) != 0)
	{
		if (err == 2)
			return GW::FILE_NOT_FOUND;
		else
			return GW::FAILURE;
	}

	//Set mode to read
	mode = ios::in;

#elif defined(__APPLE__) || defined(__linux__)
	//Ensure a file is not already open.
	if (file.is_open())
		return GW::FAILURE;

	//Open the new file in the currentWorkingDirectory.
	file.open(currDir + G_TO_UTF16(_file), ios::in | ios::binary);

	//If the file failed to open the function fails.
	if (!file.is_open())
		return GW::FILE_NOT_FOUND;

    //Set mode to read
	mode = ios::in;
#endif
	return GW::SUCCESS;
}

GW::GReturn FileIO::OpenBinaryWrite(const char* const _file)
{

	//Check for invalid arguments.
	if (_file == nullptr)
		return GW::INVALID_ARGUMENT;

#if defined(_WIN32)
	char tempDir[260];
	strcpy_s(tempDir, G_TO_UTF8(currDir).c_str());
	strcat_s(tempDir, _file);

	//using fopen_s to securely open the file in binary writing mode
	if ((err = fopen_s(&binaryFile, tempDir, "wb")) != 0)
	{
		if (err == 2)
			return GW::FILE_NOT_FOUND;
		else
			return GW::FAILURE;
	}

	//Set mode to write
	mode = ios::out;

#elif defined(__APPLE__) || defined(__linux__)
	//Check for invalid arguments.
	if (_file == nullptr)
		return GW::INVALID_ARGUMENT;

	//If the file is currently open we fail.
	if (file.is_open())
		return GW::FAILURE;

	//Open the new file.
	file.open(currDir + G_TO_UTF16(_file), ios::out | ios::binary);

	//If file failed to open we fail.
	if (!file.is_open())
		return GW::FILE_NOT_FOUND;

    //Set mode to write
	mode = ios::out;
#endif
	return GW::SUCCESS;
}

GW::GReturn FileIO::AppendBinaryWrite(const char* const _file)
{
	//Check for invalid arguments.
	if (_file == nullptr)
		return GW::INVALID_ARGUMENT;

#if defined(_WIN32)
	char tempDir[260];
	strcpy_s(tempDir, G_TO_UTF8(currDir).c_str());
	strcat_s(tempDir, _file);

	//using fopen_s to securely open the file in binary appending mode
	if ((err = fopen_s(&binaryFile, tempDir, "ab")) != 0)
	{
		if (err == 2)
			return GW::FILE_NOT_FOUND;
		else
			return GW::FAILURE;
	}

	//Set mode to write
	mode = ios::out;

#elif defined(__APPLE__) || defined(__linux__)
	//Close the current file if there is one.
	if (file.is_open())
		return GW::FAILURE;

	//Open the new file.
	file.open(currDir + G_TO_UTF16(_file), ios::out | ios::binary | ios::app | ios::ate);

	//If file failed to open we fail.
	if (!file.is_open())
		return GW::FILE_NOT_FOUND;

	//Set mode to write
	mode = ios::out;

#endif
	return GW::SUCCESS;
}

GW::GReturn FileIO::OpenTextRead(const char* const _file)
{
	//Check for invalid arguments.
	if (_file == nullptr)
		return GW::INVALID_ARGUMENT;

	//Close the current file if there is one.
	if (file.is_open())
		return GW::FAILURE;

	//Open the new file.
	file.open(currDir + G_TO_UTF16(_file), ios::in);

	if (!file.is_open())
		return GW::FILE_NOT_FOUND;

	//Set mode to read
	mode = ios::in;

#if defined(_WIN32)
	//If we are on windows we need to handle the file BOM.
	//This is what tells the system how to handle the file.
	//This only needs to be read in one time so we will read it now.
	//It can be ignored from this point on.

	//Lock the read operation.
	lock.lock();

	wchar_t BOM;
	file.get(BOM);

	lock.unlock();
#endif

	return GW::SUCCESS;
}

GW::GReturn FileIO::OpenTextWrite(const char* const _file)
{
	//Check for invalid arguments.
	if (_file == nullptr)
		return GW::INVALID_ARGUMENT;

	//Close the current file if there is one.
	if (file.is_open())
		return GW::FAILURE;

	//Open the new file.
	file.open(currDir + G_TO_UTF16(_file), ios::out);

	if (!file.is_open())
		return GW::FILE_NOT_FOUND;

	//Set mode to write
	mode = ios::out;

	//Need to write the BOM if we are _WIN32.
#if defined(_WIN32)
	//If we are on windows we need to handle the file BOM.
	//This is what tells the system how to handle the file.
	//This only needs to be wrote out one time so we will write it now.
	//It can be ignored from this point on.

	//Lock the write opertion.
	lock.lock();

	file << L'\xFEFF'; //The imbue earlier will take this wide string and treat it as UTF8.

	lock.unlock();
#endif

	return GW::SUCCESS;
}

GW::GReturn FileIO::AppendTextWrite(const char* const _file)
{
	//Check for invalid arguments.
	if (_file == nullptr)
		return GW::INVALID_ARGUMENT;

	//Close the current file if there is one.
	if (file.is_open())
		return GW::FAILURE;

	//Open the new file.
	file.open(currDir + G_TO_UTF16(_file), ios::out | ios::app | ios::ate);

	if (!file.is_open())
		return GW::FILE_NOT_FOUND;

	//Set mode to write
	mode = ios::out;

	return GW::SUCCESS;
}

GW::GReturn FileIO::Write(const char* const _inData, unsigned int _numBytes)
{
	//Check for invalid arguments.
	if (_inData == nullptr || _numBytes == 0)
		return GW::INVALID_ARGUMENT;

	//Ensure a file is open.
	if (!file.is_open() && binaryFile == NULL)
		return GW::FAILURE;

	//Make sure the file is opened for writing
	if (mode != ios::out)
		return GW::FAILURE;

	//Lock the write operations.
	lock.lock();

	//On windows we need to cast the char* to a wchar_t*.
	//We don't need to on MAC and Linux.

#if defined(_WIN32)
	if (binaryFile)
	{
		fwrite((void*)_inData, sizeof(char), _numBytes, binaryFile);
	}
	else
		file.write((wchar_t*)_inData, _numBytes);

#elif defined(__APPLE__) || defined(__linux__)
	file.write(_inData, _numBytes);

#endif
	lock.unlock();

	return GW::SUCCESS;
}

GW::GReturn FileIO::Read(char* _outData, unsigned int _numBytes)
{
	if (_numBytes == 0)
		return GW::INVALID_ARGUMENT;

	//Ensure a file is open.
	if (!file.is_open() && binaryFile == NULL)
	{
		_outData = nullptr;
		return GW::FAILURE;
	}

	//Make sure the file is opened for reading
	if (mode != ios::in)
		return GW::FAILURE;

	//Lock the read operations.
	lock.lock();

	//On Windows we need to cast the char* to a wchar_t*.
	//We don't need to on MAC or Linux.
#if defined(_WIN32)
	if (binaryFile)
	{
		//setting the buffer size(2nd parameter) be as big as reading data size(_numBytes)
		fread_s(_outData, _numBytes, sizeof(char), _numBytes, binaryFile);
	}
	else
		file.read((wchar_t*)_outData, _numBytes);

#elif defined (__APPLE__) || (__linux__)
	file.read(_outData, _numBytes);

#endif
	lock.unlock();

	return GW::SUCCESS;
}

GW::GReturn FileIO::WriteLine(const char* const _inData)
{
	//Check for invalid arguments.
	if (_inData == nullptr)
		return GW::INVALID_ARGUMENT;

	//Ensure a file is open.
	if (!file.is_open())
		return GW::FAILURE;

	//Make sure the file is opened for writing
	if (mode != ios::out)
		return GW::FAILURE;

	//Transfer the data to a string. #defines make it so the
	//string is what we need it to be on any system we support.
	string writeOutString = G_TO_UTF16(_inData);

	//Lock the write operations.
	lock.lock();

#ifdef _WIN32

	//Write out the string.
	file << writeOutString;

#elif defined(__APPLE__) || defined(__linux__)

	//Write out the string.
	file << writeOutString;

#endif

	lock.unlock();

	return GW::SUCCESS;
}

GW::GReturn FileIO::ReadLine(char* _outData, unsigned int _outDataSize, char _delimiter)
{
	if (_outData == nullptr || _outDataSize == 0)
		return GW::INVALID_ARGUMENT;

	//Ensure file is open.
	if (!file.is_open())
		return GW::FAILURE;

	//Make sure the file is opened for reading
	if (mode != ios::in)
		return GW::FAILURE;

	//The string to be read into.
	string outString;

	//Lock the read operations.
	lock.lock();

#ifdef _WIN32
	//Convert the UTF8 delimeter to UTF16.
	const wchar_t delimiter = *G_TO_UTF16(_delimiter).c_str();

	//Read the information.
	getline(file, outString, delimiter);


#elif defined(__APPLE__) || defined(__linux__)

	//Just read in data normally.
	getline(file, outString, _delimiter);
#endif

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_SIMULATOR)
	strlcpy(_outData, G_TO_UTF8(outString).c_str(), _outDataSize);
#else

#ifdef _WIN32
    //Copy the data over to the out parameter.
	strncpy_s(_outData, _outDataSize, G_TO_UTF8(outString).c_str(), _TRUNCATE);
#endif // _WIN32
	//Copy the data over to the out parameter.
	strcpy_s(_outData, _outDataSize, G_TO_UTF8(outString).c_str());

#endif

	lock.unlock();

	return GW::SUCCESS;
	}

GW::GReturn FileIO::CloseFile()
{
	//If a file is not open, we can not close it.
	if (!file.is_open() && binaryFile == NULL)
		return GW::FAILURE;

	if (binaryFile != NULL )
	{
		fflush(binaryFile);
		fclose(binaryFile);
		binaryFile = nullptr;
	}
	else
	{
		//Flush the file.
		file.flush();
		//Close the file.
		file.close();
	}
	return GW::SUCCESS;
}

GW::GReturn FileIO::FlushFile()
{
	//If a file is not open we can not flush it.
	if (!file.is_open() && binaryFile == NULL)
		return GW::FAILURE;

	if (binaryFile != NULL)
		fflush(binaryFile);
	else
	{
		//flush the file.
		file.flush();
	}
	return GW::SUCCESS;
}

GW::GReturn FileIO::GetCurrentWorkingDirectory(char* _dir, unsigned int _dirSize)
{
	//Check for valid arguments.
	if (_dir == nullptr || _dirSize == 0)
		return GW::INVALID_ARGUMENT;

	//Check that a directory is open.
	if (currDirStream == nullptr)
		return GW::FAILURE;

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_SIMULATOR)
	strlcpy(_dir, G_TO_UTF8(currDir).c_str(), _dirSize);
#else
	//Copy the current directory to the out parameter.
	strcpy_s(_dir, _dirSize, G_TO_UTF8(currDir).c_str());
#endif

	return GW::SUCCESS;
}

GW::GReturn FileIO::SetCurrentWorkingDirectory(const char* const _dir)
{
	//Check for valid arguments.
	if (_dir == nullptr)
		return GW::INVALID_ARGUMENT;

	//Get the absolute path.
#if defined (_WIN32)
	wchar_t buffer[MAX_PATH];
	if (_wfullpath(buffer, G_TO_UTF16(_dir).c_str(), MAX_PATH) == nullptr)
		return GW::FAILURE;

	//Check to make sure the directory exists.
	struct _stat s;
	if (_wstat(buffer, &s) != 0)
		return GW::FILE_NOT_FOUND;

#elif defined(__APPLE__) || defined(__linux__)
	//Get the absolute path.
	char buffer[PATH_MAX];
	if (realpath(_dir, buffer) == nullptr)
		return GW::FAILURE;

	//Ensure the directory exists.
	struct stat s;
	if (stat(buffer, &s) != 0)
		return GW::FILE_NOT_FOUND;
#endif

	//Assign the passed in directory to our internal directory storage.
	currDir = buffer;
	currDir += DIR_SEPERATOR;

	//If there is an open directory, close it.
	if (currDirStream != nullptr)
		closedir(currDirStream);

	//Open new directory.
	currDirStream = opendir(currDir.c_str());

	//Check to ensure directory is open.
	if (currDirStream == nullptr)
		return GW::FAILURE;

	return GW::SUCCESS;
}

GW::GReturn FileIO::GetDirectorySize(unsigned int& _outSize)
{
	//Check that there is a current working directory.
	if (currDirStream == nullptr)
		return GW::FAILURE;

	//Reset the dir size.
	_outSize = 0;

	//Get the number of files in directory.
	struct dirent* file;
	while ((file = readdir(currDirStream)))
	{
		if (file->d_type == DT_REG)
			++_outSize;
	}

	//Set the directory iterator back to the beginning.
	rewinddir(currDirStream);

	return GW::SUCCESS;
}

GW::GReturn FileIO::GetFilesFromDirectory(char* _outFiles[], unsigned int _numFiles, unsigned int _fileNameSize)
{
	//Check that there is a current working directory.
	if (currDirStream == nullptr)
		return GW::FAILURE;

	struct dirent* file;
	unsigned int fileNumber = 0;

	//Read the first file (Should be "." which will be skipped).
	file = readdir(currDirStream);
	if (file == nullptr)
		return GW::FAILURE;

	//Read all files and add regular files to the buffer passed in.
	for (; file != nullptr && fileNumber < _numFiles; file = readdir(currDirStream))
	{
		if (file->d_type == DT_REG)
		{
			string fileName(file->d_name);

#if defined(TARGET_OS_IOS) || defined(TARGET_OS_SIMULATOR)
			strlcpy(_outFiles[fileNumber], G_TO_UTF8(fileName).c_str(), _fileNameSize);
#else
			strcpy_s(_outFiles[fileNumber], _fileNameSize, G_TO_UTF8(fileName).c_str());
#endif

			++fileNumber;
	}
}

	return GW::SUCCESS;
}

GW::GReturn FileIO::GetFileSize(const char* const _file, unsigned int& _outSize)
{
	//Make a full path to the file.
	string filePath = currDir;
	filePath += G_TO_UTF16(_file);

	//Other than the UTF8 to UTF16 conversion for the windows calls,
	//this is handled the same for each platform.
	//We call stat() and it fills in the passed in function
	//with the stats of the passed in path.
#if defined (_WIN32)
	struct _stat s;
	if (_wstat(filePath.c_str(), &s) != 0)
		return GW::FILE_NOT_FOUND;

#elif defined(__APPLE__) || defined(__linux__)
	struct stat s;
	if (stat(filePath.c_str(), &s) != 0)
		return GW::FILE_NOT_FOUND;

#endif

	//Copy the file size to the out parameter.
	_outSize = (unsigned int)s.st_size;

	return GW::SUCCESS;
}

GW::GReturn FileIO::GetCount(unsigned int& _outCount)
{
	//Store reference count.
	_outCount = refCount;

	return GW::SUCCESS;
}

GW::GReturn FileIO::IncrementCount()
{
	//Check to make sure overflow will not occur.
	if (refCount == G_UINT_MAX)
		return GW::FAILURE;

	//Increment reference count.
	++refCount;

	return GW::SUCCESS;
}

GW::GReturn FileIO::DecrementCount()
{
	//Check to make sure underflow will not occur.
	if (refCount == 0)
		return GW::FAILURE;

	//Decrement reference count.
	--refCount;

	if (refCount == 0)
	{
		this->CloseFile();
		delete this;
	}
	return GW::SUCCESS;
}

GW::GReturn FileIO::RequestInterface(const GW::GUUIID& _interfaceID, void** _outputInterface)
{
	if (_outputInterface == nullptr)
		return GW::INVALID_ARGUMENT;

	//If passed in interface is equivalent to current interface (this).
	if (_interfaceID == GW::SYSTEM::GFileUUIID)
	{
		//Temporary GFile* to ensure proper functions are called.
		GFile* convert = reinterpret_cast<GFile*>(this);

		//Increment the count of the GFile.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
	}
	//If requested interface is multithreaded.
	else if (_interfaceID == GW::CORE::GMultiThreadedUUIID)
	{
		//Temporary GMultiThreaded* to ensure proper functions are called.
		GMultiThreaded* convert = reinterpret_cast<GMultiThreaded*>(this);

		//Increment the count of the GMultithreaded.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
	}
	//If requested interface is the primary interface.
	else if (_interfaceID == GW::CORE::GInterfaceUUIID)
	{
		//Temporary GInterface* to ensure proper functions are called.
		GInterface* convert = reinterpret_cast<GInterface*>(this);

		//Increment the count of the GInterface.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
	}
	//Interface is not supported.
	else
		return GW::INTERFACE_UNSUPPORTED;

	return GW::SUCCESS;
}

// This is an DLL exported version of the create function, the name is not mangled for explicit linking.
GATEWARE_EXPORT_EXPLICIT GW::GReturn CreateGFile(GW::SYSTEM::GFile** _outFile)
{
	// This is NOT a recursive call, this is a call to the actual C++ name mangled version below.
	return GW::SYSTEM::CreateGFile(_outFile);
}


GW::GReturn GW::SYSTEM::CreateGFile(GFile** _outFile)
{
	//Check that we were given a valid pointer.
	if (_outFile == nullptr)
		return GW::INVALID_ARGUMENT;

	//Create the new object and make sure it's valid.
	FileIO* file = new FileIO();
	if (!file)
		return GW::FAILURE;

	//Run the FileIO init function.
	GW::GReturn rv = file->Init();
	if (G_FAIL(rv))
		return rv;

	//Store FileIO in the GFile.
	*_outFile = file;

	return GW::SUCCESS;
}
