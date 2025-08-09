#ifndef UTILS_H
#define UTILS_H

// checked calloc
void* ccalloc(int c, size_t bytes);

int get_sign(int i);

float get_dist(int x1, int y1, int x2, int y2);

int clamp(int i, int max, int min);



#endif
