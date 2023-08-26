#include "game.h"
#include "initialize.h"
#include "load_media.h"
#include "asteroids.h"

bool game_new(struct Game **game) {
    *game = calloc(1, sizeof(struct Game));
    if (*game == NULL) {
        fprintf(stderr, "Error in calloc of new game.\n");
        return true;
    }
    struct Game *g = *game;

    if (game_initilize(g)) {
        return true;
    }

    if (game_load_media(g)) {
        return true;
    }

    if (ship_new(&g->ship, g->renderer, g->ship_image1, g->ship_image2)) {
        return true;
    }
    
    for (unsigned short i = 0; i < LEN(g->shots); i++) {
        if (shot_initialize(&g->shots[i], g->renderer, g->shot_image)) {
            return true;
        }
    }

    if (score_new(&g->score, g->renderer)) {
        return true;
    }


    if (message_new(&g->message, g->renderer)) {
        return true;
    }

    if (fps_new(&g->fps)) {
        return true;
    }

    g->playing = true;
    g->icon_rect.y = 10;
    g->lives = 3;

    return false;
}

void game_free(struct Game *g) {
    for (unsigned short i = 0; i < LEN(g->shots); i++) {
        shot_free(g->shots[i]);
    }

    fps_free(g->fps);
    ship_free(g->ship);
    message_free(g->message);
    score_free(g->score);
    asteroids_free(&g->asteroids);

    Mix_FreeChunk(g->thruster_sound);
    Mix_FreeChunk(g->laser_sound);
    Mix_FreeChunk(g->collide_sound);
    Mix_FreeChunk(g->boom_sound);
    Mix_FreeMusic(g->falling_music);
    SDL_DestroyTexture(g->shot_image);
    SDL_DestroyTexture(g->icon_image);
    SDL_DestroyTexture(g->asteroid_small_image);
    SDL_DestroyTexture(g->asteroid_medium_image);
    SDL_DestroyTexture(g->asteroid_large_image);
    SDL_DestroyTexture(g->ship_image1);
    SDL_DestroyTexture(g->ship_image2);
    SDL_DestroyTexture(g->background_image);
    SDL_DestroyRenderer(g->renderer);
    SDL_DestroyWindow(g->window);
    TTF_Quit();
    Mix_Quit();
    SDL_Quit();
    free(g);
}

bool game_reset(struct Game *g) {

    ship_reset(g->ship);

    for (unsigned short i = 0; i < LEN(g->shots); i++) {
        g->shots[i]->alive = false;
    }

    if (asteroids_generate(
            &g->asteroids,
            g->renderer,
            g->asteroid_large_image,
            g->asteroid_medium_image,
            g->asteroid_small_image,
            g->level)) {
        return true;
    }

    g->shot_count = 101;

    return false;
}

bool game_shoot(struct Game *g) {
    for (unsigned short i = 0; i < LEN(g->shots); i++) {
        if (!g->shots[i]->alive) {
            Mix_PlayChannel(-1, g->laser_sound, 0);
            double x = g->ship->x_pos + g->ship->rect.w / 2.0;
            double y = g->ship->y_pos + g->ship->rect.h / 2.0;
            shot_new(g->shots[i], x, y, g->ship->angle, g->ship->radius);
            if (g->shot_count >  0) {
                g->shot_count--;
            }
            return false;
        }
    }
    return false;
}

Uint32 game_mode_event(uint interval, void *param) {
    (void)interval;
    (void)param;
    SDL_Event event;
    event.type = GAME_MODE_EVENT;
    event.user.code = 0;
    event.user.data1 = NULL;
    event.user.data2 = NULL;
    SDL_PushEvent(&event);
    return 0;
}

bool set_game_mode(struct Game *g) {
	g->game_mode++;
	if (g->game_mode > 4) {
		g->game_mode = 1;
	}
	switch (g->game_mode) {
        case 1:
            g->lives = 0;
            g->level = 0;
            if (message_update(g->message, "Asteroids!")) {
                return true;
            }
            if (score_reset(g->score)) {
                return true;
            }
            g->show_game = false;
            g->show_message = true;
            break;
        case 2:
            g->lives++;
            g->level++;
            if (message_update_level(g->message, g->level)) {
                return true;
            }
            g->show_game = false;
            g->show_message = true;
            break;
        case 3:
            if (game_reset(g)) {
                return true;
            }
            g->show_game = true;
            g->show_message = false;
            break;
        case 4:
            if (message_update(g->message, "Game Over")) {
                return true;
            }
            g->show_game = true;
            g->show_message = true;
            break;
    }

    if (g->game_mode != 3) {
        SDL_AddTimer(5000, game_mode_event, NULL);
    }

    return false;
}

bool check_ship_collisions(struct Game *g) {
    if (g->ship->immunity) {
        return false;
    }
    struct Asteroid *a = g->asteroids;
    while (a != NULL) {
        if (ship_right(g->ship) > asteroid_left(a) && ship_left(g->ship) < asteroid_right(a)) {
            if (ship_bottom(g->ship) > asteroid_top(a) && ship_top(g->ship) < asteroid_bottom(a)) {
                double distance = hypot(ship_center_x(g->ship) - asteroid_center_x(a), ship_center_y(g->ship) - asteroid_center_y(a));
                if ( distance < asteriod_radius(a) + ship_radius(g->ship)) {
                    Mix_PlayChannel(-1, g->collide_sound, 0);
                    if (g->lives > 0) {
                        g->lives--;
                        ship_set_immunity(g->ship);
                    } else {
                        if (set_game_mode(g)) { 
                            return true;
                        }
                    }
                    return false;
                }
            }
        }
        a = a->next;
    }
    return false;
}

bool lives_draw(struct Game *g) {
    if (g->lives > 0) {
        for (uint i = 0; i < g->lives; i++) {
            g->icon_rect.x = (int)(i * 30 + 10);
            if ( SDL_RenderCopy(g->renderer, g->icon_image, NULL, &g->icon_rect)) {
                fprintf(stderr, "Error while rendering texture: %s\n", SDL_GetError());
                return true;
            }
        }
    }
    return false;
}

bool check_shot_collision(struct Game *g) {
    struct Asteroid *a = g->asteroids;
    bool hit = false;
    while (a != NULL) {
        hit = false;
        for (uint i = 0; i < LEN(g->shots); i++) {
            struct Shot *s = g->shots[i];
            if (!s->alive) {
                continue;
            }
            if (shot_right(s) > asteroid_left(a) && shot_left(s) < asteroid_right(a)) {
                if (shot_bottom(s) > asteroid_top(a) && shot_top(s) < asteroid_bottom(a)) {
                    double distance = hypot(shot_center_x(s) - asteroid_center_x(a), shot_center_y(s) - asteroid_center_y(a));
                    if ( distance < asteriod_radius(a) + shot_radius(s)) {
                        Mix_PlayChannel(-1, g->boom_sound, 0);
                        s->alive = false;
                        if (score_increment(g->score, g->level * g->shot_count)) {
                            return true;
                        }
                        if (a->size == 2) {
                            if (a->previous) {
                                a->previous->next = a->next;
                            } else {
                                g->asteroids = a->next;
                            }
                            if (a->next) {
                                a->next->previous = a->previous;
                            }
                            struct Asteroid *old_a = a;
                            a = a->next;
                            free(old_a);
                            hit = true;
                            if (g->asteroids == NULL) {
                                g->game_mode = 1;
                                if (set_game_mode(g)) {
                                    return true;
                                }
                            }
                        } else {
                            if (asteroid_split(a, s->angle)) {
                                return true;
                                
                            }
                        }
                        break;
                    }
                }
            }
        }
        if (hit) {
            continue;
        }
        a = a->next;
    }
    return false;
}

bool game_run(struct Game *g) {
    if (set_game_mode(g)) {
        return true;
    }

    if (Mix_PlayMusic(g->falling_music, -1)) {
        fprintf(stderr, "Error while playing music: %s\n", Mix_GetError());
        return true;
    }

    while (g->playing) {
        while (SDL_PollEvent(&g->event)) {
            switch (g->event.type) {
                case SDL_QUIT:
                    return false;
                    break;
                case IMMUNITY_EVENT:
                    ship_set_immunity(g->ship);
                    break;
                case GAME_MODE_EVENT:
                    if (set_game_mode(g)) {
                        return true;
                    }
                    break;
                case SDL_KEYDOWN:
                    switch (g->event.key.keysym.scancode) {
                        case SDL_SCANCODE_ESCAPE:
                            return false;
                            break;
                        case SDL_SCANCODE_SPACE:
                            if (game_shoot(g)) {
                                return true;
                            }
                            break;
                        case SDL_SCANCODE_F:
                            fps_toggle_display(g->fps);
                            break;
                        default:
                            break;
                    }
            }
        }

        if (!g->show_message) {
            ship_update(g->ship, g->delta_time);

            for (unsigned short i = 0; i < sizeof(g->shots) / sizeof(g->shots[0]); i++) {
                shot_update(g->shots[i], g->delta_time);
            }
            
            asteroids_update(g->asteroids, g->delta_time);

            if (check_ship_collisions(g)) {
                return true;
            }

            if (check_shot_collision(g)) {
                return true;
            }
        }

        if (SDL_RenderClear(g->renderer)) {
            fprintf(stderr, "Error while clearing renderer: %s\n", SDL_GetError());
            return true;
        }

        if ( SDL_RenderCopy(g->renderer, g->background_image, NULL, &g->background_rect)) {
            fprintf(stderr, "Error while rendering texture: %s\n", SDL_GetError());
            return true;
        }

        if (g->show_game) {
            if (asteroids_draw(g->asteroids)) {
                return true;
            }

            for (unsigned short i = 0; i < sizeof(g->shots) / sizeof(g->shots[0]); i++) {
                if (shot_draw(g->shots[i])) {
                    return true;
                }
            }

            if (lives_draw(g)) {
                return true;
            }

            if (ship_draw(g->ship)) {
                return true;
            }

            if (score_draw(g->score)) {
                return true;
            }
        }

        if (g->show_message) {
            if (message_draw(g->message)) {
                return true;
            }
        }

        SDL_RenderPresent(g->renderer);

        g->delta_time = fps_update(g->fps);
    }
    
    return false;
}
