#include "game.h"



void render_map(SDL_Renderer* renderer, Map* map, int worldOffsetX, int worldOffsetY, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
    // TODO: fix bug for maps that are too small
    SDL_Rect board_rect;
    board_rect.h = 80;
    board_rect.w = 80;
    
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

    for(int row=0; row<map->verticalTiles; row++)
    {
        for(int column=0; column<map->horizontalTiles; column++)
        {
            // Check if tile in screen
            if( column*80 >= worldOffsetX-79 &&
                row*80    >= worldOffsetY-79 &&
                column*80 <= worldOffsetX+SCREEN_WIDTH-1 &&
                row*80    <= worldOffsetY+SCREEN_HEIGHT-1)
            {
                board_rect.x = column*80 - worldOffsetX;
                board_rect.y = row*80 - worldOffsetY;
                
                SDL_Texture* texture = map->tiles[ (row*map->horizontalTiles) + column ];
                if( texture )
                    SDL_RenderCopy(renderer, texture, NULL, &board_rect);
                else
                    SDL_RenderFillRect(renderer, &board_rect);
            }
        }
    }
}


Map* load_map(SDL_Renderer* renderer, const char* path)
{

    FILE* mapFile = fopen(path, "r");
    if( !mapFile )
    {
        return NULL;
    }

    // TODO: check if fscanf fails
    int amountOfTextures;
    Map* map = malloc(sizeof(Map));
    fscanf(mapFile, "%d %d %d", &(map->horizontalTiles), &(map->verticalTiles), &amountOfTextures);

    SDL_Texture *textures[amountOfTextures];
    char t_filename[50];
    for( int i=0; i<amountOfTextures; i++ )
    {
        char t_path[100] = "resources/textures/";
        fscanf(mapFile, " %s", t_filename);
        strcat(t_path, t_filename);
        textures[i] = IMG_LoadTexture(renderer, t_path);
    }


    // Trailing newline
    fgetc(mapFile);

    int mapTotalSize = map->horizontalTiles * map->verticalTiles;
    SDL_Texture **tileMapPtr = malloc(sizeof(SDL_Texture*) * mapTotalSize);
    
    int textureIndex;
    for( int i=0; i<mapTotalSize; i++ )
    {
        if( (i+1)%map->horizontalTiles == 0 )
        {
            fscanf(mapFile, "%d\n", &textureIndex);
        }
        else
        {
            fscanf(mapFile, "%d ", &textureIndex);
        }

        tileMapPtr[i] = textures[textureIndex];
    }

    fclose(mapFile);

    map->tiles = tileMapPtr;

    return map;
}