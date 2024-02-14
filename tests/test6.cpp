#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("release,r", "Enable release mode");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("release")) {
        std::cout << "Release mode enabled" << std::endl;
        // Add code to handle release mode
    } else {
        std::cout << "Release mode not specified" << std::endl;
        // Add code for other modes or default behavior
    }

    return 0;
}