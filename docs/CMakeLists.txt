find_package(Doxygen)
find_package(Sphinx)

if(NOT DOXYGEN_FOUND)
    message(NOTICE "Documentation generation not available due to Doxygen not being installed.")
    return()
endif()

if (NOT Sphinx_FOUND)
    message(NOTICE "Documentation generation not available due to Sphinx not being installed.")
    return()
endif()

###############################################################################
#                        Doxygen Configuration                                #
###############################################################################

get_target_property(REIJI_HEADER_DIR reiji INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE REIJI_HEADERS ${REIJI_HEADER_DIR}/*.hpp)

set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/include/reiji)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/xml/index.xml)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# Replace variables inside @@ with the current values
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

# Doxygen won't create this for us
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}/xml)

add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                  DEPENDS ${REIJI_HEADERS}
                  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                  MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                  COMMENT "Generating docs"
                  VERBATIM)

add_custom_target(Doxygen DEPENDS ${DOXYGEN_INDEX_FILE})

###############################################################################
#                         Sphinx Configuration                                #
###############################################################################

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/sphinx)
set(SPHINX_INDEX_FILE ${SPHINX_BUILD}/index.html)

set(REIJI_RST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
    ${CMAKE_CURRENT_SOURCE_DIR}/symbol.rst
    ${CMAKE_CURRENT_SOURCE_DIR}/macros.rst)
message("Doxygen outputs to: ")
message(${DOXYGEN_OUTPUT_DIR})

add_custom_command(OUTPUT ${SPHINX_INDEX_FILE}
                   COMMAND
                       ${SPHINX_EXECUTABLE} -b html
                       "-Dbreathe_projects.Reiji=${DOXYGEN_OUTPUT_DIR}xml"
                   ${SPHINX_SOURCE} ${SPHINX_BUILD}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                   DEPENDS
                   ${REIJI_RST_SOURCES}
                   ${DOXYGEN_INDEX_FILE}
                   MAIN_DEPENDENCY ${SPHINX_SOURCE}/conf.py
                   COMMENT "Generating documentation with Sphinx")

add_custom_target(documentation DEPENDS ${SPHINX_INDEX_FILE})
