#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <filesystem>
#include <string>

std::string path_to_my_directory;

namespace fs = std::filesystem;

int main(int argc, char** argv) {
    doctest::Context ctx;
    ctx.applyCommandLine(argc, argv);
    return ctx.run();
}
