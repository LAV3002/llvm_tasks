#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include <windows.h>

#define GEN_SIZE 70
#define BLOCK_SIZE 10
#define WINDOW_WIDTH GEN_SIZE * BLOCK_SIZE
#define REPEAT_SIZE 500

void simFlush();
void simPutPixel(int x, int y, int argb);
int simRand();