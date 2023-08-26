#ifndef SHOT_H
#define SHOT_H

#include "main.h"

struct Shot {
    SDL_Renderer *renderer;
    SDL_Texture *image;
    SDL_Rect rect;
    double x_pos;
	double y_pos;
    double speed;
	double x_vel;
	double y_vel;
	double angle;
	double radius;
    double life_time;
    double shot_count;
    bool alive;
};

bool shot_initialize(struct Shot **shot, SDL_Renderer *renderer, SDL_Texture *image);
void shot_free(struct Shot *s);
void shot_new(struct Shot *s, double x, double y, double angle, double radius);
double shot_left(struct Shot *s);
double shot_right(struct Shot *s);
double shot_top(struct Shot *s);
double shot_bottom(struct Shot *s);
double shot_center_x(struct Shot *s);
double shot_center_y(struct Shot *s);
double shot_radius(struct Shot *s);
void shot_update(struct Shot *s, double dt);
bool shot_draw(struct Shot *s);

#endif