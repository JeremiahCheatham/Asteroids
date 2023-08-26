#include "load_media.h"

bool game_load_media(struct Game *g) {

    g->background_image = IMG_LoadTexture(g->renderer, "images/background.png");
    if (!g->background_image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }

    if (SDL_QueryTexture(g->background_image, NULL, NULL, &g->background_rect.w, &g->background_rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return true;
    }

    g->ship_image1 = IMG_LoadTexture(g->renderer, "images/ship1.png");
    if (!g->ship_image1) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }

    g->ship_image2 = IMG_LoadTexture(g->renderer, "images/ship2.png");
    if (!g->ship_image2) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }

    g->asteroid_large_image = IMG_LoadTexture(g->renderer, "images/asteroidlarge.png");
    if (!g->asteroid_large_image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }

    g->asteroid_medium_image = IMG_LoadTexture(g->renderer, "images/asteroidmedium.png");
    if (!g->asteroid_medium_image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }

    g->asteroid_small_image = IMG_LoadTexture(g->renderer, "images/asteroidsmall.png");
    if (!g->asteroid_small_image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }

    g->icon_image = IMG_LoadTexture(g->renderer, "images/icon.png");
    if (!g->icon_image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }

    if (SDL_QueryTexture(g->icon_image, NULL, NULL, &g->icon_rect.w, &g->icon_rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return true;
    }

    g->shot_image = IMG_LoadTexture(g->renderer, "images/shot.png");
    if (!g->shot_image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }


    g->falling_music = Mix_LoadMUS( "music/falling.ogg" );
    if(!g->falling_music) {
        fprintf(stderr, "Failed to load music: %s\n", Mix_GetError());
        return true;
    }

    g->boom_sound = Mix_LoadWAV("sounds/boom.ogg");
    if(!g->boom_sound) {
        fprintf(stderr, "Failed to load sound effect: %s\n", Mix_GetError());
        return true;
    }

    g->collide_sound = Mix_LoadWAV("sounds/collide.ogg");
    if(!g->collide_sound) {
        fprintf(stderr, "Failed to load sound effect: %s\n", Mix_GetError());
        return true;
    }

    g->laser_sound = Mix_LoadWAV("sounds/laser.ogg");
    if(!g->laser_sound) {
        fprintf(stderr, "Failed to load sound effect: %s\n", Mix_GetError());
        return true;
    }

    g->thruster_sound = Mix_LoadWAV("sounds/thrusters.ogg");
    if(!g->thruster_sound) {
        fprintf(stderr, "Failed to load sound effect: %s\n", Mix_GetError());
        return true;
    }

    return false;
}
