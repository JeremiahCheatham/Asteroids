#include "asteroids.h"

bool asteroids_generate(
    struct Asteroid **asteroids,
    SDL_Renderer *renderer,
    SDL_Texture *large_image,
    SDL_Texture *medium_image,
    SDL_Texture *small_image,
    unsigned int count
) {
    asteroids_free(asteroids);

    for (unsigned int i = 0; i < count; i++) {
        struct Asteroid *asteroid = calloc(1, sizeof(struct Asteroid));
        if (asteroid == NULL) {
            fprintf(stderr, "Error in calloc of new asteroid.\n");
            return true;
        }

        if (asteroid_new(
            asteroid,
            renderer,
            large_image,
            medium_image,
            small_image, 0, 0, 0, 0, 0, 0)
        ) {
            return true;
        }
        asteroid->next = *asteroids;
        if (*asteroids) {
            (*asteroids)->previous = asteroid;
        }
        *asteroids = asteroid;
    }
    return false;
}

void asteroids_free(struct Asteroid **asteroids) {
    struct Asteroid *a = *asteroids;
    while (a) {
        struct Asteroid *next = a->next;
        free(a);
        a = next;
    }
    *asteroids = NULL;
}

void asteroids_update(struct Asteroid *a, double dt) {
    while (a != NULL) {
        asteroid_update(a, dt);
        a = a->next;
    }
}

bool asteroids_draw(struct Asteroid *a) {
    while (a != NULL) {
        if (asteroid_draw(a)) {
            return true;
        }
        a = a->next;
    }

    return false;
}
