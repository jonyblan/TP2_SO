#include <standardlib.h>
#include <stdint.h>
#include <videolib.h>

#define W_PIX 1024
#define H_PIX 768

#define RED 0x00FF0000
#define GREEN 0x0000FF00
#define BLUE 0x000000FF
#define BLACK 0
#define PLAYER1_COLOR BLUE
#define PLAYER2_COLOR GREEN
#define PIXEL_SIZE 4
#define OFFSET_FROM_CORNER 4
#define WIDTH (W_PIX / PIXEL_SIZE - 2 * OFFSET_FROM_CORNER)
#define HEIGHT (H_PIX / PIXEL_SIZE - 2 * OFFSET_FROM_CORNER)
#define START_X_OFFSET (WIDTH / 8)
#define START_Y_OFFSET (WIDTH / 8)
#define UP 2
#define DOWN 1
#define LEFT 4
#define RIGHT 8
#define PLAYING 1
#define NOT_PLAYING 0

unsigned char playground[WIDTH][HEIGHT];

uint8_t speed = 1;
uint8_t p2Present = 1;
uint64_t p1wins = 0;
uint64_t p2wins = 0;
uint64_t singlePlayerDeaths = 0;

typedef struct
{
    uint8_t x;
    uint8_t y;
    uint8_t lost;
    uint32_t color;
    uint8_t prevMove;
    uint8_t nextMove;
    uint8_t playing
} Player;

void putLargePixel(uint32_t color, uint8_t x, uint8_t y, uint8_t n)
{
    drawRectangle(color, n * (x + OFFSET_FROM_CORNER), n * (y + OFFSET_FROM_CORNER), n * (x + OFFSET_FROM_CORNER + 1), n * (y + OFFSET_FROM_CORNER + 1));
}

unsigned char isOccupied(uint8_t x, uint8_t y)
{
    return playground[x][y];
}

void move(Player *p)
{

    if (p->lost)
        return;
    int x = (p->nextMove & 0x8) / 8 - (p->nextMove & 0x4) / 4;
    int y = (p->nextMove & 0x1) - (p->nextMove & 0x2) / 2;
    putLargePixel(p->color, p->x += x, p->y += y, PIXEL_SIZE);
    if (isOccupied(p->x, p->y))
        p->lost = 1;
    else
        playground[p->x][p->y] = 1;
    p->prevMove = p->nextMove;
}

void initPlayground()
{
    drawRectangle(RED, PIXEL_SIZE * (OFFSET_FROM_CORNER), PIXEL_SIZE * (OFFSET_FROM_CORNER), PIXEL_SIZE * (WIDTH + OFFSET_FROM_CORNER), PIXEL_SIZE * (HEIGHT + OFFSET_FROM_CORNER));
    drawRectangle(BLACK, PIXEL_SIZE * (OFFSET_FROM_CORNER + 1), PIXEL_SIZE * (OFFSET_FROM_CORNER + 1), PIXEL_SIZE * (WIDTH + OFFSET_FROM_CORNER - 1), PIXEL_SIZE * (HEIGHT + OFFSET_FROM_CORNER - 1));
    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
            playground[i][j] = 0;
    for (int i = 0; i < WIDTH; i++)
    {
        playground[i][0] = 1;
        playground[i][HEIGHT - 1] = 1;
    }
    for (int i = 0; i < HEIGHT; i++)
    {
        playground[0][i] = 1;
        playground[WIDTH - 1][i] = 1;
    }
}

void play(Player *p1, Player *p2)
{
    wait(speed);
    uint8_t c;
    uint64_t l;
    while (getNextToRead(&c) > 0)
        switch (c)
        {
        case 'w':;
            if (p1->prevMove != DOWN)
                p1->nextMove = UP;
            break;
        case 'a':;
            if (p1->prevMove != RIGHT)
                p1->nextMove = LEFT;
            break;
        case 's':;
            if (p1->prevMove != UP)
                p1->nextMove = DOWN;
            break;
        case 'd':;
            if (p1->prevMove != LEFT)
                p1->nextMove = RIGHT;
            break;
        case 'i':;
            if (p2->prevMove != DOWN)
                p2->nextMove = UP;
            break;
        case 'j':;
            if (p2->prevMove != RIGHT)
                p2->nextMove = LEFT;
            break;
        case 'k':;
            if (p2->prevMove != UP)
                p2->nextMove = DOWN;
            break;
        case 'l':;
            if (p2->prevMove != LEFT)
                p2->nextMove = RIGHT;
            break;
        }

    if (p2->playing)
        move(p2); // porque en el juego original si se chocan gana p2
    if (p1->playing)
        move(p1);
}

void settings()
{
    printf("Do you want to play in two player mode? (y/n)\n");
    char c;
    do
    {
        getNextToRead(&c);
        p2Present = c == 'y';
    } while (c != 'y' && c != 'n');
    printf("Select your speed from 1 to 4\n");
    do
    {
        getNextToRead(&c);
        speed = 5 - (c - '0');
    } while (c != '1' && c != '2' && c != '3' && c != '4');
}

void eliminator()
{
    printf("Press space to %s, s for settings or q to quit the game.\n", (p1wins + p2wins) ? "keep playing" : "play");
    char c;
    while (1)
    {
        getNextToRead(&c);
        if (c == ' ')
        {
            clearScreen();
            game();
            return;
        }
        if (c == 's')
        {
            settings();
            clearScreen();
            game();
            return;
        }
        if (c == 'q')
        {
            clearScreen();
            p1wins = 0;
            p2wins = 0;
            singlePlayerDeaths = 0;
            return;
        }
    }
    clearScreen();
}

void game()
{
    initPlayground();
    Player p1 = {START_X_OFFSET, START_Y_OFFSET, 0, PLAYER1_COLOR, RIGHT, RIGHT, PLAYING};
    Player p2 = {WIDTH - START_X_OFFSET, HEIGHT - START_Y_OFFSET, 0, PLAYER2_COLOR, LEFT, LEFT, p2Present ? PLAYING : NOT_PLAYING};
    while (!p1.lost && !p2.lost)
        play(&p1, &p2);
    if (p1.lost)
    {
        if (p2Present)
            p2wins++;
        else
            singlePlayerDeaths++;
    }
    else
        p1wins++;
    if (p2Present)
    {
        printf("P1 SCORE : %d ", p1wins);
        printf("P2 SCORE : %d \n", p2wins);
    }
    else
    {
        printf("P1 DEATHS : %d\n", singlePlayerDeaths);
    }
    beep(192, 30);
    eliminator();
    clearScreen();
}
