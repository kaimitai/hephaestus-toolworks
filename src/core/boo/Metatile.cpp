#include "Metatile.h"

boo::Metatile::Metatile(byte p_tl, byte p_tr, byte p_bl, byte p_br, byte p_packed_properties) :
	tilemap{ p_tl, p_tr, p_bl, p_br },
	palette_no{ static_cast<byte>((p_packed_properties & 0b00000011)) },
	properties{ static_cast<byte>((p_packed_properties & 0b11111100) >> 2) }
{
}

boo::Metatile::Metatile(void) : Metatile(0, 0, 0, 0, 0)
{
}
