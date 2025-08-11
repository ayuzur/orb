#ifndef DRAW_H
#define DRAW_H

#include <stdlib.h>



enum draw_Mode {ADD, SUB, XOR};

void draw_char(int x, int y, char ch);

typedef struct draw_BrailleScreen {
	int width;
	int height;
	wchar_t* array;
} draw_BrailleScreen;

draw_BrailleScreen draw_init_braille_screen(int width, int height);

void draw_clear_braille_screen(draw_BrailleScreen* scr);

void draw_update_braille_screen(draw_BrailleScreen* scr, int width, int height);

void draw_braille_dot(draw_BrailleScreen* scr, int x, int y, enum draw_Mode mode);
void draw_braille_screen(draw_BrailleScreen* scr);
void draw_braille_screen_nc(draw_BrailleScreen* scr);

void draw_braille_line(draw_BrailleScreen* scr, int x1, int y1, int x2, int y2, enum draw_Mode mode);

void draw_braille_circle(draw_BrailleScreen* scr, int x, int y, int radius, enum draw_Mode mode);

#endif
