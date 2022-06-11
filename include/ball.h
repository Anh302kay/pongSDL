#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "renderwindow.h"

struct ball
{
    ball(int p_x, int p_y, int p_w, int p_h) : x(p_x), y(p_y), w(p_w), h(p_h) {};
    float velocityX = ((rand() % 100 + 1) > 50) ? (rand() % 500 * 2) / 1000.0f : -((rand() % 500 * 2) / 1000.0f);
    //float velocityX = (rand() % 1000 * 2) / 1000.0f - 1.0f;
    float velocityY = (rand() % 1000 * 2) / 1000.0f - 1.0f;
    int direction;
    union
    {
        struct
        {
            mutable float x, y, w, h;
        };
        SDL_FRect ballRect;
    };
};