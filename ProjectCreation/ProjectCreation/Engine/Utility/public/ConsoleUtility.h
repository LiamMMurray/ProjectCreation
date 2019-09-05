#pragma once
namespace ConsoleUtil
{
        // Creates a console window and sets the window's name to the passed in string
        void CreateConsoleWindow(std::string nameString);

        // Prints out a passed in vector, vectorName is used to display what the variable is
        void PrintVector(DirectX::XMVECTOR vector, std::string vectorName);

        // Prints out a simple message, class name will be used to display which class called the message
        void PrintMessage(std::string msg, std::string className);
}; // namespace ConsoleUtil