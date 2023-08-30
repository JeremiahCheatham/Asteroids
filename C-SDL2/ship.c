#include "ship.h"

bool ship_new(struct Ship **ship, SDL_Renderer *renderer, SDL_Texture *image1, SDL_Texture *image2) {
    *ship = calloc(1, sizeof(struct Ship));
    if (!*ship) {
        fprintf(stderr, "Error in calloc of new ship.\n");
        return true;
    }
    struct Ship *s = *ship;

    s->renderer = renderer;
    s->image1 = image1;
    s->image2 = image2;
    s->angle_rate = 300;
    s->thrust = 360;
    s->size = 60;
    s->offset = 32;
    s->radius = 28;

    if (SDL_QueryTexture(s->image1, NULL, NULL, &s->rect.w, &s->rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return true;
    }

    s->keystate = SDL_GetKeyboardState(NULL);
    
    return false;
}

void ship_free(struct Ship **ship) {
    if (*ship) {
        (*ship)->renderer = NULL;
        (*ship)->image1 = NULL;
        (*ship)->image2 = NULL;
        (*ship)->keystate = NULL;
        free(*ship);
        *ship = NULL;
    }
}

void ship_reset(struct Ship *s) {
    s->x_vel = 0;
    s->y_vel = 0;
    s->x_pos = (double)(WINDOW_WIDTH - s->rect.w) / 2;
    s->y_pos = (double)(WINDOW_HEIGHT - s->rect.h) / 2;
    s->rect.x = (int)(s->x_pos + 0.5);
    s->rect.y = (int)(s->y_pos + 0.5);
    s->angle = 0;
    ship_set_immunity(s);
}

Uint32 ship_immunity_event(Uint32 interval, void *param) {
    (void)interval;
    (void)param;
    SDL_Event event;
    event.type = IMMUNITY_EVENT;
    event.user.code = 0;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
    return 0;
}

void ship_set_immunity(struct Ship *s) {
    if (s->immunity) {
        s->immunity = false;
    } else {
        s->immunity = true;
        SDL_AddTimer(3000, ship_immunity_event, NULL);
    }
    s->immunity_counter = 0;
}

double ship_left(struct Ship *s) {
    return s->x_pos + s->offset;
}

double ship_right(struct Ship *s) {
    return s->x_pos + s->offset + s->size;
}

double ship_top(struct Ship *s) {
    return s->y_pos + s->offset;
}

double ship_bottom(struct Ship *s) {
    return s->y_pos + s->offset + s->size;
}

double ship_center_x(struct Ship *s) {
    return s->x_pos + s->rect.w / 2.0;
}

double ship_center_y(struct Ship *s) {
    return s->y_pos + s->rect.h / 2.0;
}

double ship_radius(struct Ship *s) {
    return s->radius;
}

void ship_update(struct Ship *s, double dt) {
    if (s->keystate[SDL_SCANCODE_LEFT]) {
		s->angle -= s->angle_rate * dt;
		if (s->angle < 0) {
			s->angle += 360;
		}
	}

	if (s->keystate[SDL_SCANCODE_RIGHT]) {
		s->angle += s->angle_rate * dt;
		if (s->angle > 360) {
			s->angle -= 360;
		}
	}

	if (s->keystate[SDL_SCANCODE_UP]) {
        double radian = s->angle * M_PI / 180;
        s->x_vel += sin(radian) * s->thrust * dt;
        s->y_vel -= cos(radian) * s->thrust * dt;
        s->engine = true;
    } else {
        s->engine = false;
    }

    s->x_pos += s->x_vel * dt;
    s->y_pos += s->y_vel * dt;

    if (ship_right(s) < 0) {
        s->x_pos = WINDOW_WIDTH - s->offset;
    } else if (ship_left(s) > WINDOW_WIDTH) {
        s->x_pos = -s->offset - s->size;
    }

    if (ship_bottom(s) < 0) {
        s->y_pos = WINDOW_HEIGHT - s->offset;
    } else if (ship_top(s) > WINDOW_HEIGHT) {
        s->y_pos = -s->offset - s->size;
    }

    s->rect.x = (int)(s->x_pos + 0.5);
    s->rect.y = (int)(s->y_pos + 0.5);

    if (s->immunity) {
        s->immunity_counter += 60 * dt;
        if (s->immunity_counter > 9) {
            s->immunity_counter = 0;
        }
    }
}

bool ship_draw(struct Ship *s) {
    if (s->immunity_counter < 5) {
        SDL_Texture *image = NULL;
        if (s->engine) {
            image = s->image2;
        } else {
            image = s->image1;
        }
        if ( SDL_RenderCopyEx(s->renderer, image, NULL, &s->rect, s->angle, NULL, SDL_FLIP_NONE)) {
            fprintf(stderr, "Error while rendering texture: %s\n", SDL_GetError());
            return true;
        }
    }
    return false;
}