#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "entity.h"

class RenderWindow
{
public:
    RenderWindow(const char *p_title, int p_w, int p_h);
    ~RenderWindow();
    SDL_Texture *loadTexture(const char *filePath);
    SDL_Texture *loadTextureFromRWops(SDL_RWops* rwop);
    SDL_Surface *loadSurface(const char *filePath);
    void clear();
    void render(SDL_Texture *texture);
    void render(SDL_Texture *texture, SDL_Rect textureRect);
    void render(SDL_Texture *texture, float p_x, float p_y);
    void render(SDL_Surface *surface);
    void render(SDL_Surface *surface, SDL_Rect surfaceRect);
    void render(SDL_Surface *surface, float p_x, float p_y);
    void render(Entity &entity);
    void renderCentered(TTF_Font *font, const char *text, SDL_Colour colour, float p_x, float p_y);
    void setTitle(const char *title);
    void setIcon(SDL_Surface *icon);
    void drawLine(int x1, int y1, int x2, int y2, SDL_Colour lineColour);
    void drawRect(SDL_Rect rect, SDL_Colour rectColour);
    void drawRectF(float x, float y, float w, float h, SDL_Colour rectColour);
    void drawRectF(SDL_FRect FRect, SDL_Colour rectColour);
    void drawRect(int x, int y, int w, int h, SDL_Colour rectColour);
    void drawOutline(SDL_Rect rect, SDL_Colour outlineColour);
    void drawCircle(int x, int y, int radius, SDL_Colour circleColour);
    void drawCircle(SDL_Rect rect, SDL_Colour circleColour);
    void setDrawColour(SDL_Colour backgroundColour);
    void display();
    void handleEvents(SDL_Event &e);
    SDL_DisplayMode getDisplayMode() const { return displayMode; }
    int getWindowWidth();
    int getWindowHeight();
    float getWidthScale();
    float getHeightScale();
    bool hasKeyboardFocus();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_DisplayMode displayMode;
    int windowWidth = 800;
    int windowHeight = 600;
    const int originalWidth;
    const int originalHeight;
    bool keyboardFocus = true;
    void scaleRect(SDL_Rect &rect);
};