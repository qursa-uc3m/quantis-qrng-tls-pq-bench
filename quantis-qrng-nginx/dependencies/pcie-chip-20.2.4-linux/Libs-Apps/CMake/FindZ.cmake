# Try to find libz
#
# Once done, this will define
#   Z_FOUND - system has Dl
#   Z_INCLUDE_DIRS - the Dl include directories
#   Z_LIBRARIES - link these to use Dl

cmake_minimum_required(VERSION 2.6.0)

include("LibFindMacros")

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Z_PKGCONF Z)

# Include dir
# find_path(Z_INCLUDE_DIR
#   NAMES dlfcn.h
#   PATHS ${Dl_PKGCONF_INCLUDE_DIRS}
# )

# The library
find_library(Z_LIBRARY
  NAMES z
  PATHS ${Z_PKGCONF_LIBRARY_DIRS}
)

# set(Dl_PROCESS_INCLUDES Dl_INCLUDE_DIR)
set(Z_PROCESS_LIBS Z_LIBRARY)

libfind_process(Z)

