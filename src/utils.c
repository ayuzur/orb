#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void* ccalloc(int c, size_t bytes) {
	if (c == 0 || bytes == 0) {
		return NULL;
	}

	void* ptr = calloc(c, bytes);

	if (ptr == NULL) {
		fprintf(stderr, "ERROR: calloc failed");
		exit(1);
	}

	return ptr;
}

int get_sign(int i) {
	if (i > 0) {
		return 1;
	}
	else if (i < 0) {
		return -1;
	}
	else {
		return 0;
	}
}

float get_dist(int x1, int y1, int x2, int y2) {
	int diffx = x1 - x2;
	int diffy = y1 - y2;

	float dist = sqrtf(diffx * diffx + diffy * diffy);
	
	return dist;
}

int clamp(int i, int max, int min) {
	if (i > max) {
		i = max;
	}
	else if (i < min) {
		i = min;
	}

	return i;
}
