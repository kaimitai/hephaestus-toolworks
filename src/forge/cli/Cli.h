#ifndef FORGE_CLI_H
#define FORGE_CLI_H

#include <string>

namespace forge {

	class Cli {

		void output_oe_on_windows(void) const;
		std::string get_final_out_nes_file(const std::string& p_out_file) const;

	public:
		Cli(int argc, char** argv);
	};

}

#endif
