#ifndef GAME_H
#define GAME_H

#include "main.h"
#include "asteroid.h"
#include "message.h"
#include "score.h"
#include "ship.h"
#include "shot.h"
#include "fps.h"

struct Game {
    SDL_Event event;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *background_image;
    SDL_Rect background_rect;
    SDL_Texture *ship_image1;
    SDL_Texture *ship_image2;
    SDL_Texture *asteroid_large_image;
    SDL_Texture *asteroid_medium_image;
    SDL_Texture *asteroid_small_image;
    SDL_Texture *icon_image;
    SDL_Rect icon_rect;
    SDL_Texture *shot_image;
    Mix_Music *falling_music;
    Mix_Chunk *boom_sound;
    Mix_Chunk *collide_sound;
    Mix_Chunk *laser_sound;
    Mix_Chunk *thruster_sound;
    struct Ship *ship;
    struct Asteroid *asteroids;
    struct Shot *shots[5];
    struct Score *score;
    struct Message *message;
    struct Fps *fps;
    bool playing;
    Uint32 lives;
    Uint32 shot_count;
    Uint32 level;
    int game_mode;
    bool show_game;
    bool show_message;
    double delta_time;
};

bool game_new(struct Game **game);
void game_free(struct Game **game);
bool game_reset(struct Game *g);
bool game_shoot(struct Game *g);
Uint32 game_mode_event(Uint32 interval, void *param);
bool set_game_mode(struct Game *g);
bool check_ship_collisions(struct Game *g);
bool lives_draw(struct Game *g);
bool check_shot_collision(struct Game *g);
bool game_run(struct Game *g);

#endif