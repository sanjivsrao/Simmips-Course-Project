cmake_minimum_required(VERSION 3.5...3.22)
project(SIMMIPS CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# You will need to edit these variables as the project progresses
#-----------------------------------------------------------------------

# add source for library modules here
set(LIB_SOURCE
  Token.hpp
  Token.cpp
  Lexer.hpp
  Lexer.cpp
  Parser.hpp
  Parser.cpp
  VirtualMachineInterface.hpp
  VirtualMachine.hpp
  VirtualMachine.cpp
)

# add source for library unit tests here
set(LIB_TEST_SOURCE
  UnitTests.cpp
  TokenTests.cpp
  LexerTests.cpp
  ParserTests.cpp
  VirtualMachineTests.cpp
 )

# add source for tui modules here
set(TUI_SOURCE
  REPL.hpp
  REPL.cpp
  )

# add source for tui functional tests here
set(TUI_TEST_SOURCE
  TUITests.cpp
  )

# add source for gui modules here
set(GUI_SOURCE
  VirtualMachineGUI.hpp
  VirtualMachineGUI.cpp
  )

# add source for gui functional tests here
set(GUI_TEST_SOURCE
  GUITests.cpp
  )

# add source for simmips program here
set(SIMMIPS_SOURCE
  simmips.cpp
  )

# add source for simmips-ide program here
set(SIMMIPS_IDE_SOURCE
  simmips-ide.cpp
  )

# You should not need to edit below this line
#-----------------------------------------------------------------------
#-----------------------------------------------------------------------

# try to prevent accidental in-source builds, these cause lots of problems
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds not allowed. Remove any files created thus far and use a different directory for the build.")
endif()

# require a C++11 compiler for all targets
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# optional strict mode in reference environment
if(UNIX AND CMAKE_COMPILER_IS_GNUCXX AND STRICT)
  message("-- Enabling strict compilation mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()

# setup testing config file
set(TEST_FILE_DIR "${CMAKE_SOURCE_DIR}/tests")
configure_file(${CMAKE_SOURCE_DIR}/test_config.hpp.in 
  ${CMAKE_BINARY_DIR}/test_config.hpp)
configure_file(${CMAKE_SOURCE_DIR}/test_config.py.in 
  ${CMAKE_BINARY_DIR}/test_config.py)
include_directories(${CMAKE_BINARY_DIR})

# setup catch
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Catch)

# configure Qt
find_package(Qt5 COMPONENTS Core Widgets Test QUIET)
if(Qt5_FOUND)
  message("-- Enabling Qt")
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
else(Qt5_FOUND)
  message("-- Warning: Qt not found.")
endif()

add_library(mips ${LIB_SOURCE})

# executables without Qt dependency
add_executable(unit_tests catch.hpp ${LIB_TEST_SOURCE} ${LIB_SOURCE})
if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(unit_tests pthread)
endif()

add_executable(tui_test ${TUI_TEST_SOURCE} ${TUI_SOURCE} ${LIB_SOURCE})
if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(tui_test pthread)
endif()

add_executable(simmips ${SIMMIPS_SOURCE} ${TUI_SOURCE})
if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(simmips mips pthread)
else(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(simmips mips)
endif()

# executables with Qt dependency
if(Qt5_FOUND)

  add_executable(gui_test ${GUI_TEST_SOURCE} ${GUI_SOURCE})
  if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(gui_test mips Qt5::Widgets Qt5::Test pthread)
  else(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(gui_test mips Qt5::Widgets Qt5::Test)
  endif()
  
  add_executable(simmips-ide ${SIMMIPS_IDE_SOURCE} ${GUI_SOURCE})
  if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(simmips-ide mips Qt5::Widgets pthread)
  else(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(simmips-ide mips Qt5::Widgets)
  endif()
  
endif() # end Qt dependency

# Testing
include(CTest)

# register Catch tests with cmake
catch_discover_tests(unit_tests)

# In the reference environment enable coverage on tests and threading
if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX AND COVERAGE)
  message("-- Enabling simmips library test coverage")
  set(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
  set_target_properties(unit_tests PROPERTIES COMPILE_FLAGS ${GCC_COVERAGE_COMPILE_FLAGS} )
  target_link_libraries(unit_tests gcov)
  add_custom_target(coverage
    COMMAND ${CMAKE_COMMAND} -E env "ROOT=${CMAKE_CURRENT_SOURCE_DIR}"
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/coverage.sh)
endif()

# In the reference environment enable memory checking on tests
if(UNIX AND NOT APPLE AND CMAKE_COMPILER_IS_GNUCXX AND MEMTEST)
  message("-- Enabling simmips library memory checks")
  add_custom_target(memtest
    COMMAND valgrind ${CMAKE_BINARY_DIR}/unit_tests)
endif()
