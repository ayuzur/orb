#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>

#include "term.h"
#include "draw.h"
#include "sleep.h"
#include "utils.h"

typedef struct Point {
	int x;
	int y;
} Point;

extern const unsigned int BRAILLE_WIDTH_IN_DOTS;
extern const unsigned int BRAILLE_HEIGHT_IN_DOTS;

int main() {

	const int FLY_COUNT = 500;//400;
	const int FLY_SPEED = 1;
	const int FLY_MAX_CENTER_DISTANCE = 40;
	Point flys[FLY_COUNT];


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

	Point cursor = {centerx, centery};

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


		Point last = {centerx, centery};
		for (int i = 0; i < FLY_COUNT; i++) {
			flys[i].x += (rand() % (FLY_SPEED + 1 + FLY_SPEED)) - FLY_SPEED;
			flys[i].y += (rand() % (FLY_SPEED + 1 + FLY_SPEED)) - FLY_SPEED;

			float dist = get_dist(flys[i].x, flys[i].y, centerx, centery);

			if (dist > FLY_MAX_CENTER_DISTANCE) {
				flys[i].x = centerx;
				flys[i].y = centery;
			}
			int x = flys[i].x;
			int y = flys[i].y;
			
			draw_braille_line(&scr, last.x, last.y, x, y);
			//draw_braille_line(&scr, centerx, centery, x, y);

			draw_braille_dot(&scr, flys[i].x, flys[i].y);

			last.x = flys[i].x;
			last.y = flys[i].y;
		}
		
		for (int i = 0; i < FLY_COUNT; i++) {
			int x = flys[i].x;
			int y = flys[i].y;

			int closest_x;
			int closest_y;
			float smallest_dist = -1;
			int index_of_closest;

			for (int j = 0; j < FLY_COUNT; j++) {
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

			draw_braille_line(&scr, x, y, closest_x, closest_y);
		}

		
		draw_braille_dot(&scr, cursor.x, cursor.y);
		draw_braille_line(&scr, centerx, centery, cursor.x, cursor.y);

		draw_braille_screen_nc(&scr);
		frameSleep();
		char ch = getch();
		if (ch == 'q') {
			break;
		}

		switch (ch) {
			case 'h':
				cursor.x -= 1;
				break;
			case 'j':
				cursor.y += 1;
				break;
			case 'k':
				cursor.y -= 1;
				break;
			case 'l':
				cursor.x += 1;
				break;
		}
	}

endwin();
return 0;
}
