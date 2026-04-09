#include "src/Engine/AcademiaEngine.h"

int main(int argc, char* argv[])
{
    AcademiaEngine* engine = AcademiaEngine::Instantiate();
    if(!engine)
    {
        return -1;
    }

    if (engine->Construct(1024, 1024, 1, 1))
        engine->Start();
 
    return 0;
}
