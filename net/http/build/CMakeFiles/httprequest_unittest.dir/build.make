# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/lvy/code/net/http

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/lvy/code/net/http/build

# Include any dependencies generated for this target.
include CMakeFiles/httprequest_unittest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/httprequest_unittest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/httprequest_unittest.dir/flags.make

CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o: CMakeFiles/httprequest_unittest.dir/flags.make
CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o: ../tests/HttpRequest_test.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/lvy/code/net/http/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o"
	/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o -c /home/lvy/code/net/http/tests/HttpRequest_test.cpp

CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.i"
	/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/lvy/code/net/http/tests/HttpRequest_test.cpp > CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.i

CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.s"
	/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/lvy/code/net/http/tests/HttpRequest_test.cpp -o CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.s

CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o.requires:
.PHONY : CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o.requires

CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o.provides: CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o.requires
	$(MAKE) -f CMakeFiles/httprequest_unittest.dir/build.make CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o.provides.build
.PHONY : CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o.provides

CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o.provides.build: CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o

# Object files for target httprequest_unittest
httprequest_unittest_OBJECTS = \
"CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o"

# External object files for target httprequest_unittest
httprequest_unittest_EXTERNAL_OBJECTS =

httprequest_unittest: CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o
httprequest_unittest: CMakeFiles/httprequest_unittest.dir/build.make
httprequest_unittest: libhttp.a
httprequest_unittest: CMakeFiles/httprequest_unittest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable httprequest_unittest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/httprequest_unittest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/httprequest_unittest.dir/build: httprequest_unittest
.PHONY : CMakeFiles/httprequest_unittest.dir/build

CMakeFiles/httprequest_unittest.dir/requires: CMakeFiles/httprequest_unittest.dir/tests/HttpRequest_test.cpp.o.requires
.PHONY : CMakeFiles/httprequest_unittest.dir/requires

CMakeFiles/httprequest_unittest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/httprequest_unittest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/httprequest_unittest.dir/clean

CMakeFiles/httprequest_unittest.dir/depend:
	cd /home/lvy/code/net/http/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/lvy/code/net/http /home/lvy/code/net/http /home/lvy/code/net/http/build /home/lvy/code/net/http/build /home/lvy/code/net/http/build/CMakeFiles/httprequest_unittest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/httprequest_unittest.dir/depend

