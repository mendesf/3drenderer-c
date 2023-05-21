#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include "display.h"
#include "vector.h"

#define N_POINTS (9 * 9 * 9)
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t camera_position = {.x = 0, .y= 0, .z=-5};
vec3_t cube_rotation = {.x =0, .y = 0, .z=0};

float fov_factor = 640;
uint32_t previous_frame_time = 0;

bool is_running = false;

void setup(void) {
	color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	int point_count = 0;

	// Start loading my array of vectors
	// From -1 to 1 (in this 9x9x9 cube)
	for (float x = -1.0f; x <= 1.0f; x += 0.25f) { // NOLINT(cert-flp30-c)
		for (float y = -1.0f; y <= 1.0f; y += 0.25f) { // NOLINT(cert-flp30-c)
			for (float z = -1.0f; z <= 1.0f; z += 0.25f) { // NOLINT(cert-flp30-c)
				vec3_t new_point = {.x = x, .y= y, .z= z};
				cube_points[point_count++] = new_point;
			}
		}
	}
}

void process_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_QUIT: is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			}
			break;
	}
}

vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};

	return projected_point;
}

void update(void) {
//	while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));
	int32_t time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

//	printf("Frame Time: %d\n", SDL_GetTicks() - previous_frame_time);
	previous_frame_time = SDL_GetTicks();

	cube_rotation.x += 0.01f;
	cube_rotation.y += 0.01f;
	cube_rotation.z += 0.01f;

	for (int i = 0; i < N_POINTS; i++) {
		vec3_t point = cube_points[i];
		// Move the points away from the camera

		vec3_t transformed_point = vec3_rotate_x(point, cube_rotation.y);
		transformed_point = vec3_rotate_y(transformed_point, cube_rotation.y);
		transformed_point = vec3_rotate_z(transformed_point, cube_rotation.y);

		// Translate the point away from the camera
		transformed_point.z -= camera_position.z;

		// Project the current point
		vec2_t projected_point = project(transformed_point);

		// Save the project 2D vector in the array of projected points
		projected_points[i] = projected_point;
	}
}

void render(void) {
	uint32_t background_color = 0xFF000000;
	clear_color_buffer(background_color);

	uint32_t grid_color = 0xFFFFFFCC;
	draw_grid(10, grid_color);

	for (int i = 0; i < N_POINTS; i++) {
		vec2_t projected_point = projected_points[i];
		draw_rect(
			projected_point.x + window_width / 2,
			projected_point.y + window_height / 2,
			4,
			4,
			0xFFFF00DD
		);
	}

	render_color_buffer();
	SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
	is_running = create_window();

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}
