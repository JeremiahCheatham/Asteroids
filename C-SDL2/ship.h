#ifndef SHIP_H
#define SHIP_H

#include "main.h"

struct Ship {
    SDL_Renderer *renderer;
    SDL_Texture *image1;
    SDL_Texture *image2;
    SDL_Rect rect;
    double x_pos;
	double y_pos;
	double x_vel;
	double y_vel;
	double offset;
	double size;
	double angle;
	double angle_rate;
	double thrust;
	bool engine;
    const Uint8 *keystate;
	float radius;
	bool immunity;
	double immunity_counter;
};

bool ship_new(struct Ship **ship, SDL_Renderer *renderer, SDL_Texture *image1, SDL_Texture *image2);
void ship_free(struct Ship **ship);
void ship_reset(struct Ship *s);
Uint32 ship_immunity_event(Uint32 interval, void *param);
void ship_set_immunity(struct Ship *s);
double ship_left(struct Ship *s);
double ship_right(struct Ship *s);
double ship_top(struct Ship *s);
double ship_bottom(struct Ship *s);
double ship_center_x(struct Ship *s);
double ship_center_y(struct Ship *s);
double ship_radius(struct Ship *s);
void ship_update(struct Ship *s, double dt);
bool ship_draw(struct Ship *s);

#endif