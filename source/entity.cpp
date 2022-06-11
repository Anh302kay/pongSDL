#include <iostream>
#include <SDL2/SDL.h>

#include "entity.h"

Entity::Entity()
{
    entityX = 0;
    entityY = 0;
    entityTex = NULL;
}

Entity::Entity(SDL_Texture *texture, float x, float y)
    : entityTex(texture), entityX(x), entityY(y)
{
    SDL_QueryTexture(entityTex, NULL, NULL, &entityRect.w, &entityRect.h);
}

Entity::~Entity()
{
    free();
}

void Entity::free()
{
    if(entityTex != NULL)
    {
        SDL_DestroyTexture(entityTex);
        entityTex = NULL;
    }
}

void Entity::setTex(SDL_Texture* tex)
{
    entityTex = tex;
    SDL_QueryTexture(entityTex, NULL, NULL, &entityRect.w, &entityRect.h);
}

void Entity::setXY(float x, float y)
{
    entityX = x;
    entityY = y;
}

void Entity::setScale(float x, float y)
{
    scaleX = x;
    scaleY = y;
}

void Entity::setGrabbed(bool grab)
{
    grabbed = grab;
}

void Entity::setColourMod(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(entityTex, r, g, b);
}