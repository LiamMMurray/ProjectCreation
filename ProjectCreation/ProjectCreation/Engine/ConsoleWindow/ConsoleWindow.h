#pragma once
#include "../Controller/PlayerMovement.h"
#define NOMINMAX
#include <Windows.h>
#include <iostream>

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
        void PrintVector(DirectX::XMVECTOR vector, std::string vectorName)
        {
                DirectX::XMFLOAT4      tempFloat;
                DirectX::XMStoreFloat4(&tempFloat, vector);
                std::cout << vectorName << " <X: " << tempFloat.x << ", Y: " << tempFloat.y << ", Z: " << tempFloat.z
                          << ", W: " << tempFloat.w << ">" << std::endl;
		}
};