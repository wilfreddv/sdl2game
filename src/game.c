#include "game.h"



bool KEYS[ KEY_TOTAL ] = { false };
bool MOUSE[ MOUSE_TOTAL ] = { false };


SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;
GameState* gamestate;


int SCREEN_WIDTH  = 1420;
int SCREEN_HEIGHT = 800;
const int SCROLL_SPEED = 10;


int initSDL();
void teardown();
void eventLoop();



int main(int argc, char** argv)
{
    if( !initSDL() )
        exit(-1);


    gamestate = malloc(sizeof(GameState));
    gamestate->is_running = true;
    gamestate->worldViewX = 0;
    gamestate->worldViewY = 0;
    if( !(gamestate->map = load_map(g_renderer, "resources/level1.map")) ) error("Could not open resources/level1.map");
    gamestate->player.x = SCREEN_WIDTH/2 - 20;
    gamestate->player.y = SCREEN_HEIGHT/2 - 20;

    // Main loop
    while( gamestate->is_running )
    {
        eventLoop();
        
        // Update view
        int RIGHT_BOUND = gamestate->map->horizontalTiles * 80;
        int LOWER_BOUND = gamestate->map->verticalTiles * 80;

        int* worldViewY = &gamestate->worldViewY;
        int* worldViewX = &gamestate->worldViewX;
        int* playerX    = &gamestate->player.x;
        int* playerY    = &gamestate->player.y;

        if(KEYS[ KEY_UP ])
        {
            /*
                If the player is between the center of the
                screen and bottom of the screen, move the 
                player.
            */
            if( *playerY > SCREEN_HEIGHT/2-20 )
                *playerY -= SCROLL_SPEED;
            else
            {
                *worldViewY -= SCROLL_SPEED;

                /*
                    If the worldview is shifted all the way up
                    move the player
                */
                if( *worldViewY <= 0 )
                {
                    *worldViewY = 0;
                    *playerY -= SCROLL_SPEED;
                    if( *playerY < 0 ) *playerY = 0;
                }
            }
        }
        if(KEYS[ KEY_DOWN ])
        {
            // Upper bound player check
            if( *playerY < SCREEN_HEIGHT/2-20 )
                *playerY += SCROLL_SPEED;
            else
            {
                *worldViewY += SCROLL_SPEED;

                if( *worldViewY >= LOWER_BOUND-SCREEN_HEIGHT )
                {
                    *worldViewY = LOWER_BOUND-SCREEN_HEIGHT;
                    *playerY += SCROLL_SPEED;
                    if( *playerY > LOWER_BOUND-*worldViewY-40 ) *playerY = LOWER_BOUND-*worldViewY-40;
                }
            }
        }

        if(KEYS[ KEY_LEFT ])
        {
            /*
                If the player is between the center of the
                screen and left of the screen, move the 
                player.
            */
            if( *playerX > SCREEN_WIDTH/2-20 )
                *playerX -= SCROLL_SPEED;
            else
            {
                *worldViewX -= SCROLL_SPEED;

                /*
                    If the worldview is shifted all the way to the left
                    move the player
                */
                if( *worldViewX <= 0 )
                {
                    *worldViewX = 0;
                    *playerX -= SCROLL_SPEED;
                    if( *playerX < 0 ) *playerX = 0;
                }
            }
        }
        if(KEYS[ KEY_RIGHT ])
        {
            // Right bound player check
            if( *playerX < SCREEN_WIDTH/2-20 )
                *playerX += SCROLL_SPEED;
            else
            {
                *worldViewX += SCROLL_SPEED;

                if( *worldViewX >= RIGHT_BOUND-SCREEN_WIDTH )
                {
                    *worldViewX = RIGHT_BOUND-SCREEN_WIDTH;
                    *playerX += SCROLL_SPEED;
                    if( *playerX > RIGHT_BOUND-*worldViewX-40 ) *playerX = RIGHT_BOUND-*worldViewX-40;
                }
            }
        }

        SDL_RenderClear(g_renderer);
        render_map(g_renderer, gamestate->map, *worldViewX, *worldViewY, SCREEN_WIDTH, SCREEN_HEIGHT);

        // Drawing player
        SDL_Rect rect;
        rect.h = 40;
        rect.w = 40;
        rect.x = *playerX;
        rect.y = *playerY;
        SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(g_renderer, &rect);

        draw_menu(g_renderer);

        SDL_RenderPresent(g_renderer);

        // TODO: Actual FPS, calculating time rendering took etc.
        // Only becomes necessary with more load per cycle
        // Also, game speed depends on FPS now, not good
        SDL_Delay(1000/60);
    }

    teardown();

    return 0;
}


int initSDL()
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    g_window = SDL_CreateWindow("Casual Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if( !g_window )
    {
        printf("SDL could not initialize window! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    Uint32 r_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    g_renderer = SDL_CreateRenderer(g_window, -0, r_flags);
    if( !g_renderer )
    {
        printf("SDL could not create renderer! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return 0;
    }

    if( TTF_Init() != 0 )
    {
        printf("TTF could not be initialized! TTF_Error: %s\n", TTF_GetError());
    }

    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);

    return 1;
}


void teardown()
{
    free(gamestate);
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
}


void eventLoop()
{
    // TODO: explore SDL_GetKeyboardState
    // It looks like our custom API, but more usable
    SDL_Event event;
    while( SDL_PollEvent(&event) != 0 )
    {
        switch( event.type )
        {
            case SDL_QUIT:
                gamestate->is_running = false;
                break;
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym )
                {
                    case SDLK_UP:
                        KEYS[ KEY_UP ] = true;
                        break;
                    case SDLK_DOWN:
                        KEYS[ KEY_DOWN ] = true;
                        break;
                    case SDLK_LEFT:
                        KEYS[ KEY_LEFT ] = true;
                        break;
                    case SDLK_RIGHT:
                        KEYS[ KEY_RIGHT ] = true;
                        break;
                    case SDLK_ESCAPE:
                        gamestate->is_running = false;
                        break;
                }
                break;
            case SDL_KEYUP:
                switch( event.key.keysym.sym )
                {
                    case SDLK_UP:
                        KEYS[ KEY_UP ] = false;
                        break;
                    case SDLK_DOWN:
                        KEYS[ KEY_DOWN ] = false;
                        break;
                    case SDLK_LEFT:
                        KEYS[ KEY_LEFT ] = false;
                        break;
                    case SDLK_RIGHT:
                        KEYS[ KEY_RIGHT ] = false;
                        break;
                }
                break;
        }
    }

    Uint32 m_buttons = SDL_GetMouseState(NULL, NULL);
    MOUSE[ MOUSE_LEFT ] = m_buttons & SDL_BUTTON_LMASK;
    MOUSE[ MOUSE_RIGHT ] = m_buttons & SDL_BUTTON_RMASK;
    MOUSE[ MOUSE_MIDDLE ] = m_buttons & SDL_BUTTON_MMASK;
    MOUSE[ MOUSE_X1 ] = m_buttons & SDL_BUTTON_X1MASK;
    MOUSE[ MOUSE_X2 ] = m_buttons & SDL_BUTTON_X2MASK;
}


void error(const char* message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, g_window);
    teardown();
    exit(1);
}