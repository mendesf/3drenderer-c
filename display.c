#include <stdio.h>
#include "display.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool create_window(void) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// Use SDL to query what is the fullscreen max width and height
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);

//	window_width = display_mode.w;
//	window_height = display_mode.h;



	// Create a SDL window
	window = SDL_CreateWindow(
		NULL,
        display_mode.w / 2 + 20,
        20,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
	);

	if (!window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	// Create a SDL renderer
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

//	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true;
}

void destroy_window(void) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void clear_color_buffer(uint32_t color) {
	int length = window_width * window_height;
	for (int i = 0; i < length; ++i) {
		color_buffer[i] = color;
	}
}

void render_color_buffer(void) {
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void draw_pixel(int pos_x, int pos_y, uint32_t color) {
	if (pos_x >= 0 && pos_x < window_width && pos_y >= 0 && pos_y < window_height) {
		color_buffer[window_width * pos_y + pos_x] = color;
	}
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
	// DDA Line Drawing Algorithm
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int longest_side_length = abs(delta_x) >= abs(delta_y)
							  ? abs(delta_x)
							  : abs(delta_y);

	// Find how much we should increment in both x and y each step

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i <= longest_side_length; ++i) {
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	draw_line(x0, y0, x1, y1, 0xFFFF00DD);
	draw_line(x1, y1, x2, y2, 0xFFFF00DD);
	draw_line(x2, y2, x0, y0, 0xFFFF00DD);
}

void draw_rect(int pos_x, int pos_y, size_t width, size_t height, uint32_t color) {
	for (int y = pos_y; y < pos_y + height; y++) {
		for (int x = pos_x; x < pos_x + width; x++) {
			draw_pixel(x, y, color);
		}
	}
}

void draw_grid(int size, uint32_t color) {
	for (int y = 0; y < window_height; y += size) {
		for (int x = 0; x < window_width; x += size) {
			if ((y % size == 0 || x % size == 0)) {
				draw_pixel(x, y, color);
			}
		}
	}
}