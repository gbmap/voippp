
#pragma once

#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

SDL_AudioDeviceID select_audio_device(
    int iscapture, SDL_AudioCallback callback, SDL_AudioSpec& spec, bool auto_select=true
) {
    // list mic devices
    int i, count = SDL_GetNumAudioDevices(iscapture); // 1 = mic
    for (i = 0; i < count; ++i) {
        cout << "Audio device " << i << ": " << SDL_GetAudioDeviceName(i, iscapture) << endl;
    }

    if (!auto_select)
    {
        // select device
        cout << "Select device: ";
        cin >> i;
    }
    else
    {
        i = 0;
    }

    SDL_GetAudioDeviceSpec(i, 1, &spec);

    // Set mic callback.
    spec.callback = callback;

    // Attempt to open device with two channels.
    spec.channels = 2;

    SDL_AudioDeviceID device_id =  SDL_OpenAudioDevice(
        SDL_GetAudioDeviceName(i, iscapture), iscapture, &spec, 
        &spec, 0
    );

    cout << "Name: " << SDL_GetAudioDeviceName(i, iscapture) << endl
         << "Samples: " << spec.samples << endl
         << "Format: " << spec.format << endl
         << "Freq: " << spec.freq << endl
         << "Silence: " << spec.silence << endl
         << "Size: " << spec.size << endl
         << "Channels: " << int(spec.channels) << endl;

    if (device_id == 0)
        throw runtime_error("Failed to open audio device");

    return device_id;

}

void pause_device(SDL_AudioDeviceID device_id, int pause)
{
    SDL_PauseAudioDevice(device_id, pause);
}

void close_device(SDL_AudioDeviceID device_id)
{
    SDL_CloseAudioDevice(device_id);
}
