#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <string>
#ifdef __linux__
#    include <cstdlib>
#    include <filesystem>
#    include <iostream>
#endif

int main(int argc, char** argv) {
#ifdef __linux__
    auto* ld_library_path = std::getenv("LD_LIBRARY_PATH");
    std::cout << "\n\n[reijitests] LD_LIBRARY_PATH="
              << (ld_library_path ? ld_library_path : "(doesn't exist)")
              << "\nCWD: " << std::filesystem::current_path() << "\n\n";
#endif

    doctest::Context ctx;
    ctx.applyCommandLine(argc, argv);
    return ctx.run();
}
