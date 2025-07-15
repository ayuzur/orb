#include <stdio.h>

void term_erase() {
	printf("\033[2J");
}

void term_home() {
	printf("\033[H");
}

void term_clear() {
	term_erase();
	term_home();
}

void term_goto(int x, int y) {
	printf("\033[%d;%dH", x, y);
}
