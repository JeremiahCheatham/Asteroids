#include "fps.h"

// Print FPS to standard output.
void fps_print() {
    static Uint32 next_time = 1000;
    static int frame_count = 1;
    Uint32 current_time = SDL_GetTicks();
    if (current_time >= next_time) {
        if ((current_time - 1000) > next_time) {
            if (current_time > (UINT32_MAX - 1000))
                next_time = 1000;
            else
                next_time = current_time + 1000;
        } else {
            printf("%d\n", frame_count);
            if (next_time > (UINT32_MAX - 1000))
                next_time = 1000;
            else
                next_time += 1000;
        }
        frame_count = 1;
    } else {
        if (next_time > (UINT32_MAX - 1000))
            next_time = 1000;
        else
            frame_count++;
    }
}

// Calculate delay needed for the FPS.
void fps_delay(struct Game *ast) {
    static float carry_delay = 0;
    static Uint32 last_time = 0;

    Uint32 current_time = SDL_GetTicks();
    // Checking for roll over
    if (current_time >= last_time) {
        Uint32 elapsed_time = current_time - last_time;
        if ((ast->frame_delay + carry_delay) > elapsed_time) {
            unsigned int current_delay = ast->frame_delay - elapsed_time + carry_delay;
            SDL_Delay(current_delay);
            current_time = SDL_GetTicks();
            // Checking for roll over again.
            if (current_time >= last_time) {
                elapsed_time = current_time - last_time;
                carry_delay = ast->frame_delay - elapsed_time + carry_delay;
            } else {
                carry_delay = 0;
            }
        } else {
            carry_delay = ast->frame_delay - elapsed_time + carry_delay;
        }
    } else {
        carry_delay = 0;
    }
    if (carry_delay > ast->frame_delay)
        carry_delay = ast->frame_delay;
    if (carry_delay < -(ast->frame_delay))
        carry_delay = -(ast->frame_delay);
    last_time = current_time;
}
