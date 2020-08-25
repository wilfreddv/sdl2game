#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>



typedef struct _Map
{
    int verticalTiles;
    int horizontalTiles;
    SDL_Texture *(*tiles);
} Map;


Map* load_map(SDL_Renderer* renderer, const char* path);
void delete_map(Map* map);
void render_map(SDL_Renderer* renderer, Map* map, int worldOffsetX, int worldOffsetY, int screenWidth, int screenHeight);
