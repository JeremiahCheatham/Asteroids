#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>

#define WINDOW_TITLE "Asteroids!"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TARGET_FPS 60
#define IMMUNITY_EVENT SDL_USEREVENT
#define GAME_MODE_EVENT SDL_USEREVENT + 1
#define SCORE_FONT_SIZE 20
#define MESSAGE_FONT_SIZE 100

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Length of array macro.
#define LEN(x) (sizeof(x)/sizeof(x[0]))

#endif