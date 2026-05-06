#include "htConfig.h"
#include "config_constants.h"
#include <format>
#include <stdexcept>

using script::Flow;
using script::ArgDomain;
using script::ArgType;

ht::htConfig::htConfig(void) {
	// TODO: Get from xml
	addresses = {
		{c::ID_WORLD_DEFINITIONS_ADDR, Address(7, 0xcfb0)},
		{c::ID_CHR_TRANSFERSCRIPT_ADDR, Address(7, 0xe633)},
		{c::ID_SHARED_BG_PALETTE_ADDR, Address(7, 0xd016)}
	};

	counts = {
		{c::ID_WORLD_COUNT, 12},
		{c::ID_SPRITE_CHR_BANK_COUNT, 52},
		{c::ID_CHR_TRANSFERSCRIPT_COUNT, 22},
	};

	scriptConfig.opcodes = {
	{0x00, {"SetNPC", Flow::Continue, ArgType::Byte, ArgDomain::NPC}},
	{0x01, {"IfNotFlag", Flow::ConditionalJump, ArgType::Byte, ArgDomain::Flag}},
	{0x02, {"Jump", Flow::Jump, ArgType::None, ArgDomain::None}},
	{0x03, {"Msg", Flow::Continue, ArgType::PointerString, ArgDomain::None}},
	{0x04, {"End", Flow::End, ArgType::None, ArgDomain::None}},
	{0x05, {"SetFlag", Flow::Continue, ArgType::Byte, ArgDomain::Flag}},
	{0x06, {"ClearFlag", Flow::Continue, ArgType::Byte, ArgDomain::Flag}},
	{0x07, {"IfFlag", Flow::ConditionalJump, ArgType::Byte, ArgDomain::Flag}},
	{0x08, {"GetItem", Flow::Continue, ArgType::Byte, ArgDomain::Flag}},
	{0x09, {"MsgEnd", Flow::End, ArgType::ImmediateString, ArgDomain::None}},
	{0x0a, {"IfDecline", Flow::ConditionalJump, ArgType::None, ArgDomain::None}},
	{0x0b, {"IfSkinsLessThan20", Flow::ConditionalJump, ArgType::None, ArgDomain::None}},
	{0x0c, {"IfOlivesLessThan", Flow::ConditionalJump, ArgType::Byte, ArgDomain::None}},
	{0x0d, {"PasswordEnd", Flow::End, ArgType::None, ArgDomain::None}},
	{0x0e, {"HealthEnd", Flow::End, ArgType::None, ArgDomain::None}},
	{0x0f, {"NectarEnd", Flow::End, ArgType::None, ArgDomain::None}}
	};

	scriptConfig.defines = {
		{
		ArgDomain::Flag, {
			{0x00, "ITEM_CLUB"},
			{0x01, "ITEM_STAFF_OF_FENNEL"},
			{0x02, "ITEM_NYMPH_SWORD"},
			{0x03, "ITEM_DIVINE_SWORD"},
			{0x04, "ITEM_HARP"},
			{0x05, "ITEM_OCARINA"},
			{0x08, "ITEM_MOON_ORB"},
			{0x09, "ITEM_SANDALS"},
			{0x0a, "ITEM_ATHENAS_SHIELD"},
			{0x0b, "ITEM_SALAMANDER_SHIELD"},
			{0x0c, "ITEM_BRACELET"},
			{0x0d, "ITEM_KEY"},
			{0x0e, "ITEM_LOVE_FRAGMENT_1"},
			{0x0f, "ITEM_LOVE_FRAGMENT_2"},
			{0x10, "ITEM_LOVE_FRAGMENT_3"},
			{0x12, "FLAG_ZEUS_MET"},
			{0x14, "ITEM_AMBROSIA_TREE"},
			{0x16, "ITEM_AMBROSIA_1"},
			{0x17, "ITEM_AMBROSIA_2"},
			{0x18, "ITEM_AMBROSIA_3"},
			{0x1c, "FLAG_NECTAR_FULL"},
			{0x1f, "FLAG_CHILD_RESCUED"},
			{0x20, "FLAG_HERMES_MISSED"},
			{0x21, "FLAG_FENNEL_UPGRADED"},
			{0x22, "FLAG_HYDRA_KILLED"}
			}
		},
			{
		ArgDomain::NPC, {
			{0x00, "NPC_WOMAN_LONG_GREEN"},
			{0x01, "NPC_WOMAN_BUN_GREEN"},
			{0x02, "NPC_MAN_BEARD_GREEN"},
			{0x03, "NPC_MAN_BALD_GREEN"},
			{0x04, "NPC_CHILD_GREEN"},
			{0x05, "NPC_MAN_GREEN"},
			{0x06, "NPC_MAN_STAFF_GREEN"},
			{0x07, "NPC_WOMAN_STAFF_GREEN"},

			{0x08, "NPC_WOMAN_LONG_PINK"},
			{0x09, "NPC_WOMAN_BUN_PINK"},
			{0x0a, "NPC_MAN_BEARD_PINK"},
			{0x0b, "NPC_MAN_BALD_PINK"},
			{0x0c, "NPC_CHILD_PINK"},
			{0x0d, "NPC_MAN_PINK"},
			{0x0e, "NPC_MAN_STAFF_PINK"},
			{0x0f, "NPC_WOMAN_STAFF_PINK"},

			{0x10, "NPC_WOMAN_LONG_ORANGE"},
			{0x11, "NPC_WOMAN_BUN_ORANGE"},
			{0x12, "NPC_MAN_BEARD_ORANGE"},
			{0x13, "NPC_MAN_BALD_ORANGE"},
			{0x14, "NPC_CHILD_ORANGE"},
			{0x15, "NPC_MAN_ORANGE"},
			{0x16, "NPC_MAN_STAFF_ORANGE"},
			{0x17, "NPC_WOMAN_STAFF_ORANGE"},

			{0x18, "NPC_WOMAN_LONG_RED"},
			{0x19, "NPC_WOMAN_BUN_RED"},
			{0x1a, "NPC_MAN_BEARD_RED"},
			{0x1b, "NPC_MAN_BALD_RED"},
			{0x1c, "NPC_CHILD_RED"},
			{0x1d, "NPC_MAN_RED"},
			{0x1e, "NPC_MAN_STAFF_RED"},
			{0x1f, "NPC_WOMAN_STAFF_RED"},

			{0x20, "NPC_WOMAN_LONG_GREY"},
			{0x21, "NPC_WOMAN_BUN_GREY"},
			{0x22, "NPC_MAN_BEARD_GREY"},
			{0x23, "NPC_MAN_BALD_GREY"},
			{0x24, "NPC_CHILD_GREY"},
			{0x25, "NPC_MAN_GREY"},
			{0x26, "NPC_MAN_STAFF_GREY"},
			{0x27, "NPC_WOMAN_STAFF_GREY"},

			{0x28, "NPC_WOMAN_LONG_BLUE"},
			{0x29, "NPC_WOMAN_BUN_BLUE"},
			{0x2a, "NPC_MAN_BEARD_BLUE"},
			{0x2b, "NPC_MAN_BALD_BLUE"},
			{0x2c, "NPC_CHILD_BLUE"},
			{0x2d, "NPC_MAN_BLUE"},
			{0x2e, "NPC_MAN_STAFF_BLUE"},
			{0x2f, "NPC_WOMAN_STAFF_BLUE"},

			{0x30, "NPC_WOMAN_LONG_WHITE"},
			{0x31, "NPC_WOMAN_BUN_WHITE"},
			{0x32, "NPC_MAN_BEARD_WHITE"},
			{0x33, "NPC_MAN_BALD_WHITE"},
			{0x34, "NPC_CHILD_WHITE"},
			{0x35, "NPC_MAN_WHITE"},
			{0x36, "NPC_MAN_STAFF_WHITE"},
			{0x37, "NPC_WOMAN_STAFF_WHITE"},

			{0x38, "NPC_WOMAN_LONG_BLACK"},
			{0x39, "NPC_WOMAN_BUN_BLACK"},
			{0x3a, "NPC_MAN_BEARD_BLACK"},
			{0x3b, "NPC_MAN_BALD_BLACK"},
			{0x3c, "NPC_CHILD_BLACK"},
			{0x3d, "NPC_MAN_BLACK"},
			{0x3e, "NPC_MAN_STAFF_BLACK"},
			{0x3f, "NPC_WOMAN_STAFF_BLACK"},

			{0x40, "NPC_ZEUS"},
			{0x41, "NPC_GODDESS"},
			{0x42, "NPC_GOD"},
			{0x43, "NPC_GOD_MESSENGER"},

			{0x60, "NPC_FAIRY_PINK"},
			{0x61, "NPC_FAIRY_GREEN"},
			{0x62, "NPC_FAIRY_PURPLE"},
			{0x63, "NPC_FAIRY_BLUE"},

			{0x80, "NPC_NOBODY"},
			{0xc0, "NPC_WALL"}
			}
		}
	};

	nes_palette = {
	{0x66,0x66,0x66}, {0x00,0x2a,0x88}, {0x14,0x12,0xa7}, {0x3b,0x00,0xa4},
	{0x5c,0x00,0x7e}, {0x6e,0x00,0x40}, {0x6c,0x07,0x00}, {0x56,0x1d,0x00},
	{0x33,0x35,0x00}, {0x0c,0x48,0x00}, {0x00,0x52,0x00}, {0x00,0x4f,0x08},
	{0x00,0x40,0x4d}, {0x00,0x00,0x00}, {0x00,0x00,0x00}, {0x00,0x00,0x00},

	{0xad,0xad,0xad}, {0x15,0x5f,0xd9}, {0x42,0x40,0xff}, {0x75,0x27,0xfe},
	{0xa0,0x1a,0xcc}, {0xb7,0x1e,0x7b}, {0xb5,0x31,0x20}, {0x99,0x4e,0x00},
	{0x6b,0x6d,0x00}, {0x38,0x87,0x00}, {0x0d,0x93,0x00}, {0x00,0x8f,0x32},
	{0x00,0x7c,0x8d}, {0x00,0x00,0x00}, {0x00,0x00,0x00}, {0x00,0x00,0x00},

	{0xff,0xff,0xff}, {0x64,0xb0,0xff}, {0x92,0x90,0xff}, {0xc6,0x76,0xff},
	{0xf2,0x6a,0xff}, {0xff,0x6e,0xcc}, {0xff,0x81,0x70}, {0xea,0x9e,0x22},
	{0xbc,0xbe,0x00}, {0x88,0xd8,0x00}, {0x5c,0xe4,0x30}, {0x45,0xe0,0x82},
	{0x48,0xcd,0xde}, {0x4f,0x4f,0x4f}, {0x00,0x00,0x00}, {0x00,0x00,0x00},

	{0xff,0xff,0xff}, {0xc0,0xdf,0xff}, {0xd3,0xd2,0xff}, {0xe8,0xc8,0xff},
	{0xfa,0xc2,0xff}, {0xff,0xc4,0xea}, {0xff,0xcc,0xc5}, {0xf7,0xd8,0xa5},
	{0xe4,0xe5,0x94}, {0xcf,0xef,0x96}, {0xbd,0xf4,0xab}, {0xb3,0xf3,0xcc},
	{0xb5,0xeb,0xf2}, {0xb8,0xb8,0xb8}, {0x00,0x00,0x00}, {0x00,0x00,0x00}
	};
}

const ht::ScriptConfig& ht::htConfig::get_script_config(void) const {
	return scriptConfig;
}

std::size_t ht::htConfig::count(const std::string& p_id) const {
	auto iter{ counts.find(p_id) };
	if (iter != end(counts))
		return iter->second;
	else throw std::runtime_error(
		std::format("Could not find count constant '{}'", p_id)
	);
}

ht::Address ht::htConfig::address(const std::string& p_id) const {
	auto iter{ addresses.find(p_id) };
	if (iter != end(addresses))
		return iter->second;
	else throw std::runtime_error(
		std::format("Could not find address constant '{}'", p_id)
	);
}

const std::vector<ht::RGBColor>& ht::htConfig::get_nes_palette(void) const {
	return nes_palette;
}
