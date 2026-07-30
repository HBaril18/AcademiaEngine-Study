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
        engine->Start();
    }
 
    return 0;
}
