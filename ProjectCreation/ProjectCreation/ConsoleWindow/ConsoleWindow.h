#pragma once
#include "../Controller/PlayerMovement.h"
#include <Windows.h>

class ConsoleWindow
{
    public:
        void CreateConsoleWindow(std::string nameString)
        {
                const char* windowName;
                windowName = nameString.c_str();
                FILE* stream;

                AllocConsole();
                freopen_s(&stream, "CONOUT$", "w", stdout);
                SetConsoleTitle(windowName);
		}
};