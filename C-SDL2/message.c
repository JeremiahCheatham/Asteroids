#include "message.h"

bool message_new(struct Message **message, SDL_Renderer *renderer) {
    *message = calloc(1, sizeof(struct Message));
    if (!*message) {
        fprintf(stderr, "Error in calloc of message!\n");
        return true;
    }
    struct Message *m = *message;

    m->renderer = renderer;
    m->color = (SDL_Color){255, 255, 255, 255};

    m->font = TTF_OpenFont("fonts/freesansbold.ttf", MESSAGE_FONT_SIZE);
    if (!m->font) {
        fprintf(stderr, "Error creating font: %s\n", TTF_GetError());
        return true;
    }

    return false;
}

void message_free(struct Message **message) {
    if (*message) {
        TTF_CloseFont((*message)->font);
        (*message)->font = NULL;
        SDL_DestroyTexture((*message)->image);
        (*message)->image = NULL;
        (*message)->renderer = NULL;
        free(*message);
        *message = NULL;
    }
}

bool message_update_level(struct Message *m, Uint32 level) {
    int length = snprintf(NULL, 0, "Level: %d", level) + 1;
    char level_str[length];
    snprintf(level_str, (size_t)length, "Level: %d", level);
    if (message_update(m, level_str)) {
        return true;
    }
    return false;
}

bool message_update(struct Message *m, const char* text) {
    if (m->image) {
        SDL_DestroyTexture(m->image);
    }

    SDL_Surface *surface = TTF_RenderText_Blended(m->font, text, m->color);
    if (!surface) {
        fprintf(stderr, "Error creating a surface: %s\n", SDL_GetError());
        return true;
    }

    m->image = SDL_CreateTextureFromSurface(m->renderer, surface);
    if (!m->image) {
        fprintf(stderr, "Error creating a texture: %s\n", SDL_GetError());
        return true;
    }

    SDL_FreeSurface(surface);

    if (SDL_QueryTexture(m->image, NULL, NULL, &m->rect.w, &m->rect.h)) {
        fprintf(stderr, "Error while querying texture: %s\n", SDL_GetError());
        return true;
    }

    m->rect.x = (WINDOW_WIDTH - m->rect.w) / 2;
    m->rect.y = (WINDOW_HEIGHT - m->rect.h) /2 ;
    
    return false;
}

bool message_draw(struct Message *m) {
    if (SDL_RenderCopy(m->renderer, m->image, NULL, &m->rect)) {
        fprintf(stderr, "Error while rendering texture: %s\n", SDL_GetError());
        return true;
    }

    return false;
}
