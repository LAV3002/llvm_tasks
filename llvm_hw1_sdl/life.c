#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include <windows.h>


#define GEN_SIZE 70
#define BLOCK_SIZE 10
#define WINDOW_WIDTH GEN_SIZE * BLOCK_SIZE
#define REPEAT_SIZE 500

SDL_Event event;
SDL_Renderer* renderer;
SDL_Window* window;
Uint32 pixels[WINDOW_WIDTH * WINDOW_WIDTH];
unsigned gen1[GEN_SIZE * GEN_SIZE];
unsigned gen2[GEN_SIZE * GEN_SIZE];
unsigned* nextGen = gen1;
unsigned* prevGen = gen2;

void put_pixel(int x, int y, Uint32 argb) {
    pixels[y * WINDOW_WIDTH + x] = argb;
}

void draw_shape(int x, int y, Uint32 argb) {
    unsigned i = 0;
    unsigned j = 0;
    for (i = x * BLOCK_SIZE; i < (x + 1) * BLOCK_SIZE; ++i) {
        for (j = y * BLOCK_SIZE; j < (y + 1) * BLOCK_SIZE; ++j)
        {
            put_pixel(i, j, argb);
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

void flush() {

}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_WIDTH, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WINDOW_WIDTH, WINDOW_WIDTH);

    for (int x = 0; x < GEN_SIZE; ++x) {
        for (int y = 0; y < GEN_SIZE; ++y) {
            prevGen[y * GEN_SIZE + x] = (rand() % 6) == 0;
        }
    }

    drawGen(prevGen);
    SDL_UpdateTexture(texture, NULL, pixels, WINDOW_WIDTH * sizeof(Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    unsigned i = 0;
    for (i = 0; i < REPEAT_SIZE; i++)
    {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            break;
        }
        calcGen(nextGen, prevGen);
        drawGen(nextGen);
        unsigned* tmp = prevGen;
        prevGen = nextGen;
        nextGen = tmp;
        SDL_UpdateTexture(texture, NULL, pixels, WINDOW_WIDTH * sizeof(Uint32));
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        Sleep(300);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}