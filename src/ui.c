#include "game.h"



SDL_Color WHITE = {255, 255, 255};
SDL_Color RED   = {255, 0, 0};
SDL_Color GREEN = {0, 255, 0};
SDL_Color BLUE  = {0, 0, 255};
SDL_Color BLACK = {0, 0, 0};



Animation* load_animation(const char* file, int width, int height, int amount, int x, int y, int framesPerSprite, bool loops)
{
    Animation* animation = malloc(sizeof(Animation));

    SDL_Texture* texture = IMG_LoadTexture(g_renderer, file);
    if( !texture )
    {
        // TODO: handle it
        animation->success = false;
        return animation;
    }

    animation->x = x;
    animation->y = y;
    animation->amountOfSprites = amount;
    animation->currentSprite = 0;
    animation->framesPerSprite = framesPerSprite;
    animation->currentFrame = 0;
    animation->texture = texture;
    animation->clipRect = (SDL_Rect) {0, 0, width, height};
    animation->loops = loops;
    animation->success = true;

    return animation;
}


AnimationList* create_animation_list()
{
    AnimationList* list = malloc(sizeof(AnimationList));
    if( list == NULL )
    {
        printf("Could not alloc for animation list\n");
        return NULL;
    }
    list->animation = NULL;
    list->next = NULL;
    return list;
}


void add_animation(AnimationList* list, Animation* animation)
{
    if( list->animation == NULL )
    {
        list->animation = animation;
    }
    else
    {
        // Search end
        while( list->next ) list = list->next;
        
        AnimationList* new = malloc(sizeof(AnimationList));
        new->animation = animation;
        new->next = NULL;
        list->next = new;
    }
}


void update_and_draw_vfx(AnimationList* animations)
{
    // Includes particles, explosions, n shit


    // WARNING: Introduces possible bug if no Animation is given
    // but the list is longer than just this one.
    if( animations == NULL )
    {   // Nothing to do
        return;
    }


    AnimationList* previous = NULL;
    do
    {
        Animation* animation = animations->animation;
        if( animation == NULL )
            continue;
        // Check if animation is visible
        // TODO: Unhardcode size of w and h, scale it more properly bro
        // *8 and *4 should be removed
        if( animation->x + animation->clipRect.w*4 > gamestate->worldViewX &&
            animation->x - animation->clipRect.w*4 < gamestate->worldViewX+SCREEN_WIDTH &&
            animation->y + animation->clipRect.h*4 > gamestate->worldViewY &&
            animation->y - animation->clipRect.h*4 < gamestate->worldViewY+SCREEN_HEIGHT)
        {
            SDL_Rect destRect = {animation->x - gamestate->worldViewX - animation->clipRect.w/2,
                                 animation->y - gamestate->worldViewY - animation->clipRect.h/2,
                                 animation->clipRect.w*8,
                                 animation->clipRect.h*8};
            SDL_RenderCopy(g_renderer, animation->texture, &animation->clipRect, &destRect);
        }


        animation->currentFrame++;
        if( animation->currentFrame == animation->framesPerSprite )
        {
            animation->currentSprite = (animation->currentSprite + 1) % animation->amountOfSprites;
            if( animation->currentSprite == 0 ) // Reached 0 AGAIN
            {
                AnimationList* next = animations->next;
                if( !previous ) // We are on HEAD
                {
                    free(animation);
                    animations->animation = NULL;
                }
                else
                {
                    free(animation);
                    free(animations);
                    previous->next = next;
                }
            }
            animation->currentFrame = 0;
        }
        animation->clipRect.x = animation->clipRect.w * animation->currentSprite;

        previous = animations;
    } while( (animations = animations->next) );
}


SDL_Surface* get_text_surface(const char* text, const char* font, int size)
{
    TTF_Font* ttf_font = TTF_OpenFont(font, size);
    if( ttf_font == NULL )
    {
        char msg[100] = "Could not open font: ";
        strcat(msg, font);
        error(msg);
    }
    SDL_Surface* surface = TTF_RenderText_Solid(ttf_font, text, WHITE);
    TTF_CloseFont(ttf_font);
    return surface;
}


void draw_button(SDL_Rect* rect, const char* text, void(* callback)(), void* callbackArg)
{
    SDL_SetRenderDrawColor(g_renderer, 0x80, 0x80, 0x80, 0xFF);
    
    int oldW = rect->w;
    int oldX = rect->x;

    if( text )
    {
        SDL_Surface* textSurface = get_text_surface(text, "resources/fonts/FreeMono.ttf", 50);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_renderer, textSurface);
        SDL_Rect surfRect;
        SDL_GetClipRect(textSurface, &surfRect);

        if( surfRect.w+20 > rect->w )
        {
            rect->x = rect->x - (surfRect.w - rect->w)/2;
            rect->w = surfRect.w + 20;
        }
        surfRect.x = rect->x - (surfRect.w - rect->w)/2;
        surfRect.y = rect->y - (surfRect.h - rect->h)/2;
        
        SDL_RenderFillRect(g_renderer, rect);

        SDL_RenderCopy(g_renderer, textTexture, NULL, &surfRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    else
    {
        SDL_RenderFillRect(g_renderer, rect);
    }
    

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if( rect->x < mouseX && rect->x+rect->w > mouseX &&
        rect->y < mouseY && rect->y+rect->h > mouseY )
    {
        if( MOUSE[ MOUSE_LEFT ] ) callback(callbackArg);
        SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0x00, 0x30);
        SDL_RenderFillRect(g_renderer, rect);
    }

    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRect(g_renderer, rect);

    rect->w = oldW;
    rect->x = oldX;
}


void draw_menu()
{
    SDL_Rect rect;
    rect.x = (SCREEN_WIDTH-200)/2;
    rect.y = 100;
    rect.h = 120;
    rect.w = 240;

    draw_button(&rect, 0, error, "Goodbye my lover,\nGoodbye my friend!");
    rect.y += rect.h + 50;
    draw_button(&rect, "long text button", error, "Goodbye my lover,\nGoodbye my friend!");
    rect.y += rect.h + 50;
    draw_button(&rect, "v", error, "Goodbye my lover,\nGoodbye my friend!");
    rect.y += rect.h + 50;
    draw_button(&rect, "Quit", EXIT_GAME, NULL);
}