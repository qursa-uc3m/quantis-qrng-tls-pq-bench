# Try to find Dl
#
# Once done, this will define
#   Dl_FOUND - system has Dl
#   Dl_INCLUDE_DIRS - the Dl include directories
#   Dl_LIBRARIES - link these to use Dl

cmake_minimum_required(VERSION 2.6.0)

include("LibFindMacros")

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Dl_PKGCONF Dl)

# Include dir
find_path(Dl_INCLUDE_DIR
  NAMES dlfcn.h
  PATHS ${Dl_PKGCONF_INCLUDE_DIRS}
)

# The library
find_library(Dl_LIBRARY
  NAMES dl
  PATHS ${Dl_PKGCONF_LIBRARY_DIRS}
)

set(Dl_PROCESS_INCLUDES Dl_INCLUDE_DIR)
set(Dl_PROCESS_LIBS Dl_LIBRARY)

libfind_process(Dl)

