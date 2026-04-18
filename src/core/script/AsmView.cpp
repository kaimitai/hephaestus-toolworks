#include "AsmView.h"
#include "script_constants.h"
#include <format>

script::AsmView::AsmView(const ht::ScriptConfig& config, const ScriptLayer& layer) {
	const auto& opcodes{ config.opcodes };
	const auto& defines{ config.defines };

	// state and lambda for label generation / retrieval
	std::unordered_map<size_t, std::string> labels;
	std::size_t current_world{ 0 }, current_room{ 0 }, label_counter{ 0 };
	auto get_label = [&](size_t target) -> const std::string& {
		auto it = labels.find(target);
		if (it != labels.end())
			return it->second;

		auto& name = labels[target];
		name = std::format("@label_{}_{}_{}", current_world, current_room, label_counter++);

		return name;
		};

	// collect all labels
	for (std::size_t i{ 0 }; i < layer.instructions.size(); ++i) {
		auto iter{ layer.ptr_table.find(i) };
		if (iter != end(layer.ptr_table)) {
			for (const auto& wr : iter->second) {
				current_world = wr.first;
				current_room = wr.second;
				label_counter = 0;
			}
		}

		const auto& instr{ layer.instructions[i] };
		if (instr.jump_target)
			get_label(*instr.jump_target);
	}


	for (std::size_t i{ 0 }; i < layer.instructions.size(); ++i) {
		// emit entrypoint lines
		auto iter{ layer.ptr_table.find(i) };
		if (iter != end(layer.ptr_table)) {
			for (const auto& wr : iter->second) {
				Line epline{
					.type = LineType::Directive
				};
				Segment segment{
					.text = std::format("{} {}.{}", c::ASM_DIRECTIVE_ENTRYPOINT, wr.first, wr.second),
					.kind = SegmentKind::Entrypoint
				};
				epline.segments.push_back(segment);

				lines.push_back(std::move(epline));
			}
		}
		// emit label lines
		auto lbliter{ labels.find(i) };
		if (lbliter != end(labels)) {
			Line lblline{
				.type = LineType::Label
			};
			Segment segment{
				.text = std::format("{}:", lbliter->second),
				.kind = SegmentKind::Label
			};
			lblline.segments.push_back(segment);
			lines.push_back(std::move(lblline));
		}

		// emit instructions
		const auto& inst{ layer.instructions[i] };
		const auto& opcode{ config.opcodes.at(inst.opcode) };

		Line line{ .type = LineType::Instruction };

		line.segments.push_back(Segment{
			.text = opcode.mnemonic,
			.kind = SegmentKind::Opcode
			});

		if (inst.arg) {
			std::string argstr;

			if (defines.contains(opcode.argDomain)
				&& defines.at(opcode.argDomain).contains(*inst.arg))
				argstr = defines.at(opcode.argDomain).at(*inst.arg);
			else
				argstr = std::format("{}", *inst.arg);

			line.segments.push_back(Segment{
			.text = argstr,
			.kind = SegmentKind::Operand
				});
		}

		if (inst.string_ref) {
			line.segments.push_back(Segment{
				.text = layer.strings.at(inst.string_ref.value()),
				.kind = SegmentKind::String
				});
		}
		else if (inst.string) {
			line.segments.push_back(Segment{
				.text = *inst.string,
				.kind = SegmentKind::String
				});
		}

		if (inst.jump_target) {
			line.segments.push_back(Segment{
			.text = labels.at(*inst.jump_target),
			.kind = SegmentKind::Label
			});
		}

		lines.push_back(line);
	}

}
