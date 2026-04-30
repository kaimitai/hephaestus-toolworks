#include <iostream>
#include "./cli/Cli.h"

int main(int argc, char** argv) try {
	forge::Cli cli(argc, argv);
}
catch (const std::exception& ex) {
	std::cout << ex.what() << "\n";
	return 1;
}
