// Override export symbols for DLL builds (must be included before interface code).
#include "../DLL_Export_Symbols.h"
#include "../../Interface/G_System/GLog.h"
#include "../../Interface/G_System/GFile.h"
#include "GUtility.h"

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <queue> //To queue the messages
#include <thread> //Threading
#include <mutex> //Thread safety
#include <sstream> //String streams
#include <atomic> //Thread safety
#include <iostream> //For cout
#include <cstring>
#include <condition_variable> //For conditional variables

using namespace INTERNAL;
using std::queue;
using std::thread;
using std::string;
using std::mutex;
using std::atomic;
using std::unique_lock;
using std::condition_variable;
using std::cout;

#define MAX_QUEUE_SIZE 20
#define THREAD_SLEEP_TIME 1
#define TIME_BUFFER 40

class LogFile : public GW::SYSTEM::GLog
{
	GW::SYSTEM::GFile* logFile;  //Our internal GFile to log to file.

	thread* worker; //The worker thread we will spin off.

	mutex queueLock; //Queuelock for locking the queue we are logging to.

	queue<string> logQueue; //The queue we will log to.

	bool isVerbose;  //Verbose logging boolean.

	bool isConsoleLogged; //Console logging boolean.

	atomic<bool> threadRunning; //Boolean to kill the thread.

	atomic<unsigned int> refCount; //Reference counter.

	condition_variable conditional;

public:
	LogFile();
	virtual ~LogFile();

	GW::GReturn Init(const char* const _fileName);

	GW::GReturn Init(GW::SYSTEM::GFile* _file);

	GW::GReturn Log(const char* const _log) override;

	GW::GReturn LogCatergorized(const char* const _category, const char* const _log) override;

	GW::GReturn EnableVerboseLogging(bool _value) override;

	GW::GReturn EnableConsoleLogging(bool _value) override;

	GW::GReturn Flush() override;

	GW::GReturn GetCount(unsigned int &_outCount) override;

	GW::GReturn IncrementCount() override;

	GW::GReturn DecrementCount() override;

	GW::GReturn RequestInterface(const GW::GUUIID &_interfaceID, void** _outputInterface) override;

private:
	void LogWorker(); //Our worker thread function.

	unsigned long long GetThreadID(); //Gets the thread ID.
};

LogFile::LogFile() : refCount(1)
{
	isConsoleLogged = false;
	isVerbose = true;
}

LogFile::~LogFile()
{
}

GW::GReturn LogFile::Init(const char* const _fileName)
{
	//Create a GFile.
	GW::GReturn rv = GW::SYSTEM::CreateGFile(&logFile);
	if (G_FAIL(rv))
		return rv;

	//Open the GFile for text write out with append.
	rv = logFile->AppendTextWrite(_fileName);
	if (G_FAIL(rv))
		return rv;

	//Fire off worker thread.
	threadRunning = true;
	worker = new thread(&LogFile::LogWorker, this);

	//isVerbose is defaulted to true.
	isVerbose = true;
	return GW::SUCCESS;
}

GW::GReturn LogFile::Init(GW::SYSTEM::GFile* _file)
{
	//Check to ensure valid GFile.
	if (_file == nullptr)
		return GW::INVALID_ARGUMENT;

	//Set the GFile and increment the ref count.
	logFile = _file;
	logFile->IncrementCount();

	//Fire off worker thread.
	threadRunning = true;
	worker = new thread(&LogFile::LogWorker, this);

	//isVerbose is defaulted to true.
	isVerbose = true;
	return GW::SUCCESS;
}

GW::GReturn LogFile::Log(const char* const _log)
{
	if (_log == nullptr)
		return GW::INVALID_ARGUMENT;

	std::stringstream logStream;

	//Check verbose logging and add the verbose info if on.
	if (isVerbose)
	{
		time_t t = time(0);   //Get time now.
		char timeBuffer[TIME_BUFFER];

#if defined(_WIN32)
		//Parse the time out to readable time.
		struct tm buf;
		localtime_s(&buf, &t);
		asctime_s(timeBuffer, TIME_BUFFER, &buf);

#elif defined(__APPLE__) || defined(__linux__)
		//Parse the time out to readable time.
		string buffer(asctime(localtime(&t)));
		strcpy_s(timeBuffer, TIME_BUFFER, buffer.c_str());

#endif
		//Get rid of new line added by asctime.
		timeBuffer[strlen(timeBuffer) - 1] = '\0';

		//Create our log string.
		logStream << "[" << timeBuffer << "] ThreadID[";
		logStream << GetThreadID() << "]\t";
	}

	//Add the log and a newline.
	logStream << _log << "\r\n";

	//Lock the mutex to push the new message.
	queueLock.lock();

	//Check to see if we are at our max messages.
	if (logQueue.size() >= MAX_QUEUE_SIZE)
	{
		queueLock.unlock();
		return GW::FAILURE;
	}

	//Push the message to the queue.
	logQueue.push(logStream.str());

	if (isConsoleLogged)
		cout << logStream.str();

#if defined(_MSC_VER)
	OutputDebugStringW(G_TO_UTF16(logStream.str()).c_str());
#endif


	queueLock.unlock();

	return GW::SUCCESS;
}

GW::GReturn LogFile::LogCatergorized(const char* const _category, const char* const _log)
{
	if (_category == nullptr || _log == nullptr)
		return GW::INVALID_ARGUMENT;

	//The stream that will contain the full message.
	std::stringstream logStream;

	//Check verbose logging and add the verbose info if on.
	if (isVerbose)
	{
		time_t t = time(0);   //Get time now.
		char timeBuffer[TIME_BUFFER];

#if defined(_WIN32)

		//Parse time to readable time.
		struct tm buf;
		localtime_s(&buf, &t);
		asctime_s(timeBuffer, TIME_BUFFER, &buf);

#elif defined(__APPLE__) || defined(__linux__)

		//Parse time to readable time.
		string buffer(asctime(localtime(&t)));
		strcpy_s(timeBuffer, TIME_BUFFER, buffer.c_str());

#endif
		//Get rid of new line added by asctime.
		timeBuffer[strlen(timeBuffer) - 1] = '\0';

		//Build the string.
		logStream << "[" << timeBuffer << "] ThreadID[";
		logStream << GetThreadID() << "]\t";
	}

	//Add the category and message.
	logStream << "[" << _category << "]\t" << _log << "\r\n";

	//Lock the mutex to push the new msg.
	queueLock.lock();

	//Check to see if we are at our max messages.
	if (logQueue.size() >= MAX_QUEUE_SIZE)
	{
		queueLock.unlock();
		return GW::FAILURE;
	}

	//Push the message to the queue.
	logQueue.push(logStream.str());

	if (isConsoleLogged)
		cout << logStream.str();

#if defined(_MSC_VER)
	OutputDebugStringW(G_TO_UTF16(logStream.str()).c_str());
#endif

	queueLock.unlock();

	return GW::SUCCESS;
}

GW::GReturn LogFile::EnableVerboseLogging(bool _value)
{
	//Set the verbose boolean.
	isVerbose = _value;
	return GW::SUCCESS;
}

GW::GReturn LogFile::EnableConsoleLogging(bool _value)
{
	//Set the console logging boolean.
	isConsoleLogged = _value;
	return GW::SUCCESS;
}

GW::GReturn LogFile::Flush()
{
	conditional.notify_all();

	//Flush the file.
	return GW::SUCCESS;
}

unsigned long long LogFile::GetThreadID()
{
	//Get the thread ID and store it in a string stream.
	std::stringstream ss;
	ss << std::this_thread::get_id();

	//Convert the string sequence to an unsigned long long.
	return std::stoull(ss.str());
}

GW::GReturn LogFile::GetCount(unsigned int &_outCount)
{
	_outCount = refCount;
	return GW::SUCCESS;
}

GW::GReturn LogFile::IncrementCount()
{
	//Check for possible overflow.
	if (refCount == G_UINT_MAX)
		return GW::FAILURE;

	//Increment the count.
	++refCount;

	return GW::SUCCESS;
}

GW::GReturn LogFile::DecrementCount()
{
	//Check for possible underflow.
	if (refCount == 0)
		return GW::FAILURE;

	//Decrement the count.
	--refCount;

	//Make sure the object should still exist.
	//Delete it if not.
	if (refCount == 0)
	{
		//Tell the thread to stop running and wait for it.
		threadRunning = false;
		worker->join();
		
		//Decrement the count of our internal GFile.
		logFile->DecrementCount();

		//Cleanup.
		delete worker;
		delete this;
	}

	return GW::SUCCESS;
}

GW::GReturn LogFile::RequestInterface(const GW::GUUIID &_interfaceID, void** _outputInterface)
{
	if (_outputInterface == nullptr)
		return GW::INVALID_ARGUMENT;

	//If passed in interface is equivalent to current interface (this).
	if (_interfaceID == GW::SYSTEM::GLogUUIID)
	{
		//Temporary GFile* to ensure proper functions are called.
		GLog* convert = reinterpret_cast<GLog*>(this);

		//Increment the count of the GFile.
		convert->IncrementCount();

		//Store the value.
		(*_outputInterface) = convert;
	}
	//If requested interface is multi-threaded.
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
GATEWARE_EXPORT_EXPLICIT GW::GReturn CreateGLog(const char* const _fileName, GW::SYSTEM::GLog** _outLog)
{
	// This is NOT a recursive call, this is a call to the actual C++ name mangled version below.
	return GW::SYSTEM::CreateGLog(_fileName, _outLog);
}

GW::GReturn GW::SYSTEM::CreateGLog(const char* const _fileName, GLog** _outLog)
{
	//Check to make sure the user passed a valid pointer.
	if (_outLog == nullptr)
		return GW::INVALID_ARGUMENT;

	//Create a new LogFile.
	LogFile* logFile = new LogFile();
	if (logFile == nullptr)
		return GW::FAILURE;

	//Init the log file.
	GW::GReturn rv = logFile->Init(_fileName);
	if (G_FAIL(rv))
	{
		delete logFile;
		return rv;
	}

	//Store new log file in passed in pointer.
	(*_outLog) = logFile;

	return GW::SUCCESS;
}

// This is an DLL exported version of the create function, the name is not mangled for explicit linking.
GATEWARE_EXPORT_EXPLICIT GW::GReturn CreateGLogCustom(GW::SYSTEM::GFile* _file, GW::SYSTEM::GLog** _outLog)
{
	// This is NOT a recursive call, this is a call to the actual C++ name mangled version below.
	return GW::SYSTEM::CreateGLogCustom(_file, _outLog);
}

GW::GReturn GW::SYSTEM::CreateGLogCustom(GFile* _file, GLog** _outLog)
{
	//Check to make sure the user passed a valid pointer.
	if (_outLog == nullptr)
		return GW::INVALID_ARGUMENT;

	//Create a new LogFile.
	LogFile* logFile = new LogFile();
	if (logFile == nullptr)
		return GW::FAILURE;

	//Init the log file.
	GW::GReturn rv = logFile->Init(_file);
	if (G_FAIL(rv))
	{
		delete logFile;
		return rv;
	}

	//Store new log file in passed in pointer.
	(*_outLog) = logFile;

	return GW::SUCCESS;
}

//LogWorker is the function that the logging thread will run. This thread will also control
//the closing of GFile when the log is destroyed.
void LogFile::LogWorker()
{
	unique_lock<mutex> localQueueLock(queueLock);
	while (threadRunning || logQueue.size() != 0)
	{
		//Will lock the mutex when awaken and unlock it when put back to sleep.
		conditional.wait_for(localQueueLock, std::chrono::seconds(THREAD_SLEEP_TIME));

		//If there is anything to write.
		if (logQueue.size() != 0)
		{
			while (logQueue.size() != 0)
			{
				logFile->WriteLine(logQueue.front().c_str());
				logQueue.pop();

				logFile->FlushFile();
			}
		}
	}

	////Close the file.
	//unsigned int refCount = 0;
	//logFile->GetCount(refCount);
	//if (refCount != 0)
	//{
	//	logFile->CloseFile();
	//}
}
