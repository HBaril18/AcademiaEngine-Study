#pragma once
#include <Utils.h>

#define ACADEMIA_EXAMPLE

#ifdef ACADEMIA_EXAMPLE
#include "ExampleGameObject.h"
#include "Player.h"
#endif

class AcademiaEngine;

class GameManager
{

public:

    void Initialize(AcademiaEngine* engineContext);
    void Update(float elapsedTime);
    void Uninitialize();

private:

    AcademiaEngine* _EngineContext = nullptr;

#ifdef ACADEMIA_EXAMPLE
    ExampleGameObject _ExampleObject;
    Player _Player;
#endif

};

