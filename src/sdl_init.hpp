

#pragma once

#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>

using namespace std;

SDL_Window* g_pWindow;
SDL_Renderer* g_pRenderer;

SDL_Point* g_pTopGraphPoints;
SDL_Point* g_pBottomGraphPoints;

#define WAVEFORM_RESOLUTION 128

int w, h;

bool should_use_window(int argc, char** argv)
{
    if (argc == 1)
        return 1;

    bool use_window = 0;
    try
    {
        use_window = (bool)atoi(argv[1]);
    }
    catch (...)
    {
        cout << "Usage: \n\t" << "arg 0: 0/1 (display waveform)" << endl;
        return 0;
    }

    return use_window;
}

void sdl_init(bool use_window)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    Uint32 window_flags = SDL_WINDOW_SHOWN;
    w = h = use_window ? 640 : 0;

    g_pWindow = SDL_CreateWindow(
        "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, window_flags
    );

    if (!use_window)
        return;

    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (g_pRenderer == 0)
        throw runtime_error("Could not create renderer.");

    g_pTopGraphPoints = new SDL_Point[WAVEFORM_RESOLUTION];
    g_pBottomGraphPoints = new SDL_Point[WAVEFORM_RESOLUTION];

    // Initialize graph points with the correct X coordinate.
    // Leave y coordinates to when buffer is received.
    for (int i = 0; i < WAVEFORM_RESOLUTION; i++)
    {
        int x = (int)(i/(WAVEFORM_RESOLUTION-1.0) * w);
        g_pTopGraphPoints = new SDL_Point{x, 0};
        g_pBottomGraphPoints = new SDL_Point{x, 0};
    }

    SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 125, 255);
}

void sdl_deinit()
{
    if (g_pRenderer)
        SDL_DestroyRenderer(g_pRenderer);
    
    if (g_pWindow)
        SDL_DestroyWindow(g_pWindow);

    if (g_pTopGraphPoints)
    {
        delete g_pTopGraphPoints;
        delete g_pBottomGraphPoints;
    }
}

void sdl_prepare_render()
{
    SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(g_pRenderer);
}

void sdl_draw_waveform(const Uint8* buffer, int buffer_length, Uint8 index)
{
    SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 125, 255);
    SDL_Point* pPoints = index == 0 ? g_pTopGraphPoints : g_pBottomGraphPoints;

    int half_h = h/2;
    int y_base = half_h*index;
    int buffer_index_step = buffer_length / WAVEFORM_RESOLUTION;
    for (int i = 0; i < WAVEFORM_RESOLUTION; i++) 
    {
        int array_index = buffer_index_step*i;
        pPoints[i].y = (int)(y_base + (buffer[array_index] / 255.0)*half_h); 
    }

    SDL_RenderDrawLines(g_pRenderer, pPoints, WAVEFORM_RESOLUTION);
}

void sdl_render()
{
    SDL_RenderPresent(g_pRenderer);
}