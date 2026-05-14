#include "Screen.h"

boo::Screen::Screen(void) :
	tilemap{}, minimap_x{ 0x10 }, minimap_y{ 0x10 },
	scroll_left{ 0xff }, scroll_right{ 0xff },
	sprite_pal0_offset{ 0x00 }, sprite_pal1_offset{ 0x00 }
{
}
