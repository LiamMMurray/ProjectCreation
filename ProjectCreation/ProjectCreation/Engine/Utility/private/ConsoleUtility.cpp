#include <ConsoleUtility.h>

namespace ConsoleUtil
{
        void CreateConsoleWindow(std::string nameString)
        {
                const char* windowName;
                windowName = nameString.c_str();
                FILE* stream;

                AllocConsole();
                freopen_s(&stream, "CONOUT$", "w", stdout);
                SetConsoleTitle(windowName);
        }

        void PrintVector(DirectX::XMVECTOR vector, std::string vectorName)
        {
                DirectX::XMFLOAT4 tempFloat;
                DirectX::XMStoreFloat4(&tempFloat, vector);
                std::cout << vectorName << " <X: " << tempFloat.x << ", Y: " << tempFloat.y << ", Z: " << tempFloat.z
                          << ", W: " << tempFloat.w << ">" << std::endl;
        }

        void PrintMessage(std::string msg, std::string className)
        {
                std::cout << className << ": " << msg << std::endl;
        }

} // namespace ConsoleUtil