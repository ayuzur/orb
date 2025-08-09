#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>

#include "term.h"
#include "draw.h"
#include "sleep.h"
#include "utils.h"

typedef struct Flys {
	float x;
	float y;
	float vx;
	float vy;
} Flys;

typedef struct Cursor {
	Flys p;
	int r;
} Cursor;

extern const unsigned int BRAILLE_WIDTH_IN_DOTS;
extern const unsigned int BRAILLE_HEIGHT_IN_DOTS;

void effect_draw_closest(draw_BrailleScreen* scr, int fly_count, Flys* flys, enum draw_Mode mode) {

	for (int i = 0; i < fly_count; i++) {
		int x = flys[i].x;
		int y = flys[i].y;

		int closest_x;
		int closest_y;
		float smallest_dist = -1;
		int index_of_closest;

		for (int j = 0; j < fly_count; j++) {
			if (j == i) {
				continue;
			}
			float dist = get_dist(x, y, flys[j].x, flys[j].y);
			if (dist < smallest_dist || smallest_dist == -1) {
				smallest_dist = dist;
				index_of_closest = j;
			}
		}
		closest_x = flys[index_of_closest].x;
		closest_y = flys[index_of_closest].y;

		draw_braille_line(scr, x, y, closest_x, closest_y, mode);
	}
}

int time_to_seconds(char* time) {
	int len = strlen(time);

	if (len != 8) {
		printf("ERROR: Input should be 8 characters. Ensure input is in the form of hh:mm:ss.\n");
		printf("Ensure you have 0s in unused slots. EX: 01:00:00 for 1 hour\n");
		exit(1);
	}

	bool bad_colon = time[2] != ':' || time[5] != ':';

	if (bad_colon) {
		printf("ensure input is in the form of hh:mm:ss\n");
		exit(1);
	}

	int seconds = 0;

	for (int i = 0; i < len; i += 3) {
		char digits[3];
		digits[0] = time[i];
		digits[1] = time[i + 1];
		digits[2] = '\0';

		int factor;
		if (i == 0) {
			factor = 60 * 60;
		}
		else if (i == 3) {
			factor = 60;
		}
		else if (i == 6) {
			factor = 1;
		}

		int s = atoi(digits) * factor;
		seconds += s;
	}

	return seconds;
}

// MAKE SURE GIVEN CHAR* IS AT LEAST 9 CHARS LONG
void seconds_to_time(char* time, int seconds) {
	int h = (seconds / 60) / 60;
	seconds -= (h * 60 * 60);
	int m = seconds / 60;
	seconds -= (m * 60);
	int s = seconds;
	//int s = seconds % 60 % 60;
	//int m = (seconds / 60) % 60;

	int h1 = h / 10;
	int h2 = h % 10;
	int m1 = m / 10;
	int m2 = m % 10;
	int s1 = s / 10;
	int s2 = s % 10;

	//sprintf(time, "%d:%d:%d", h, m, s);
	sprintf(time, "%d%d:%d%d:%d%d", h1, h2, m1, m2, s1, s2);

}

int main(int argc, char* argv[]) {
	if (argc > 2) {
		printf("ERROR: too many args\n");
		exit(1);
	}
	else if (argc < 2) {
		printf("ERROR: please provide time argument hh:mm:ss (Ex: 01:00:00 for 1 hour)\n");
		exit(1);
	}

	int input_seconds = time_to_seconds(argv[1]);

	time_t cur_time = time(NULL);
	time_t end_time = cur_time + input_seconds;

	const int FLY_COUNT = 400;
	const int FLY_SPEED = 1;
	const int FLY_MAX_CENTER_DISTANCE = 40;
	Flys flys[FLY_COUNT];

	bool effect_closest = false;
	bool effect_chain = false;
	bool effect_center = false;
	bool effect_circle = false;
	bool effect_dot = false;


	effect_closest = true;
	effect_chain = true;


	bool start_blow_up = false;
	bool blowing_up = false;
	bool blow_up_done = false;

	//bool set_effect_dots = false;
	//bool flys_moving = true;

	//effect_circle = true;


	setlocale(LC_ALL, "");

	initscr();
	curs_set(0);
	noecho();
	cbreak();
	nodelay(stdscr, true);

	int width = getmaxx(stdscr);
	int height = getmaxy(stdscr);
	int centerx = width * BRAILLE_WIDTH_IN_DOTS / 2;
	int centery = height * BRAILLE_HEIGHT_IN_DOTS / 2;

	Cursor cursor = {{centerx, centery, 0, 0}, 10};

	for (int i = 0; i < FLY_COUNT; i++) {
		flys[i].x = centerx;
		flys[i].y = centery;
	}

	draw_BrailleScreen scr = draw_init_braille_screen(width, height);

	while (true) {

		width = getmaxx(stdscr);
		height = getmaxy(stdscr);

		centerx = width * BRAILLE_WIDTH_IN_DOTS / 2;
		centery = height * BRAILLE_HEIGHT_IN_DOTS / 2;

		draw_update_braille_screen(&scr, width, height);
		draw_clear_braille_screen(&scr);
		erase();


		Flys last = {centerx, centery, 0, 0};
		for (int i = 0; i < FLY_COUNT; i++) {
			if (blow_up_done) {
				break;
			}
			if (start_blow_up) {
				float factor = 0.8;
				flys[i].vx = (centerx - flys[i].x) * factor;
				flys[i].vy = (centery - flys[i].y) * factor;
				if (i == FLY_COUNT - 1) {
					start_blow_up = false;
					blowing_up = true;
					i = 0;
				}
				else {
					continue;
				}
			}
			if (blowing_up) {
				flys[i].x += flys[i].vx;
				flys[i].y += flys[i].vy;
				flys[i].vx *= 0.9;
				flys[i].vy *= 0.9;

				//bool horizontal_out = flys[i].x < 0 || flys[i].x > width * BRAILLE_WIDTH_IN_DOTS;
				//bool vertical_out = flys[i].y < 0 || flys[i].y > height * BRAILLE_HEIGHT_IN_DOTS;
			}
			float dist = get_dist(flys[i].x, flys[i].y, centerx, centery);
			if (!blowing_up) {
				flys[i].x += (rand() % (FLY_SPEED + 1 + FLY_SPEED)) - FLY_SPEED;
				flys[i].y += (rand() % (FLY_SPEED + 1 + FLY_SPEED)) - FLY_SPEED;
				if (dist > FLY_MAX_CENTER_DISTANCE) {
					flys[i].x = centerx;
					flys[i].y = centery;
				}
			}

			int x = flys[i].x;
			int y = flys[i].y;

			if (effect_chain) {
				draw_braille_line(&scr, last.x, last.y, x, y, ADD);
			}

			if (effect_center) {
				draw_braille_line(&scr, centerx, centery, x, y, ADD);
			}

			if (effect_dot) {
				draw_braille_dot(&scr, flys[i].x, flys[i].y, ADD);
			}

			if (effect_circle) {
				draw_braille_circle(&scr, flys[i].x, flys[i].y, clamp(FLY_MAX_CENTER_DISTANCE/(dist), 5, 0), ADD);
			}

			last.x = flys[i].x;
			last.y = flys[i].y;
		}
		if (blowing_up && !effect_dot) {
			effect_chain = false;
			effect_closest = false;
			effect_dot = true;
		}
		if (effect_closest) {
			effect_draw_closest(&scr, FLY_COUNT, flys, ADD);
		}
		bool good_enough_velocity = false;
		float too_small = 0.001;
		for (int i = 0; i < FLY_COUNT; i++) {
			if (flys[i].vx > too_small || flys[i].vy > too_small) {
				good_enough_velocity = true;
			}
		}
		if (!good_enough_velocity) {
			blow_up_done = true;
		}

		if (blow_up_done) {
			break;
		}
		//draw_braille_dot(&scr, cursor.p.x, cursor.p.y, ADD);
		//draw_braille_line(&scr, centerx, centery, cursor.p.x, cursor.p.y);

		//draw_braille_circle(&scr, cursor.p.x, cursor.p.y, cursor.r, XOR);
		// core
		
		if (!blowing_up) {
			draw_braille_circle(&scr, centerx - 1, centery, 10, SUB);
			draw_braille_circle(&scr, centerx, centery, 10, SUB);
		}
		draw_braille_screen_nc(&scr);


		if (!blowing_up) {
			cur_time = time(NULL);
			int diff = end_time - cur_time;
			if (diff > 0)  {
				char output[9];
				seconds_to_time(output, diff);

				mvprintw(height / 2, (width / 2) - 4, "%s", output);
				//mvprintw(height / 2 + 1, (width / 2) - 4, "%d", input_seconds);
			} else {
				start_blow_up = true;
			}
		}

		frameSleep();
		char ch = getch();
		if (ch == 'q') {
			break;
		}

		switch (ch) {
			case 'h':
				cursor.p.x -= 1;
				break;
			case 'j':
				cursor.p.y += 1;
				break;
			case 'J':
				cursor.r -= 1;
				break;
			case 'k':
				cursor.p.y -= 1;
				break;
			case 'K':
				cursor.r += 1;
				break;
			case 'l':
				cursor.p.x += 1;
				break;

		}
	}

	endwin();
	return 0;
}
