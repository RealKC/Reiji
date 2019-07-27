cmake_minimum_required(VERSION 3.5)
project(reiji VERSION 0.1.0 LANGUAGES CXX)

add_library(reiji src/unique_shared_lib.cpp)

target_include_directories(reiji
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Reiji)

install(TARGETS reiji
    EXPORT reiji-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set_target_properties(reiji PROPERTIES EXPORT_NAME Reiji)

install(DIRECTORY /include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT reiji-targets
    FILE ReijiTargets.cmake
    NAMESPACE Reiji::
    DESTINATION ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ReijiConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/ReijiConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ReijiConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ReijiConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ReijiConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT reiji-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/ReijiTargets.cmake NAMESPACE Reiji::)

export(PACKAGE Reiji)
