#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "renderwindow.h"
#include "circle.h"

void RenderWindow::drawLine(int x1, int y1, int x2, int y2, SDL_Colour lineColour)
{
    SDL_SetRenderDrawColor(renderer, lineColour.r, lineColour.g, lineColour.b, lineColour.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void RenderWindow::drawRect(SDL_Rect rect, SDL_Colour rectColour)
{
    SDL_SetRenderDrawColor(renderer, rectColour.r, rectColour.g, rectColour.b, rectColour.a);
    // scaleRect(rect);
    SDL_RenderFillRect(renderer, &rect);
}

void RenderWindow::drawRect(int x, int y, int w, int h, SDL_Colour rectColour)
{
    SDL_Rect rect = {(x), (y), (w), (h)};
    // scaleRect(rect);
    SDL_SetRenderDrawColor(renderer, rectColour.r, rectColour.g, rectColour.b, rectColour.a);
    SDL_RenderFillRect(renderer, &rect);
}

void RenderWindow::drawRectF(SDL_FRect FRect, SDL_Colour rectColour)
{
    SDL_SetRenderDrawColor(renderer, rectColour.r, rectColour.g, rectColour.b, rectColour.a);
    // scaleRect(rect);
    SDL_RenderFillRectF(renderer, &FRect);
}

void RenderWindow::drawRectF(float x, float y, float w, float h, SDL_Colour rectColour)
{
    SDL_FRect FRect = {(x), (y), (w), (h)};
    // scaleRect(rect);
    SDL_SetRenderDrawColor(renderer, rectColour.r, rectColour.g, rectColour.b, rectColour.a);
    SDL_RenderFillRectF(renderer, &FRect);
}

void RenderWindow::drawOutline(SDL_Rect rect, SDL_Colour outlineColour)
{
    // scaleRect(rect);
    SDL_SetRenderDrawColor(renderer, outlineColour.r, outlineColour.g, outlineColour.b, outlineColour.a);

    /* jank solution i made so emscripten would draw the rect*/
    SDL_RenderDrawLine(renderer, rect.x + 1000, rect.y + 1000, rect.x + rect.w + 1000, rect.y + rect.h + 1000);
    SDL_RenderDrawRect(renderer, &rect);
}