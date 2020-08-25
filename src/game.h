#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "map.h"
#include "ui.h"
#include "input.h"


typedef struct _Player
{
    int x;
    int y;
} Player;


typedef struct _GameState
{
    bool is_running;
    int worldViewX;
    int worldViewY;
    Map* map;
    Player player;
} GameState;



bool KEYS[ KEY_TOTAL ];
bool MOUSE[ MOUSE_TOTAL ];


SDL_Window* g_window;
SDL_Renderer* g_renderer;
GameState* gamestate;


int SCREEN_WIDTH;
int SCREEN_HEIGHT;
const int SCROLL_SPEED;



void error(const char* message);