#include "src/Engine/AcademiaEngine.h"
#include <windows.h>
#include <iostream>

int main(int argc, char* argv[])
{
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    AcademiaEngine* engine = AcademiaEngine::Instantiate();
    if(!engine)
    {
        return -1;
    }

    if (engine->Construct(width, height, 1, 1, true)) {
		std::cout << "Starting engine...\n";
		std::cout << "Screen size: " << width << "x" << height << "\n";
        engine->Start();
    }
 
    return 0;
}
