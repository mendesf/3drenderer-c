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

	window_width = display_mode.w;
	window_height = display_mode.h;

	// Create a SDL window
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
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
	free(color_buffer);
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

void draw_grid(int size, uint32_t color) {
	for (int y = 0; y < window_height; y += size) {
		for (int x = 0; x < window_width; x += size) {
			if ((y % size == 0 || x % size == 0)) {
				draw_pixel(x, y, color);
			}
		}
	}
}

void draw_rect(int pos_x, int pos_y, size_t width, size_t height, uint32_t color) {
	for (int y = pos_y; y < pos_y + height; y++) {
		for (int x = pos_x; x < pos_x + width; x++) {
			draw_pixel(x, y, color);
		}
	}
}

