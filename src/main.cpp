
#pragma once

#include <iostream>

#include "buffer_fill.hpp"
#include "network.hpp"
#include "sdl_init.hpp"
#include "audio_device.hpp"
#undef main

BufferFill* g_pBufferFill; // holds audio data to be played. (Double buffering)
NetClient* g_pNetworkClient;

bool g_micAlwaysOn = false;

/*
    Pushes into a BufferFill the incoming network audio data, so it can
    be transfered to the output device once it asks for data.
*/
void HandleNetworkMessage(void* data, int len);

void Callback_AudioOutput(void* userdata, Uint8* stream, int len);
void Callback_AudioInput(void* userdata, Uint8* stream, int len);
void InitializeAudioDevices(
    SDL_AudioSpec& spec, 
    SDL_AudioDeviceID& output_device_id, 
    SDL_AudioDeviceID& input_device_id
);

void InitializeWindow();
void HandleInput(const SDL_Event& event, SDL_AudioDeviceID id_mic, SDL_AudioDeviceID id_speaker);
void Run(SDL_AudioDeviceID id_mic, SDL_AudioDeviceID id_speaker);

int main(int argc, char** argv)
{
    if (argc == 0)
    {
        std::cout << "Usage: " << argv[0] << " <incoming port> <outgoing port>" << std::endl;
        return 0;
    }


    int incomingPort = atoi(argv[1]);
    int outgoingPort = atoi(argv[2]);

    if (argc == 4)
    {
        std:: cout << "[MICROPHONE] KEEPING MICROPHONE OPEN, ADVERT YOUR EARS." << std::endl;
        g_micAlwaysOn = true;
    }


    std::cout << "[NETWORK] Ports configured: " << incomingPort << "(incoming), " << outgoingPort << " (outgoing)" << std::endl;

    std::cout << "[DISPLAY] Initializing..." << std::endl;
    InitializeWindow();

    // Initialize devices.
    std::cout << "[AUDIO] Initializing..." << std::endl;
    SDL_AudioSpec spec;
    SDL_AudioDeviceID output_device_id;
    SDL_AudioDeviceID input_device_id;
    InitializeAudioDevices(spec, output_device_id, input_device_id);

    // Initialize buffer to be the size of expected audio output. 
    std::cout << "Initializing audio buffer..." << std::endl;
    g_pBufferFill = new BufferFill(spec.size);

    std::cout << "[NETWORK] Initializing..." << std::endl;
    g_pNetworkClient = new NetClient(incomingPort, "127.0.0.1", outgoingPort, 1024*32, &HandleNetworkMessage);

    Run(input_device_id, output_device_id);

    sdl_deinit();
    delete g_pNetworkClient;
    delete g_pBufferFill;

    return 0;
}


void InitializeWindow()
{
    sdl_init(true);
}

/*
    \returns The size of the buffer that will be used to store incoming audio data.
*/
void InitializeAudioDevices(
    SDL_AudioSpec& spec, 
    SDL_AudioDeviceID& output_device_id, 
    SDL_AudioDeviceID& input_device_id
) {
    input_device_id = select_audio_device(
        1, Callback_AudioInput, spec, true
    );

    output_device_id = select_audio_device(
        0, Callback_AudioOutput, spec, true
    );
}

void HandleInput(const SDL_Event& event, SDL_AudioDeviceID id_mic, SDL_AudioDeviceID id_speaker)
{
    if (!g_micAlwaysOn)
    {
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                pause_device(id_mic, 0);
            }
        }
        else if (event.type == SDL_KEYUP)
        {
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                // Clear buffers so we don't get a never-ending loop.
                g_pBufferFill->Clear();
                pause_device(id_mic, 1);
            }
        }
    }
}

void Callback_AudioOutput(void* userdata, Uint8* stream, int len)
{
    // If we were sure that SDL wouldn't change the stream we could
    // pass the buffer's raw pointer to it.     
    SDL_memcpy(
        stream, 
        g_pBufferFill->Get(), 
        min((int)g_pBufferFill->GetBufferSize(), len)
    );
}

void Callback_AudioInput(void* userdata, Uint8* stream, int len)
{
    std::cout << "[NETWORK] Sending audio data " << len << "bytes." << std::endl;
    g_pNetworkClient->Send(stream, len);
}

void HandleNetworkMessage(void* data, int len)
{
    std:: cout << "[NETWORK] Received " << len << " bytes." << std::endl;
    g_pBufferFill->Push(reinterpret_cast<Uint8*>(data), (Uint32)len);
}

void Run(SDL_AudioDeviceID id_mic, SDL_AudioDeviceID id_speaker)
{
    bool should_quit = false;
    SDL_Event event;

    pause_device(id_mic, !g_micAlwaysOn);
    pause_device(id_speaker, g_micAlwaysOn);

    while (!should_quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                should_quit = true;
                break;
            }
            HandleInput(event, id_mic, id_speaker);
        }

        sdl_prepare_render();
        sdl_render();
    }
}
