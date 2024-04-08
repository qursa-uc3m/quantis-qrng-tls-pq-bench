# This script defines some variables that describe paths needed 
# to install an application:
#   * CMAKE_INSTALL_BIN_DIR
#   * CMAKE_INSTALL_LIB_DIR
#
# Requires SOFTWARE_ARCHITECTURE

# Documentation string
set(CMAKE_INSTALL_BIN_DIR_DOC  "user executables [PREFIX/bin]")
set(CMAKE_INSTALL_LIB_DIR_DOC  "object code libraries [PREFIX/lib]")

# General case
set(CMAKE_INSTALL_BIN_DIR "bin" CACHE PATH ${CMAKE_INSTALL_BIN_DIR_DOC})

if(SOFTWARE_ARCHITECTURE MATCHES "i386")
  if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
    set(CMAKE_INSTALL_LIB_DIR "lib/32" CACHE PATH ${CMAKE_INSTALL_LIB_DIR_DOC})
  else()
    set(CMAKE_INSTALL_LIB_DIR "lib" CACHE PATH ${CMAKE_INSTALL_LIB_DIR_DOC})
  endif()
elseif(SOFTWARE_ARCHITECTURE MATCHES "amd64")
  if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
    set(CMAKE_INSTALL_LIB_DIR "lib/64" CACHE PATH ${CMAKE_INSTALL_LIB_DIR_DOC})
  else()
    set(CMAKE_INSTALL_LIB_DIR "lib64" CACHE PATH ${CMAKE_INSTALL_LIB_DIR_DOC})
  endif()
else()
  message(FATAL_ERROR "Unknown SOFTWARE_ARCHITECTURE")
endif()


mark_as_advanced(
  CMAKE_INSTALL_BIN_DIR
  CMAKE_INSTALL_LIB_DIR
)




