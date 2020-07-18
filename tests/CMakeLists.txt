add_library(lib1 SHARED lib1.cpp)
target_compile_features(lib1 PRIVATE cxx_std_17)

add_library(lib2 SHARED lib2.cpp)
target_compile_features(lib2 PRIVATE cxx_std_17)

if(WIN32)
    set_target_properties(lib1 lib2 PROPERTIES PREFIX "")
endif()

add_subdirectory(doctest)

add_executable(reijitests main.cpp symbol.cpp usl.cpp)
target_link_libraries(reijitests doctest)
target_link_libraries(reijitests reiji)
target_compile_features(reijitests PRIVATE cxx_std_17)
add_dependencies(reiji lib1 lib2)

include(doctest/scripts/cmake/doctest.cmake)
doctest_discover_tests(reijitests)
