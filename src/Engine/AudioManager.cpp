#include "AudioManager.h"
#include <iostream>

bool AudioManager::Initialize()
{
    if (_Initialized)
        return true;

    if (ma_engine_init(nullptr, &_Engine) != MA_SUCCESS)
    {
        std::cerr << "Audio initialization failed\n";
        return false;
    }

    _Initialized = true;
    return true;
}

void AudioManager::Shutdown()
{
    if (!_Initialized)
        return;

    ma_engine_uninit(&_Engine);
    _Initialized = false;
}

//Play a one-timer audio
bool AudioManager::PlayAudio(const std::string& path)
{
    if (!_Initialized)
        return false;

    return ma_engine_play_sound(
        &_Engine,
        path.c_str(),
        nullptr
    ) == MA_SUCCESS;
}

//Play a music in loop
bool AudioManager::PlayMusic(const std::string& path) {
    if (!_Initialized)
        return false;

    StopMusic();
    
    ma_result result = ma_sound_init_from_file(
        &_Engine,
        path.c_str(),
        MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_NO_SPATIALIZATION,
        nullptr,
        nullptr,
        &_Music
    );

    if (result != MA_SUCCESS)
        return false;

    _MusicInitialized = true;

    // Active la lecture en boucle
    ma_sound_set_looping(&_Music, MA_TRUE);

    return ma_sound_start(&_Music) == MA_SUCCESS;
}

//Stop the music that loops
void AudioManager::StopMusic()
{
    if (!_MusicInitialized)
        return;

    ma_sound_stop(&_Music);
    ma_sound_uninit(&_Music);

    _MusicInitialized = false;
}

void AudioManager::SetMasterVolume(float volume)
{
    if (_Initialized)
        ma_engine_set_volume(&_Engine, volume);
}
