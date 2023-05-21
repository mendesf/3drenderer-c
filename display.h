#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *color_buffer;
extern SDL_Texture *color_buffer_texture;
extern int window_width;
extern int window_height;

bool create_window(void);
void destroy_window(void);
void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void draw_grid(int size, uint32_t color);
void draw_pixel(int pos_x, int pos_y, uint32_t color);
void draw_rect(int pos_x, int pos_y, size_t width, size_t height, uint32_t color);