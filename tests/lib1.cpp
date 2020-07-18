#include "export.hpp"
#include <string>

extern "C" {

extern EXPORT int bar;
int bar = 5;

EXPORT int increase_bar_and_return_it() {
    return ++bar;
}
}
