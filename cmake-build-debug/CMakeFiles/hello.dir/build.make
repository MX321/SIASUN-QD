# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /home/siasunhebo/clion-2019.1.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/siasunhebo/clion-2019.1.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/siasunhebo/workspace/IMUtest

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/siasunhebo/workspace/IMUtest/cmake-build-debug

# Utility rule file for hello.

# Include the progress variables for this target.
include CMakeFiles/hello.dir/progress.make

CMakeFiles/hello:
	cd /home/siasunhebo/workspace/IMUtest && make

hello: CMakeFiles/hello
hello: CMakeFiles/hello.dir/build.make

.PHONY : hello

# Rule to build all files generated by this target.
CMakeFiles/hello.dir/build: hello

.PHONY : CMakeFiles/hello.dir/build

CMakeFiles/hello.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hello.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hello.dir/clean

CMakeFiles/hello.dir/depend:
	cd /home/siasunhebo/workspace/IMUtest/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/siasunhebo/workspace/IMUtest /home/siasunhebo/workspace/IMUtest /home/siasunhebo/workspace/IMUtest/cmake-build-debug /home/siasunhebo/workspace/IMUtest/cmake-build-debug /home/siasunhebo/workspace/IMUtest/cmake-build-debug/CMakeFiles/hello.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/hello.dir/depend

