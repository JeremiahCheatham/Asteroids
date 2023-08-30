#ifndef MESSAGE_H
#define MESSAGE_H

#include "main.h"

struct Message {
    SDL_Renderer *renderer;
    SDL_Texture* image;
    TTF_Font *font;
    SDL_Color color;
    SDL_Rect rect;
};

bool message_new(struct Message **message, SDL_Renderer *renderer);
void message_free(struct Message **message);
bool message_update_level(struct Message *m, Uint32 level);
bool message_update(struct Message *m, const char* text);
bool message_draw(struct Message *m);

#endif