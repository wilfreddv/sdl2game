#include "game.h"



bool KEYS[ KEY_TOTAL ] = { false };
bool KEY_PRESSED[ KEY_TOTAL ] = { false };
bool KEY_RELEASED[ KEY_TOTAL ] = { false };
bool MOUSE[ MOUSE_TOTAL ] = { false };


SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;
GameState* gamestate;


// int SCREEN_WIDTH  = 1420;
// int SCREEN_HEIGHT = 820;
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
    gamestate->windowResized = false;

    int showingMenu = 0;
    int mReleased = 1;


    AnimationList* animations = create_animation_list();
    Animation* a = load_animation("resources/textures/circle_sprites.png", 16, 16, 6, 420, 420, 20, true);
    add_animation(animations, a);


    int RIGHT_BOUND = gamestate->map->horizontalTiles * TILE_WIDTH;
    int LOWER_BOUND = gamestate->map->verticalTiles * TILE_HEIGHT;


    // Main loop
    while( gamestate->is_running )
    {
        eventLoop();
        
        /* The idea is good but needs more brainsmarts
        if( gamestate->windowResized )
        {
            int oldWidth = SCREEN_WIDTH;
            int oldHeight = SCREEN_HEIGHT;
            SDL_GetWindowSize(g_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
            
            gamestate->worldViewX += oldWidth - SCREEN_WIDTH;
            gamestate->worldViewY += oldHeight - SCREEN_HEIGHT;

            gamestate->windowResized = false;
        }*/


        // TODO: if x_BOUND are smaller than SCREEN_W/H, the map is
        // smaller than the screen. Center the map, don't update worldview
        // and only move the player
        // Update view

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

        if( KEY_PRESSED[ KEY_SPACE ] )
        {
            add_animation(animations, load_animation("resources/textures/circle_sprites.png", 16, 16, 6,
                          *playerX+*worldViewX, *playerY+*worldViewY, 20, true));
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


        update_and_draw_vfx(animations);


        if( KEY_PRESSED[ KEY_M ] )
        {
            if( mReleased )
            {
                showingMenu = showingMenu ? 0: 1;
                SDL_ShowCursor(SDL_ShowCursor(-1) ? 0: 1);
                mReleased = 0;
            }
        }
        if( !mReleased ) mReleased = KEY_RELEASED[ KEY_M ];

        // TODO: Pause game when menu is showing
        if( showingMenu )
            draw_menu();
        

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
                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_FULLSCREEN_DESKTOP);
    if( !g_window )
    {
        printf("SDL could not initialize window! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    SDL_GetWindowSize(g_window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
    SDL_ShowCursor(0);

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
    for( int i=0; i<KEY_TOTAL; i++ ) KEY_PRESSED[i] = false;
    for( int i=0; i<KEY_TOTAL; i++ ) KEY_RELEASED[i] = false;
    
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
                        KEY_PRESSED[ KEY_UP ] = true;
                        break;
                    case SDLK_DOWN:
                        KEYS[ KEY_DOWN ] = true;
                        KEY_PRESSED[ KEY_DOWN ] = true;
                        break;
                    case SDLK_LEFT:
                        KEYS[ KEY_LEFT ] = true;
                        KEY_PRESSED[ KEY_LEFT ] = true;
                        break;
                    case SDLK_RIGHT:
                        KEYS[ KEY_RIGHT ] = true;
                        KEY_PRESSED[ KEY_RIGHT ] = true;
                        break;
                    case SDLK_m:
                        KEYS[ KEY_M ] = true;
                        KEY_PRESSED[ KEY_M ] = true;
                        break;
                    case SDLK_SPACE:
                        KEYS[ KEY_SPACE ] = true;
                        KEY_PRESSED[ KEY_SPACE ] = true;
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
                        KEY_RELEASED[ KEY_UP ] = true;
                        break;
                    case SDLK_DOWN:
                        KEYS[ KEY_DOWN ] = false;
                        KEY_RELEASED[ KEY_DOWN ] = true;
                        break;
                    case SDLK_LEFT:
                        KEYS[ KEY_LEFT ] = false;
                        KEY_RELEASED[ KEY_LEFT ] = true;
                        break;
                    case SDLK_RIGHT:
                        KEYS[ KEY_RIGHT ] = false;
                        KEY_RELEASED[ KEY_RIGHT ] = true;
                        break;
                    case SDLK_m:
                        KEYS[ KEY_M ] = false;
                        KEY_RELEASED[ KEY_M ] = true;
                        break;
                    case SDLK_SPACE:
                        KEYS[ KEY_SPACE ] = false;
                        KEY_RELEASED[ KEY_SPACE ] = true;
                        break;
                }
                break;
            case SDL_WINDOWEVENT_RESIZED:
                gamestate->windowResized = true;
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

void EXIT_GAME(){ gamestate->is_running = false; }