#include "sim.h"

void draw_shape(int x, int y, Uint32 argb) {
    unsigned i = 0;
    unsigned j = 0;
    for (i = x * BLOCK_SIZE; i < (x + 1) * BLOCK_SIZE; ++i) {
        for (j = y * BLOCK_SIZE; j < (y + 1) * BLOCK_SIZE; ++j)
        {
            simPutPixel(i, j, argb);
        }
    }
}

void drawGen(unsigned* gen)
{
    unsigned x = 0;
    unsigned y = 0;
    for (y = 0; y < GEN_SIZE; ++y) {
        for (x = 0; x < GEN_SIZE; ++x)
        {
            draw_shape(x, y, 0xFF000000 + 0xFF00 * gen[y * GEN_SIZE + x]);
        }
    }
}

void calcGen(unsigned* nextGen, unsigned* prevGen)
{
    int x = 0;
    int y = 0;
    int dx = 0;
    int dy = 0;
    int cnt = 0;
    for (y = 0; y < GEN_SIZE; ++y) {
        for (x = 0; x < GEN_SIZE; ++x)
        {
            cnt = 0;
            for (dx = -1; dx < 2; ++dx) {
                for (dy = -1; dy < 2; ++dy) {
                    cnt += prevGen[((y + dy + GEN_SIZE) % GEN_SIZE) * GEN_SIZE + ((x + dx + GEN_SIZE) % GEN_SIZE)];
                }
            }
            cnt -= prevGen[y * GEN_SIZE + x];
            if (prevGen[y * GEN_SIZE + x]) {
                if (cnt == 2 || cnt == 3) {
                    nextGen[y * GEN_SIZE + x] = 1;
                }
                else {
                    nextGen[y * GEN_SIZE + x] = 0;
                }
            }
            else {
                if (cnt == 3) {
                    nextGen[y * GEN_SIZE + x] = 1;
                }
                else {
                    nextGen[y * GEN_SIZE + x] = 0;
                }
            }
        }
    }
}

void app() {
    unsigned gen1[GEN_SIZE * GEN_SIZE];
    unsigned gen2[GEN_SIZE * GEN_SIZE];
    unsigned* nextGen = gen1;
    unsigned* prevGen = gen2;

    for (int x = 0; x < GEN_SIZE; ++x) {
        for (int y = 0; y < GEN_SIZE; ++y) {
            prevGen[y * GEN_SIZE + x] = (simRand() % 6) == 0;
        }
    }
    simFlush();
    Sleep(300);

    unsigned i = 0;
    for (i = 0; i < REPEAT_SIZE; i++)
    {
        calcGen(nextGen, prevGen);
        drawGen(nextGen);
        unsigned* tmp = prevGen;
        prevGen = nextGen;
        nextGen = tmp;
        simFlush();
        Sleep(300);
    }
}