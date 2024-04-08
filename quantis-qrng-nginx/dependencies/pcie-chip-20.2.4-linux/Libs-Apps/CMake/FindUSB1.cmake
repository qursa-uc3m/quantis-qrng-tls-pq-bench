# Try to find libusb
#
# Once done, this will define
#   USB1_FOUND - system has libusb-1
#   USB1_INCLUDE_DIRS - the libusb-1 include directories
#   USB1_LIBRARIES - link these to use libusb-1

cmake_minimum_required(VERSION 2.6.0)

include("LibFindMacros")

# Dependencies
if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
  libfind_package(USB1 Rt)
#else()
#  message ("MacOS")
endif()

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(USB1_PKGCONF USB1)

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  # Include dir
  find_path(USB1_INCLUDE_DIR
    NAMES libusb.h
    PATHS ${USB1_PKGCONF_INCLUDE_DIRS}
  )

  # The library
  find_library(USB1_LIBRARY
    NAMES usb
    PATHS ${USB1_PKGCONF_LIBRARY_DIRS}
  )
else()
  # Include dir
  find_path(USB1_INCLUDE_DIR
    NAMES libusb-1.0/libusb.h
    PATHS ${USB1_PKGCONF_INCLUDE_DIRS}
  )

  # The library
  find_library(USB1_LIBRARY
    NAMES usb-1.0
    PATHS ${USB1_PKGCONF_LIBRARY_DIRS}
  )
endif()

set(USB1_PROCESS_INCLUDES USB1_INCLUDE_DIR)
set(USB1_PROCESS_LIBS USB1_LIBRARY)

if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(USB1_PROCESS_INCLUDES ${USB1_PROCESS_INCLUDES} Rt_INCLUDE_DIRS)
  set(USB1_PROCESS_LIBS ${USB1_PROCESS_LIBS} Rt_LIBRARIES)
endif()


libfind_process(USB1)
