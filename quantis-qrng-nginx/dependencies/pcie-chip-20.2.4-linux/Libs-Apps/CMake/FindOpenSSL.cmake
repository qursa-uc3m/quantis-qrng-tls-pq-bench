################################################################################
#
# Try to find OpenSSL
#
# Once done, this will define
#   OpenSSL_FOUND - system has OpenSSL
#   OpenSSL_INCLUDE_DIRS - the OpenSSL include directories
#   OpenSSL_LIBRARIES - link these to use OpenSSL
#
################################################################################

cmake_minimum_required(VERSION 2.6.0)

include("LibFindMacros")

# Dependencies
libfind_package(OpenSSL Z)
libfind_package(OpenSSL Dl)

# Use pkg-config to get hints about paths
libfind_pkg_check_modules(OpenSSL_PKGCONF OpenSSL)

# Include dir
find_path(OpenSSL_INCLUDE_DIR
  NAMES openssl/ssl.h
  PATHS ${OpenSSL_PKGCONF_INCLUDE_DIRS}
)

# The libraries
find_library(OpenSSL_LIBSSL_LIBRARY
  NAMES ssl
  PATHS ${OpenSSL_PKGCONF_LIBRARY_DIRS}
)

find_library(OpenSSL_LIBCRYPTO_LIBRARY
  NAMES crypto
  PATHS ${OpenSSL_PKGCONF_LIBRARY_DIRS}
)

set(OpenSSL_PROCESS_INCLUDES OpenSSL_INCLUDE_DIR)
set(OpenSSL_PROCESS_LIBS OpenSSL_LIBSSL_LIBRARY OpenSSL_LIBCRYPTO_LIBRARY Z_LIBRARIES Dl_LIBRARIES)
libfind_process(OpenSSL)

