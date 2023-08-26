#ifndef ASTEROID_H
#define ASTEROID_H

#include "main.h"

struct Asteroid {
    SDL_Renderer *renderer;
    SDL_Texture *images[3];
    SDL_Rect rects[3];
    double radii[3];
    double x_pos;
	double y_pos;
	double x_vel;
	double y_vel;
	double angle;
    double angle_rate;
    int size;
    struct Asteroid *next;
    struct Asteroid *previous;
};

bool asteroid_new(
    struct Asteroid *a,
    SDL_Renderer *renderer,
    SDL_Texture *large_image,
    SDL_Texture *medium_image,
    SDL_Texture *small_image,
    double x_pos,
    double y_pos,
    double angle,
    double x_vel,
    double y_vel,
    int size
);
void asteroid_reset(struct Asteroid *a);
bool asteroid_split(struct Asteroid *a, double angle);
double asteroid_left(struct Asteroid *a);
double asteroid_right(struct Asteroid *a);
double asteroid_top(struct Asteroid *a);
double asteroid_bottom(struct Asteroid *a);
double asteroid_center_x(struct Asteroid *a);
double asteroid_center_y(struct Asteroid *a);
double asteriod_radius(struct Asteroid *a);
void asteroid_update(struct Asteroid *a, double dt);
bool asteroid_draw(struct Asteroid *a);

#endif