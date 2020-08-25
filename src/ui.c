#include "game.h"



SDL_Color WHITE = {255, 255, 255};
SDL_Color RED   = {255, 0, 0};
SDL_Color GREEN = {0, 255, 0};
SDL_Color BLUE  = {0, 0, 255};
SDL_Color BLACK = {0, 0, 0};



SDL_Texture* get_text_texture(SDL_Renderer* renderer, const char* text, const char* font, int size)
{
    TTF_Font* ttf_font = TTF_OpenFont(font, size);
    if( ttf_font == NULL )
    {
        char msg[100] = "Could not open font: ";
        strcat(msg, font);
        error(msg);
    }
    SDL_Surface* surface = TTF_RenderText_Solid(ttf_font, text, WHITE);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void draw_button(SDL_Renderer* renderer, SDL_Rect* rect, const char* text, void(* callback)(), void* callbackArg)
{
    SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xFF);
    SDL_RenderFillRect(renderer, rect);

    SDL_Texture* textTexture = get_text_texture(renderer, text, "resources/fonts/FreeMono.ttf", 50);
    SDL_RenderCopy(renderer, textTexture, NULL, rect);
    SDL_DestroyTexture(textTexture);

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    if( rect->x < mouseX && rect->x+rect->w > mouseX &&
        rect->y < mouseY && rect->y+rect->h > mouseY )
    {
        if( MOUSE[ MOUSE_LEFT ] ) callback(callbackArg);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x30);
        SDL_RenderFillRect(renderer, rect);
    }

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderDrawRect(renderer, rect);
}


void draw_menu(SDL_Renderer* renderer)
{
    SDL_Rect rect;
    rect.x = 50;
    rect.y = 50;
    rect.h = 120;
    rect.w = 240;

    draw_button(renderer, &rect, "Quit", error, "Goodbye my lover,\nGoodbye my friend!");
}