#define GEN_SIZE 50
#define BLOCK_SIZE 10
#define WINDOW_WIDTH GEN_SIZE * BLOCK_SIZE
#define REPEAT_SIZE 5

void simFlush();
void simPutPixel(int x, int y, int argb);
int simRand();