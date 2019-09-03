#pragma once

//INCLUDES
#include <PlayerMovement.h>
#define NOMINMAX
#include <Windows.h>
#include <iostream>

// Pressing the L key will allow a message to be written out to the console window
//#define DEBUG_LOG_CHECK(KEY) (GCoreInput::GetKeyState(KeyCode::KEY) == KeyState::DownFirst)

class ConsoleWindow
{
    public:
        // Creates a console window and sets the window's name to the passed in string
        void CreateConsoleWindow(std::string nameString)
        {
                const char* windowName;
                windowName = nameString.c_str();
                FILE* stream;

                AllocConsole();
                freopen_s(&stream, "CONOUT$", "w", stdout);
                SetConsoleTitle(windowName);
        }

        // Prints out a passed in vector, vectorName is used to display what the variable is
        static void PrintVector(DirectX::XMVECTOR vector, std::string vectorName)
        {
                DirectX::XMFLOAT4 tempFloat;
                DirectX::XMStoreFloat4(&tempFloat, vector);
                std::cout << vectorName << " <X: " << tempFloat.x << ", Y: " << tempFloat.y << ", Z: " << tempFloat.z
                          << ", W: " << tempFloat.w << ">" << std::endl;
        }

        // Prints out a simple message, class name will be used to display which class called the message
        static void PrintMessage(std::string msg, std::string className)
        {
                std::cout << className << ": " << msg << std::endl;
		}
};