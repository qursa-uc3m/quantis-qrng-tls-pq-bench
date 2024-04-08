# Try to find librt
#
# Once done, this will define
#   Rt_FOUND - system has librt
#   Rt_INCLUDE_DIRS - the librt include directories
#   Rt_LIBRARIES - link these to use librt

cmake_minimum_required(VERSION 2.6.0)

include("LibFindMacros")

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(Rt_PKGCONF Rt)

# Include dir
set(Rt_INCLUDE_DIR "/usr/include")
# find_path(Rt_INCLUDE_DIR
#   NAMES rt.h
#   PATHS ${Rt_PKGCONF_INCLUDE_DIRS}
# )

# The library
find_library(Rt_LIBRARY
  NAMES rt
  PATHS ${Rt_PKGCONF_LIBRARY_DIRS}
)

set(Rt_PROCESS_INCLUDES Rt_INCLUDE_DIR)
set(Rt_PROCESS_LIBS Rt_LIBRARY)

libfind_process(Rt)
