#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include "game.h"

bool asteroids_generate(
    struct Asteroid **asteroids,
    SDL_Renderer *renderer,
    SDL_Texture *large_image,
    SDL_Texture *medium_image,
    SDL_Texture *small_image,
    unsigned int count
);
void asteroids_free(struct Asteroid **asteroids);
void asteroids_update(struct Asteroid *a, double dt);
bool asteroids_draw(struct Asteroid *a);

#endif