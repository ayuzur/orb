#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ncurses.h>

#include "draw.h"
#include "term.h"
#include "utils.h"

int braille_nums[8] = {0x1, 0x8, 0x2, 0x10, 0x4, 0x20, 0x40, 0x80};

const unsigned int BRAILLE_WIDTH_IN_DOTS = 2;
const unsigned int BRAILLE_HEIGHT_IN_DOTS = 4;

const int BRAILLE_EMPTY = 0x2800;

void draw_char(int x, int y, char ch) {
	term_goto(x, y);
	printf("%c", ch);
}

void draw_clear_braille_screen(draw_BrailleScreen* scr) {
	for (int i = 0; i < scr->width * scr->height; i++) {
		scr->array[i] = BRAILLE_EMPTY;
	}
}

draw_BrailleScreen draw_init_braille_screen(int width, int height) {
	width *= BRAILLE_WIDTH_IN_DOTS;
	height *= BRAILLE_HEIGHT_IN_DOTS;
	draw_BrailleScreen scr;
	scr.width = width;
	scr.height = height;
	scr.array = (wchar_t*) ccalloc(width * height, sizeof(wchar_t));

	draw_clear_braille_screen(&scr);

	return scr;
}

void draw_update_braille_screen(draw_BrailleScreen* scr, int width, int height) {
	width *= BRAILLE_WIDTH_IN_DOTS;
	height *= BRAILLE_HEIGHT_IN_DOTS;
	if (scr->width == width && scr->height == height) {
		return;
	}


	int new_size;

	if (width * height > 0) {
		scr->width = width;
		scr->height = height;
		new_size = width * height;
	}
	else {
		scr->width = 1;
		scr->height = 1;
		new_size = 1;
	}

	scr->array = (wchar_t*) realloc(scr->array, new_size * sizeof(wchar_t));
}

void draw_braille_dot(draw_BrailleScreen* scr, int x, int y, enum draw_Mode mode) {
	int char_x = x / BRAILLE_WIDTH_IN_DOTS;
	int char_y = y / BRAILLE_HEIGHT_IN_DOTS;
	
	bool x_out = char_x >= scr->width || char_x < 0;
	bool y_out = char_y >= scr->height || char_y < 0;

	if (x_out || y_out) {
		return;
	}

	int index = char_y * scr->width + char_x;

	int local_x = x % BRAILLE_WIDTH_IN_DOTS;
	int local_y = y % BRAILLE_HEIGHT_IN_DOTS;

	int local_index = local_y * BRAILLE_WIDTH_IN_DOTS + local_x;

	switch (mode) {
		case ADD:
			scr->array[index] |= braille_nums[local_index];
			break;
		case SUB:
			scr->array[index] = BRAILLE_EMPTY;
			break;
		case XOR:
			scr->array[index] ^= braille_nums[local_index];
			break;
	}
}

void draw_braille_screen(draw_BrailleScreen* scr) {
	for (int i = 0; i < scr->width * scr->height; i++) {
		wchar_t wch = scr->array[i];
		if (wch == BRAILLE_EMPTY) {
			continue;
		}
		int x = i % scr->width;
		int y = i / scr->width;
		//printf("x:%d, y:%d, w:%d, i:%d %%:%d", x, y, scr->width, i, i % scr->width);
		term_goto(x, y);

		printf("%lc", wch);
	}
}

void draw_braille_screen_nc(draw_BrailleScreen* scr) {
	for (int i = 0; i < scr->width * scr->height; i++) {
		wchar_t wch = scr->array[i];
		if (wch ==  BRAILLE_EMPTY) {
			continue;
		}
		int x = i % scr->width;
		int y = i / scr->width;

		mvprintw(y, x, "%lc", wch);
	}
}

void draw_braille_line(draw_BrailleScreen* scr, int x1, int y1, int x2, int y2, enum draw_Mode mode) {
	int signed_diff_x = x2 - x1;
	int signed_diff_y = y2 - y1;

	if (signed_diff_x == 0) {
		int sign = get_sign(signed_diff_y);
		int ny = y1;
		for (int i = 0; i <= abs(signed_diff_y); i++) {	
			draw_braille_dot(scr, x1, ny, mode);
			ny += sign;
		}
		return;
	}

	int dir = get_sign(signed_diff_x);

	float slope = (float) signed_diff_y / signed_diff_x;

	int nx = 0;
	int prev_y = 0;
	int prev_x = 0;

	for (int i = 0; i <= abs(signed_diff_x); i++) {
		int ny = nx * slope;
		draw_braille_dot(scr, nx + x1, ny + y1, mode);
		if (abs(ny - prev_y) > 1) {
			int ydir = get_sign(ny - prev_y);
			for (int j = 0; j < abs(ny - prev_y); j++) {
				draw_braille_dot(scr, prev_x + x1, prev_y + y1 + (j * ydir), mode);
			}
		}
		prev_y = ny;
		prev_x = nx;
		nx += dir;
	}
}

void draw_braille_circle(draw_BrailleScreen* scr, int x, int y, int radius, enum draw_Mode mode) {

	// a^2 + b^2 = c^2
	// x^2 + y^2 = r^2
	// y^2 = r^2 - x^2
	// y = sqrt(r^2 - x^2)
	int left_x = x - radius;
	int current_x = left_x;
	int change_x = -radius;
	int change_y = 0;

	int prev_x = change_x;
	int prev_y = change_y;

	for (int i = 0; i <= radius * 2; i++) {
		change_y = ceil(sqrtf(radius * radius - change_x * change_x));
		int new_y = change_y + y;
		draw_braille_dot(scr, current_x, new_y, mode);
		if (change_y != 0) {
			draw_braille_dot(scr, current_x, y + (y - new_y), mode);
		}

		if (abs(change_y - prev_y) > 1) {
			int ydir = get_sign(change_y - prev_y);

			int offset = 0;
			if (change_y < prev_y) {
				offset = 1;
			}

			for (int j = 1; j < abs(change_y - prev_y); j++) {
				int fill_y = prev_y + y + (j * ydir);
				draw_braille_dot(scr, prev_x + x + offset, fill_y, mode);
				fill_y = y + (y - fill_y);
				draw_braille_dot(scr, prev_x + x + offset, fill_y, mode);
			}
		}

		prev_x = change_x;
		prev_y = change_y;

		change_x += 1;
		current_x += 1;

	}
}
