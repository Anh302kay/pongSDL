#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "renderwindow.h"

struct paddle
{
    paddle(int p_x, int p_y, int p_w, int p_h) : x(p_x), y(p_y), w(p_w), h(p_h){};
    int score = 0;
    union
    {
        struct
        {
            mutable float x, y, w, h;
        };
        SDL_FRect paddleRect;
    };
};