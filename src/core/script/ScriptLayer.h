#ifndef SCRIPTLAYER_H
#define SCRIPTLAYER_H

#include <unordered_map>
#include <string>
#include <vector>
#include "Instruction.h"

namespace script {

	struct ScriptLayer {
		// maps instruction index to vector of (world, room) having that instruction as entrypt
		std::unordered_map<std::size_t, std::vector<std::pair<std::size_t, std::size_t>>> ptr_table;
		// sequential instructions; instructions with string refs have index values that reference...
		std::vector<Instruction> instructions;
		// ...the actual strings
		std::vector<std::string> strings;
	};

}

#endif
