#pragma once

#include "../../external/miniaudio/miniaudio.h"
#include <string>

class AudioManager
{
public:
    bool Initialize();
    void Shutdown();

    bool PlayAudio(const std::string& path);
    bool PlayMusic(const std::string& path);
    void StopMusic();
    void SetMasterVolume(float volume);

private:
    ma_engine _Engine{};
    bool _Initialized = false;

    ma_sound _Music{};
    bool _MusicInitialized = false;
};
