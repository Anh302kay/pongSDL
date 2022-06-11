#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "renderwindow.h"

void RenderWindow::handleEvents(SDL_Event &e)
{

    if (e.type == SDL_WINDOWEVENT)
    {
        switch (e.window.event)
        {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            windowWidth = e.window.data1;
            windowHeight = e.window.data2;
            SDL_RenderPresent(renderer);
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            keyboardFocus = true;
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            keyboardFocus = false;
            break;
        default:
            break;
        }
    }
}

bool RenderWindow::hasKeyboardFocus()
{
    return keyboardFocus;
}