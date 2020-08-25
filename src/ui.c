#include "game.h"



SDL_Color WHITE = {255, 255, 255};
SDL_Color RED   = {255, 0, 0};
SDL_Color GREEN = {0, 255, 0};
SDL_Color BLUE  = {0, 0, 255};
SDL_Color BLACK = {0, 0, 0};




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
    

    SDL_Surface* textSurface = get_text_surface(text, "resources/fonts/FreeMono.ttf", 50);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_renderer, textSurface);
    SDL_Rect surfRect;
    SDL_GetClipRect(textSurface, &surfRect);

    int oldW = rect->w;
    int oldX = rect->x;
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
    draw_button(&rect, "Quit", error, "Goodbye my lover,\nGoodbye my friend!");
}

