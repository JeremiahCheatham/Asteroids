#include "shot.h"


bool shot_initialize(struct Shot **shot, SDL_Renderer *renderer, SDL_Texture *image) {
    *shot = calloc(1, sizeof(struct Shot));
    if (!*shot) {
        fprintf(stderr, "Error in calloc of new shot.\n");
        return true;
    }
    struct Shot *s = *shot;

    s->renderer = renderer;
    s->image = image;

    if (SDL_QueryTexture(s->image, NULL, NULL, &s->rect.w, &s->rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return true;
    }

    s->speed = 600;
    s->life_time = 40;
    s->shot_count = 0;
    s->alive = false;

    return false;
}

void shot_free(struct Shot *s) {
    if (s) {
        free(s);
    }
}

void shot_new(struct Shot *s, double x, double y, double angle, double radius) {
    s->angle = angle;
    double radian = s->angle * M_PI / 180;
    s->x_vel = sin(radian) * s->speed;
    s->y_vel = -cos(radian) * s->speed;
    s->x_pos = x - (s->rect.w / 2.0);
    s->y_pos = y - (s->rect.h / 2.0);
    s->x_pos += sin(radian) * radius;
    s->y_pos -= cos(radian) * radius;
    s->angle = angle;
    s->shot_count = 0;
    s->alive = true;
}

double shot_left(struct Shot *s) {
    return s->x_pos;
}

double shot_right(struct Shot *s) {
    return s->x_pos + s->rect.w;
}

double shot_top(struct Shot *s) {
    return s->y_pos;
}

double shot_bottom(struct Shot *s) {
    return s->y_pos + s->rect.h;
}

double shot_center_x(struct Shot *s) {
    return s->x_pos + s->rect.w / 2.0;
}

double shot_center_y(struct Shot *s) {
    return s->y_pos + s->rect.h / 2.0;
}

double shot_radius(struct Shot *s){
    return s->radius;
}

void shot_update(struct Shot *s, double dt) {
    if (s->alive) {
        s->shot_count += 60 * dt;
        if (s->shot_count > s->life_time) {
            s->alive = false;
            return;
        }

        s->x_pos += s->x_vel * dt;
        s->y_pos += s->y_vel * dt;

        if (s->x_pos + s->rect.w < 0) {
            s->x_pos = WINDOW_WIDTH;
        } else if (s->x_pos > WINDOW_WIDTH) {
            s->x_pos = -s->rect.w;
        }

        if (s->y_pos + s->rect.h < 0) {
            s->y_pos = WINDOW_HEIGHT;
        } else if (s->y_pos > WINDOW_HEIGHT) {
            s->y_pos = -s->rect.h;
        }

        s->rect.x = (int)(s->x_pos + 0.5);
        s->rect.y = (int)(s->y_pos + 0.5);
    }
}

bool shot_draw(struct Shot *s) {
    if (s->alive) {
        if ( SDL_RenderCopyEx(s->renderer, s->image, NULL, &s->rect, s->angle, NULL, SDL_FLIP_NONE)) {
            fprintf(stderr, "Error while rendering texture: %s\n", SDL_GetError());
            return true;
        }
    }
    return false;
}
