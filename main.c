#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {.x = 0, .y= 0, .z=-5};

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

	load_cube_mesh_data();
//	load_obj_file_data("./assets/cube.obj");
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
	int32_t time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
//		SDL_Delay(time_to_wait);
	}

	triangles_to_render = NULL;

    printf("%d ms\n", SDL_GetTicks() - previous_frame_time);

	previous_frame_time = SDL_GetTicks();

	mesh.rotation.x += 0.01f;
	mesh.rotation.y += 0.01f;
	mesh.rotation.z += 0.02f;

	// Loop all triangle faces of our mesh
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++) {
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		triangle_t projected_triangle;

		// Loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++) {
			vec3_t transformed_vertex = face_vertices[j];
			transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

			// Translate the vertex away from the camera
			transformed_vertex.z -= camera_position.z;

			// Project the current point
			vec2_t projected_point = project(transformed_vertex);

			// Scale and translate the projected points to the middle of the screen

			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);

			projected_triangle.points[j] = projected_point;
		}

		// Save the project 2D vector in the array of triangles to render
//		triangles_to_render[i] = projected_triangle;
		array_push(triangles_to_render, projected_triangle);
	}
}

void render(void) {
	uint32_t background_color = 0xFF000000;
	clear_color_buffer(background_color);

//	draw_triangle(100, 200, 200, 150, 150, 100, 0xFFFF00DD);

    draw_grid(10, 0xFF333333);

	// Loop all projected triangles and render them
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++) {
		triangle_t triangle = triangles_to_render[i];

		for (int j = 0; j < 3; j++) {
			vec2_t point_a = triangle.points[j];
			vec2_t point_b = triangle.points[(j + 1) % 3];
			draw_line(
				point_a.x,
				point_a.y,
				point_b.x,
				point_b.y,
				0xFFFF00DD
			);
//			draw_rect(
//				point_a.x,
//				point_a.y,
//				3,
//				3,
//				0xFF00FFDD
//			);
		}
	}

	array_free(triangles_to_render);

	render_color_buffer();
	SDL_RenderPresent(renderer);
}

void free_resources(void) {
	free(color_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);
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
	free_resources();
	return 0;
}
