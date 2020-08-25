#define IS_MOUSEBUTTON_HELD(mouse_button) (MOUSE[ mouse_button ])
#define IS_KEY_PRESSED(key)               (KEYS[ key ])



enum KeyPresses
{
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_M,
    KEY_TOTAL
};


enum MouseButtons
{
    MOUSE_LEFT,
    MOUSE_RIGHT,
    MOUSE_MIDDLE,
    MOUSE_X1,
    MOUSE_X2,
    MOUSE_TOTAL
};