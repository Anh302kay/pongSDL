#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "renderwindow.h"
#ifndef __EMSCRIPTEN__
#include "font/gravitybold8.h"

#include "gfx/pongSDL_icon.h"
#include "gfx/p1Inst.h"
#include "gfx/p2Inst.h"
#endif
#include "sound/bounce.h"
#include "sound/hit.h"
#include "sound/score.h"

#include "ball.h"
#include "paddle.h"
// command to compile under emscripten
/* em++ -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sUSE_SDL_TTF=2 -sUSE_SDL_MIXER=2 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 -s SDL2_IMAGE_FORMATS='["png"]' -std=c++20 -Iinclude -Ires source/main.cpp source/window_draw.cpp source/window_events.cpp source/renderwindow.cpp source/entity.cpp --preload-file .\res -o b.html --use-preload-plugins -mnontrapping-fptoint
 */

bool init()
{
    srand((unsigned)time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0)
        std::cout << "could not init sdl 2" << SDL_GetError() << "\n";
    if (!(IMG_Init(IMG_INIT_PNG)))
        std::cout << "IMG_INT HAS FAILED SDL_ERROR: " << SDL_GetError() << "\n";
    if (TTF_Init() == -1)
        std::cout << "ttf_init HAS FAILED SDL_ERROR: " << SDL_GetError() << "\n";
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        std::cout << "sdl mixer has dfailed lol: " << Mix_GetError() << "\n";

    return true;
}

SDL_Surface *createSurfaceFromText(const char *text, TTF_Font *font, SDL_Colour colour)
{
    SDL_Surface *message = TTF_RenderText_Blended(font, text, colour);
    return message;
}

bool SDLinit = init();
bool twoPlayer = true;
Uint64 collisionTimer = SDL_GetTicks64();

RenderWindow window("Pong", 800, 600);
ball ball1(400, 200, 15, 15);
paddle paddle1(20, 230, 15, 60);
paddle paddle2(765, 230, 15, 60);

Uint64 currentTick = SDL_GetPerformanceCounter();
Uint64 lastTick = 0;
double deltaTime = 0;
bool gameRunning = true;
bool titleScreen = true;
SDL_Event e;

// loads all resources
#ifdef __EMSCRIPTEN__
TTF_Font *font32 = TTF_OpenFont("res/font/GravityBold8.ttf", 32);

TTF_Font *font16 = TTF_OpenFont("res/font/GravityBold8.ttf", 16);

// sets icon
SDL_Surface *icon = IMG_Load("res/gfx/pongSDL_icon.png");

SDL_RWops *bounceRW = SDL_RWFromConstMem(bounce_wav, bounce_wav_size);
Mix_Chunk *bounce = Mix_LoadWAV_RW(bounceRW, 1);

SDL_RWops *hitRW = SDL_RWFromConstMem(hit_wav, hit_wav_size);
Mix_Chunk *hit = Mix_LoadWAV_RW(hitRW, 1);

SDL_RWops *scoreRW = SDL_RWFromConstMem(score_wav, score_wav_size);
Mix_Chunk *score = Mix_LoadWAV_RW(scoreRW, 1);

SDL_Texture *p1Inst = window.loadTexture("res/gfx/p1Instructions.png");

SDL_Texture *p2Inst = window.loadTexture("res/gfx/p2Instructions.png");
#else
SDL_RWops *font32RW = SDL_RWFromConstMem(GravityBold8_ttf, GravityBold8_ttf_size);
TTF_Font *font32 = TTF_OpenFontRW(font32RW, 1, 32);

SDL_RWops *font16RW = SDL_RWFromConstMem(GravityBold8_ttf, GravityBold8_ttf_size);
TTF_Font *font16 = TTF_OpenFontRW(font16RW, 1, 16);

// sets icon
SDL_RWops *iconRW = SDL_RWFromConstMem(pongSDL_icon_png, pongSDL_icon_png_size);
SDL_Surface *icon = IMG_Load_RW(iconRW, 1);

SDL_RWops *bounceRW = SDL_RWFromConstMem(bounce_wav, bounce_wav_size);
Mix_Chunk *bounce = Mix_LoadWAV_RW(bounceRW, 1);

SDL_RWops *hitRW = SDL_RWFromConstMem(hit_wav, hit_wav_size);
Mix_Chunk *hit = Mix_LoadWAV_RW(hitRW, 1);

SDL_RWops *scoreRW = SDL_RWFromConstMem(score_wav, score_wav_size);
Mix_Chunk *score = Mix_LoadWAV_RW(scoreRW, 1);

SDL_RWops *p1InstRW = SDL_RWFromConstMem(p1Instructions_png, p1Instructions_png_size);
SDL_Texture *p1Inst = window.loadTextureFromRWops(p1InstRW);

SDL_RWops *p2InstRW = SDL_RWFromConstMem(p2Instructions_png, p2Instructions_png_size);
SDL_Texture *p2Inst = window.loadTextureFromRWops(p2InstRW);
#endif
SDL_Surface *playButton = createSurfaceFromText("Play", font16, {255, 255, 255, 255});
float playScale = 1.5f;

SDL_Surface* playerButton = createSurfaceFromText("Two Players: On", font16, {255,255,255,255});
float playerScale = 1.5f;

void drawPaddle()
{
    window.drawRectF(paddle1.paddleRect, {255, 255, 255, 255});
    window.drawRectF(paddle2.paddleRect, {255, 255, 255, 255});
}

void paddleCollision(paddle &paddle, Mix_Chunk *hit)
{
    if (paddle.y < 0)
        paddle.y = 0;

    if (paddle.y + paddle.h > window.getWindowHeight())
        paddle.y = window.getWindowHeight() - paddle.h;

    SDL_Rect paddlerect = {(int)paddle.x, (int)paddle.y, (int)paddle.w, (int)paddle.h};
    SDL_Rect ballrect = {(int)ball1.x, (int)ball1.y, (int)ball1.w, (int)ball1.h};
    if (SDL_HasIntersection(&paddlerect, &ballrect) && SDL_GetTicks64() > collisionTimer)
    {
        collisionTimer = SDL_GetTicks64() + 500;
        // ball.velocityX = -ball.velocityX;
        // ball.velocityY = -ball.velocityY;
        if ((ball1.y - paddle.y) / paddle.h < 0.33)
        {
            // ball.velocityX = -ball.velocityX + 0.2f;
            ball1.velocityX = (ball1.velocityX > 0) ? (-ball1.velocityX) - 0.2 : (-ball1.velocityX) + 0.2;
            ball1.velocityY = (ball1.velocityY > 0) ? (-ball1.velocityY) - 0.2 : (ball1.velocityY) - 0.2;
            Mix_PlayChannel(-1, hit, 0);
        }
        else if ((ball1.y - paddle.y) / paddle.h < 0.66)
        {
            // ball.velocityX = -ball.velocityX + 0.2f;
            ball1.velocityX = (ball1.velocityX > 0) ? (-ball1.velocityX) - 0.2 : (-ball1.velocityX) + 0.2;
            ball1.velocityY = (rand() % 500 * 2) / 1000.0f;
            Mix_PlayChannel(-1, hit, 0);
        }
        else if ((ball1.y - paddle.y) / paddle.h < 1)
        {
            // ball.velocityX = -ball.velocityX + 0.2f;
            ball1.velocityX = (ball1.velocityX > 0) ? (-ball1.velocityX) - 0.2 : (-ball1.velocityX) + 0.2;
            ball1.velocityY = (ball1.velocityY > 0) ? (ball1.velocityY) + 0.2 : (-ball1.velocityY) + 0.2;
            Mix_PlayChannel(-1, hit, 0);
        }
    }
}

void drawBall()
{
    ball1.x = ball1.x;
    ball1.y = ball1.y;
    window.drawRectF(ball1.ballRect, {255, 255, 255, 255});
}

void ballMovement(Mix_Chunk* bounce)
{
    ball1.x += ball1.velocityX;
    ball1.y += ball1.velocityY;

    if (ball1.y < 0)
    {
        ball1.velocityY = -ball1.velocityY;
        Mix_PlayChannel(-1, bounce, 0);
    }
    else if (ball1.y + ball1.w > window.getWindowHeight())
    {
        ball1.velocityY = -ball1.velocityY;
        Mix_PlayChannel(-1, bounce, 0);
    }
}

void gameloop()
{
    if (titleScreen)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        lastTick = currentTick;
        currentTick = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentTick - lastTick) * 1000 / (double)SDL_GetPerformanceFrequency());

        SDL_Rect mouse = {mouseX, mouseY, 1, 1};
        SDL_Rect play = {(int)((window.getWindowWidth() / 2) - (playButton->w * (playScale + 1.0f)) / 2),
                         160 - (int)((playButton->h * (playScale + 1.0f)) / 2),
                         (int)(playButton->w * (playScale + 1.0f)),
                         (int)(playButton->h * (playScale + 1.0f))};
        SDL_Rect player = {(int)(window.getWindowWidth() / 2 - playerButton->w * playerScale / 2), (int)(220 - playerButton->h * playerScale / 2), (int)(playerButton->w * playerScale), (int)(playerButton->h * playerScale)};
        SDL_Rect playerOutline = {(int)(window.getWindowWidth() / 2 - playerButton->w * (playerScale + 1.0f) / 2), (int)(220 - playerButton->h * (playerScale + 1.0f) / 2), (int)(playerButton->w * (playerScale + 1.0f)), (int)(playerButton->h * (playerScale + 1.0f))};

        if (SDL_HasIntersection(&mouse, &playerOutline))
        {
            playerScale = 2.5f;
        }
        else
        {
            playerScale = 1.5f;
        }

        if (SDL_HasIntersection(&mouse, &play))
        {
            playScale = 2.5f;
        }
        else
        {
            playScale = 1.5f;
        }

        SDL_PumpEvents();
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                gameRunning = false;
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (SDL_HasIntersection(&mouse, &play))
                    titleScreen = false;
                if (SDL_HasIntersection(&mouse, &playerOutline))
                {
                    twoPlayer = !twoPlayer;
                    if(twoPlayer)
                    {
                        playerButton = createSurfaceFromText("Two Players: On", font16, {255, 255, 255, 255});
                    }
                    else
                    {
                        playerButton = createSurfaceFromText("Two Players: Off", font16, {255, 255, 255, 255});
                    }
                    
                }
            }
        }
        const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

        if (currentKeyStates[SDL_SCANCODE_W])
            paddle1.y -= 0.5 * deltaTime;
        if (currentKeyStates[SDL_SCANCODE_S])
            paddle1.y += 0.5 * deltaTime;
        if (currentKeyStates[SDL_SCANCODE_UP])
            paddle2.y -= 0.5 * deltaTime;
        if (currentKeyStates[SDL_SCANCODE_DOWN])
            paddle2.y += 0.5 * deltaTime;

        if (paddle1.y - 20 <= 0)
        {
            paddle1.y = 20;
        }
        if (paddle2.y - 20 <= 0)
        {
            paddle2.y = 20;
        }
        if (paddle1.y + paddle1.h + 20 > window.getWindowHeight())
        {
            paddle1.y = window.getWindowHeight() - paddle1.h - 20;
        }
        if (paddle2.y + paddle2.h + 20 > window.getWindowHeight())
        {
            paddle2.y = window.getWindowHeight() - paddle2.h - 20;
        }

        window.setDrawColour({0, 0, 0, 0});
        window.clear();

        window.render(p1Inst, {35, ((int)paddle1.y - 20), 35, 103});
        if(twoPlayer)
        window.render(p2Inst, {720, ((int)paddle2.y - 20), 35, 103});

        drawPaddle();

        window.renderCentered(font32, "PongSDL", {255, 255, 255, 255}, window.getWindowWidth() / 2, 100);
        window.drawOutline({(int)((window.getWindowWidth() / 2) - (playButton->w * (playScale + 1.0f)) / 2),
                            160 - (int)((playButton->h * (playScale + 1.0f)) / 2),
                            (int)(playButton->w * (playScale + 1.0f)),
                            (int)(playButton->h * (playScale + 1.0f))},
                           {255, 255, 255, 255});
        window.drawOutline(playerOutline, {255,255,255,255});
        window.render(playButton, {(int)((window.getWindowWidth() / 2) - (playButton->w * playScale) / 2),
                                   160 - (int)((playButton->h * playScale) / 2),
                                   (int)(playButton->w * playScale),
                                   (int)(playButton->h * playScale)});
        window.render(playerButton, player);

        window.display();
    }
    else
    {
        lastTick = currentTick;
        currentTick = SDL_GetPerformanceCounter();
        deltaTime = (double)((currentTick - lastTick) * 1000 / (double)SDL_GetPerformanceFrequency());

        const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

        SDL_PumpEvents();
        while (SDL_PollEvent(&e))
        {

            if (e.type == SDL_QUIT)
                gameRunning = false;

            window.handleEvents(e);
        }
        // input
        if (currentKeyStates[SDL_SCANCODE_ESCAPE])
            gameRunning = false;

        // paddle movement
        if (currentKeyStates[SDL_SCANCODE_W])
            paddle1.y -= 0.5 * deltaTime;
        if (currentKeyStates[SDL_SCANCODE_S])
            paddle1.y += 0.5 * deltaTime;
        if(twoPlayer)
        {
            if (currentKeyStates[SDL_SCANCODE_UP])
                paddle2.y -= 0.5 * deltaTime;
            if (currentKeyStates[SDL_SCANCODE_DOWN])
                paddle2.y += 0.5 * deltaTime;
        }
        else
        {
            paddle2.y = ball1.y;
        }

        // checks if ball is out of bounds
        if (ball1.x > window.getWindowWidth())
        {
            ball1.velocityX = (rand() % 500 * 2) / 1000.0f;
            ball1.velocityY = (rand() % 500 * 2) / 1000.0f;
            paddle1.score += 1;
            ball1.x = window.getWindowWidth() / 2 + ball1.w / 2;
            ball1.y = window.getWindowHeight() / 2 + ball1.w / 2;
            Mix_PlayChannel(-1, score, 0);
        }
        if (ball1.x + ball1.w < 0)
        {
            ball1.velocityX = -((rand() % 500 * 2) / 1000.0f);
            ball1.velocityY = -((rand() % 500 * 2) / 1000.0f);
            paddle2.score += 1;
            ball1.x = window.getWindowWidth() / 2 + ball1.w / 2;
            ball1.y = window.getWindowHeight() / 2 + ball1.w / 2;
            Mix_PlayChannel(-1, score, 0);
        }

        paddleCollision(paddle1, hit);
        paddleCollision(paddle2, hit);

        if (window.hasKeyboardFocus())
            ballMovement(bounce);

        // rendering code
        window.setDrawColour({0, 0, 0, 0});
        window.clear();

        drawPaddle();
        drawBall();
        window.renderCentered(font32, std::to_string(paddle2.score).c_str(), {255, 255, 255, 255}, 600, 100);
        window.renderCentered(font32, std::to_string(paddle1.score).c_str(), {255, 255, 255, 255}, 200, 100);
        window.drawRect({window.getWindowWidth() / 2 - 2, 20, 4, window.getWindowHeight() - 40}, {255, 255, 255, 255});

        window.display();
    }
}
int main(int argc, char *argv[])
{

    window.setDrawColour({0, 0, 0, 0});
    window.setIcon(icon);
    SDL_FreeSurface(icon);
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(gameloop, 0, 1);
#else
    while (gameRunning)
    {
        gameloop();
    }
#endif
    SDL_FreeSurface(playerButton);
    SDL_FreeSurface(playButton);

    SDL_DestroyTexture(p2Inst);
    SDL_DestroyTexture(p1Inst);

    Mix_FreeChunk(score);
    Mix_FreeChunk(hit);
    Mix_FreeChunk(bounce);
    TTF_CloseFont(font16);
    TTF_CloseFont(font32);
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}