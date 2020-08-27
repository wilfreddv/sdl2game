#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>


typedef struct _Animation
{
    int x;  // x and y are worldview coords
    int y;  // and point to the origin/center of the sprite
    int amountOfSprites;
    int currentSprite;
    int framesPerSprite;  // Control animation speed
    int currentFrame;
    SDL_Texture* texture;
    SDL_Rect clipRect;
    bool loops;
    bool success;
} Animation;


typedef struct _AnimationList
{
    Animation animation;
    struct _AnimationList* next;
} AnimationList;


Animation load_animation(const char* file, int width, int height, int amount, int framesPerSprite, bool loops);
AnimationList* create_animation_list(Animation);
void add_animation(AnimationList*, Animation);
void update_and_draw_vfx(AnimationList*);
void draw_hud();
void draw_menu();