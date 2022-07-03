#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "renderwindow.h"

RenderWindow::RenderWindow(const char *p_title, int p_w, int p_h)
    : window(NULL), renderer(NULL), windowWidth(p_w), windowHeight(p_h), originalWidth(p_w), originalHeight(p_h)
{
    window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        std::cout << "failed to init window haha: " << SDL_GetError() << "\n";
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_GetCurrentDisplayMode(0, &displayMode);
}

RenderWindow::~RenderWindow()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}

SDL_Texture *RenderWindow::loadTexture(const char *filePath)
{
    SDL_Texture *texture = NULL;
    texture = IMG_LoadTexture(renderer, filePath);
    if (texture == NULL)
    {
        std::cout << "failed to load texture at " << filePath << SDL_GetError() << "\n";
    }
    return texture;
}

SDL_Texture* RenderWindow::loadTextureFromRWops(SDL_RWops *rwop)
{
    SDL_Texture* texture = NULL;
    texture = IMG_LoadTexture_RW(renderer, rwop, 1);
    if (texture == NULL)
    {
        std::cout << "failed to load texture from rwop " << SDL_GetError() << "\n";
    }
    return texture;
}

SDL_Surface *RenderWindow::loadSurface(const char *filePath)
{
    SDL_Surface *surface = NULL;
    surface = IMG_Load(filePath);
    if (surface == NULL)
    {
        std::cout << "failed to load surface at " << filePath << SDL_GetError() << "\n";
    }
    return surface;
}

void RenderWindow::clear()
{
    SDL_RenderClear(renderer);
}

void RenderWindow::render(SDL_Texture *texture)
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
}

void RenderWindow::render(SDL_Texture *texture, SDL_Rect textureRect)
{
    SDL_RenderCopy(renderer, texture, NULL, &textureRect);
}

void RenderWindow::render(SDL_Texture *texture, float p_x, float p_y)
{
    SDL_Rect textureRect;
    SDL_QueryTexture(texture, NULL, NULL, &textureRect.w, &textureRect.y);
    textureRect.x = p_x;
    textureRect.y = p_y;
    SDL_RenderCopy(renderer, texture, NULL, &textureRect);
}

void RenderWindow::render(SDL_Surface *surface)
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_DestroyTexture(texture);
}

void RenderWindow::render(SDL_Surface *surface, SDL_Rect surfaceRect)
{
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderCopy(renderer, texture, NULL, &surfaceRect);
    SDL_DestroyTexture(texture);
}

void RenderWindow::render(SDL_Surface *surface, float p_x, float p_y)
{
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textureRect;
    textureRect.w = surface->w;
    textureRect.h = surface->h;
    textureRect.x = p_x;
    textureRect.y = p_y;
    SDL_RenderCopy(renderer, texture, NULL, &textureRect);
    SDL_DestroyTexture(texture);
}

void RenderWindow::render(Entity &entity)
{
    SDL_Rect srcRect;
    srcRect.x = entity.getRect().x;
    srcRect.y = entity.getRect().y;
    srcRect.w = entity.getRect().w;
    srcRect.h = entity.getRect().h;
    SDL_Rect dstRect;
    dstRect.x = entity.getX();
    dstRect.y = entity.getY();
    dstRect.w = entity.getRect().w * entity.getScaleX();
    dstRect.h = entity.getRect().h * entity.getScaleY();

    SDL_RenderCopy(renderer, entity.getTex(), &srcRect, &dstRect);
}

void RenderWindow::renderCentered(TTF_Font *font, const char *text, SDL_Colour colour, float p_x, float p_y)
{
    SDL_Surface *message = TTF_RenderText_Blended(font, text, colour);
    SDL_Texture *messageTex = SDL_CreateTextureFromSurface(renderer, message);
    SDL_Rect messageRect;
    SDL_QueryTexture(messageTex, NULL, NULL, &messageRect.w, &messageRect.h);
    messageRect.x = p_x - messageRect.w / 2;
    messageRect.y = p_y - messageRect.h / 2;

    SDL_RenderCopy(renderer, messageTex, NULL, &messageRect);

    SDL_DestroyTexture(messageTex);
    SDL_FreeSurface(message);
}

void RenderWindow::setDrawColour(SDL_Colour backgroundColour)
{
    SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
}

void RenderWindow::display()
{
    SDL_RenderPresent(renderer);
}

void RenderWindow::setTitle(const char *title)
{
    SDL_SetWindowTitle(window, title);
}
void RenderWindow::setIcon(SDL_Surface *icon)
{
    SDL_SetWindowIcon(window, icon);
}

int RenderWindow::getWindowWidth()
{
    return windowWidth;
}

int RenderWindow::getWindowHeight()
{
    return windowHeight;
}

float RenderWindow::getWidthScale()
{
    return (float)getWindowWidth() / (float)originalWidth;
}

float RenderWindow::getHeightScale()
{
    return (float)getWindowHeight() / (float)originalHeight;
}

void RenderWindow::scaleRect(SDL_Rect &rect)
{
    if (getWidthScale() < getHeightScale())
    {
        rect.w *= getWidthScale();
        rect.h *= getWidthScale();
        rect.x *= getWidthScale();
        rect.y *= getWidthScale();
    }
    else
    {
        rect.w *= getHeightScale();
        rect.h *= getHeightScale();
        rect.x *= getHeightScale();
        rect.y *= getHeightScale();
    }
}