#include "asteroid.h"

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
) {
    a->renderer = renderer;
    a->images[0] = large_image;
    a->images[1] = medium_image;
    a->images[2] = small_image;
    a->x_pos = x_pos;
    a->y_pos = y_pos;
    a->angle = angle;
    a->x_vel = x_vel;
    a->y_vel = y_vel;
    a->size = size;

    for (unsigned short i = 0; i < sizeof(a->images) / sizeof(a->images[0]); i++) {
        if (SDL_QueryTexture(a->images[i], NULL, NULL, &a->rects[i].w, &a->rects[i].h)) {
            fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
            return true;
        }
        a->radii[i] = a->rects[i].w / 2.0;
    }

    if ((int)x_pos == 0 && (int)y_pos == 0) {
        while (true) {
            double center_x = (WINDOW_WIDTH - (double)a->rects[a->size].w) / 2;
            double center_y = (WINDOW_HEIGHT - (double)a->rects[a->size].h) / 2;
            a->x_pos = rand() % WINDOW_WIDTH;
            a->y_pos = rand() % WINDOW_HEIGHT;
            if (hypot(center_x - a->x_pos, center_y - a->y_pos) > 150) {
                break;
            }
        }
    }

    if ( (int)a->angle == 0 ) {
        a->angle = rand() % 360;
    }

    if ((int)x_vel == 0 && (int)y_vel == 0) {
        asteroid_reset(a);
    } else {
        a->angle_rate = (((rand() % (200 * (a->size + 1))) / 100.0) - 1) * 60;
    }

    return false;
}

void asteroid_reset(struct Asteroid *a) {
    a->angle_rate += (((rand() % (200 * (a->size + 1))) / 100.0) - 1) * 60;
    double radian = a->angle * M_PI / 180;
    double speed = ((rand() % 100) / 100.0) * 60;
    a->x_vel += sin(radian) * speed;
    a->y_vel -= cos(radian) * speed;
}

bool asteroid_split(struct Asteroid *a, double angle) {
    double center_x = asteroid_center_x(a);
    double center_y = asteroid_center_y(a);
    a->size++;
    a->x_pos = center_x - a->radii[a->size];
    a->y_pos = center_y - a->radii[a->size];
    a->rects[a->size].x = (int)(a->x_pos + 0.5);
	a->rects[a->size].y = (int)(a->y_pos + 0.5);

    struct Asteroid *new_a = calloc(1, sizeof(struct Asteroid));
    if (new_a == NULL) {
        printf("Error in calloc of new asteroid.\n");
        return true;
    }

    if (asteroid_new(new_a,
        a->renderer,
        a->images[0],
        a->images[1],
        a->images[2],
        a->x_pos,
        a->y_pos,
        a->angle,
        a->x_vel,
        a->y_vel,
        a->size)) {
        return true;
    }

    angle += 90;
    if (angle > 360) {
        angle -= 360;
    }
    new_a->angle = angle;
    asteroid_reset(new_a);

    angle += 180;
    if (angle > 360) {
        angle -= 360;
    }
    a->angle = angle;
    asteroid_reset(a);

    if (a->next) {
        a->next->previous = new_a;
    }
    new_a->next = a->next;
    new_a->previous = a;
    a->next = new_a;

    return false;
}

double asteroid_left(struct Asteroid *a) {
    return a->x_pos;
}

double asteroid_right(struct Asteroid *a) {
    return a->x_pos + a->rects[a->size].w;
}

double asteroid_top(struct Asteroid *a) {
    return a->y_pos;
}

double asteroid_bottom(struct Asteroid *a) {
    return a->y_pos + a->rects[a->size].h;
}

double asteroid_center_x(struct Asteroid *a) {
    return a->x_pos + a->rects[a->size].w / 2.0;
}

double asteroid_center_y(struct Asteroid *a) {
    return a->y_pos + a->rects[a->size].h / 2.0;
}

double asteriod_radius(struct Asteroid *a) {
    return a->radii[a->size];
}

void asteroid_update(struct Asteroid *a, double dt) {
    a->angle += a->angle_rate * dt;
    if (a->angle > 360) {
        a->angle -= 360;
    } else if (a->angle < 0) {
        a->angle += 360;
    }

    a->x_pos += a->x_vel * dt;
    a->y_pos += a->y_vel * dt;

    if (a->x_pos + a->rects[a->size].w < 0) {
        a->x_pos = WINDOW_WIDTH;
    } else if (a->x_pos > WINDOW_WIDTH) {
        a->x_pos = -a->rects[a->size].w;
    }

    if (a->y_pos + a->rects[a->size].h < 0) {
        a->y_pos = WINDOW_HEIGHT;
    } else if (a->y_pos > WINDOW_HEIGHT) {
        a->y_pos = -a->rects[a->size].h;
    }

    a->rects[a->size].x = (int)(a->x_pos + 0.5);
	a->rects[a->size].y = (int)(a->y_pos + 0.5);
}

bool asteroid_draw(struct Asteroid *a) {
    if ( SDL_RenderCopyEx(a->renderer, a->images[a->size], NULL, &a->rects[a->size], a->angle, NULL, SDL_FLIP_NONE)) {
        fprintf(stderr, "Error while rendering texture: %s\n", SDL_GetError());
        return true;
    }

    return false;
}