#pragma once
#define WIN_32_LEAN_AND_MEAN
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#define PROFILING
inline LARGE_INTEGER TimeStamp()
{
        LARGE_INTEGER TimeStamp;
        LARGE_INTEGER Frequency;
        QueryPerformanceFrequency(&Frequency);
        QueryPerformanceCounter(&TimeStamp);
        TimeStamp.QuadPart *= 1000000;
        TimeStamp.QuadPart /= Frequency.QuadPart;
        return TimeStamp;
}

struct ProfilerContext
{
#ifdef PROFILING
        std::stringstream       ss;
        std::stack<std::string> name_Stack;
        std::stack<std::string> category_stack;
        std::thread::id         tid;
        unsigned                pid;
#endif


        inline void Initialize()
        {
#ifdef PROFILING

                tid = std::this_thread::get_id();
                pid = ::_getpid();
                ss << "[";

#endif
        }

        inline void Begin(std::string cat = "", std::string name = "")
        {
#ifdef PROFILING

                ss << "{ \"cat\":\"" << cat << "\",\"ph\":\"B\", \"name\":\"" << name << "\",\"pid\":\"" << pid
                   << "\",\"tid\":\"" << tid << "\",\"ts\":\"" << TimeStamp().QuadPart << "\"},";
                category_stack.emplace(cat);
                name_Stack.emplace(name);
#endif
        }


        inline void End()
        {
#ifdef PROFILING

                std::string name = name_Stack.top();
                std::string cat  = category_stack.top();
                name_Stack.pop();
                category_stack.pop();
                ss << "{ \"cat\":\"" << cat << "\",\"ph\":\"E\", \"name\":\"" << name << "\",\"pid\":\"" << pid
                   << "\",\"tid\":\"" << tid << "\",\"ts\":\"" << TimeStamp().QuadPart << "\"},";
#endif
        }

        inline void Dump()
        {
#ifdef PROFILING

                ss.seekp(-1, std::ios_base::end);
                ss << "]";
                std::string  s = ss.str();
                std::fstream fs("profiling.json", std::ios::out);
                fs.write(s.c_str(), s.size());
                fs.close();

#endif
        }
};