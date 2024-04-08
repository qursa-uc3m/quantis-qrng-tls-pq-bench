#!/bin/sh

# Directories
FindRm()
{
  find . -name "$1" -print -exec rm -Rf '{}' \;
}

make clean

# Dirs
rm -Rf "./bin/"
FindRm "CMakeFiles"
FindRm "_CPack_Packages"

# Files
FindRm "CMakeCache.txt"
FindRm "cmake_install.cmake"
FindRm "CPackConfig.cmake"
FindRm "CPackSourceConfig.cmake"
FindRm "Makefile"
FindRm "*~"
