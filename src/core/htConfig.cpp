#include "htConfig.h"

using script::Flow;
using script::ArgDomain;
using script::ArgType;

ht::htConfig::htConfig(void) {

	// TODO: Get from xml
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
			{0x1c, "FLAG_NECTAR_GOT"},
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
}

const ht::ScriptConfig& ht::htConfig::get_script_config(void) const {
	return scriptConfig;
}
