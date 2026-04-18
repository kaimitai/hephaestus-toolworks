#ifndef SCRIPT_ASMVIEW_H
#define SCRIPT_ASMVIEW_H

#include <string>
#include <vector>
#include "./../htConfig.h"
#include "ScriptLayer.h"

namespace script {

	class AsmView {

	public:
		enum class LineType { Directive, Instruction, Label };
		enum class SegmentKind { Opcode, Operand, String, Label, Entrypoint };

		AsmView(const ht::ScriptConfig& config, const ScriptLayer& layer);

		struct Segment {
			std::string text;
			SegmentKind kind;
		};

		struct Line {
			LineType type;
			std::vector<Segment> segments;
		};

		std::vector<Line> lines;

	};

}

#endif
