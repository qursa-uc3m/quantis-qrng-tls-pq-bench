project(Quantis)
cmake_minimum_required(VERSION 2.6.0)

# ########## Generates custom header definitions ##########
include(CheckIncludeFile)
include(CheckIncludeFileCXX)

# Configuration checks
CHECK_INCLUDE_FILE("malloc.h" HAVE_MALLOC_H)

# Generate a header file with the specific definitions
configure_file(
  "${PROJECT_SOURCE_DIR}/QuantisLibConfig.h.in"
  "${PROJECT_BINARY_DIR}/QuantisLibConfig.h"
)
# Include directory containing generated files
include_directories(${PROJECT_BINARY_DIR})

# #################### Quantis Libraries ####################

### Quantis library
if(NOT DISABLE_QUANTIS_JAVA)
  find_package(JNI REQUIRED)
  include_directories(${JNI_INCLUDE_DIRS})
endif()


set(QuantisBase_SRCS
  Conversion.c
  Quantis_C.c
  Quantis_Cpp.cpp
  Quantis_Java.cpp
  Quantis_random_device.cpp
)

set(Public_Headers
    Conversion.h
    DllMain.h
    Quantis.h
    QuantisUsb_Commands.h
    Quantis_Compat.h
    Quantis_Internal.h
    Quantis_Java.h
    msc_stdint.h
    resource.h
    Quantis.hpp
    Quantis_random_device.hpp
)

if(UNIX)
  # Sources
  set(Quantis_SRCS
    ${QuantisBase_SRCS}
    QuantisPci_Unix.c
    QuantisUsb_Unix.c
  )

  # Add libusb-1.0 dependency
  if(NOT DISABLE_QUANTIS_USB)
    find_package(USB1 REQUIRED)
    #Rarely in a standard location on Mac OS and perhaps few other OS
    include_directories(${USB1_INCLUDE_DIRS})
    #link_directories(${USB1_LIBRARIES})
  endif()
else()
  message(FATAL_ERROR "-- Quantis library not supported on this system!")
endif()


### Quantis-NoHardware
set(QuantisNoHw_SRCS
  ${QuantisBase_SRCS}
  Quantis_NoHardware.c
)

### Quantis-Compat
set(QuantisCompat_SRCS
  ${QuantisBase_SRCS}
  Quantis_Compat.c
)

########## Quantis Library ##########

# Quantis Shared library
add_library(Quantis SHARED ${Quantis_SRCS})
set_target_properties(Quantis PROPERTIES
  VERSION ${CPACK_PACKAGE_VERSION}
  SOVERSION ${API_VERSION}
  CLEAN_DIRECT_OUTPUT 1

)

# Quantis Static library
add_library(Quantis-static STATIC ${Quantis_SRCS})
set_target_properties(Quantis-static PROPERTIES
  OUTPUT_NAME "Quantis"
  CLEAN_DIRECT_OUTPUT 1
)

# Add libusb-1.0 dependency to Quantis library
if(UNIX AND NOT DISABLE_QUANTIS_USB)
  target_link_libraries(Quantis ${USB1_LIBRARIES})
  target_link_libraries(Quantis-static ${USB1_LIBRARIES})

   if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
   #To link with the system dynamic libraries used by libusb (statically linked on Mac)
   #
   #-install_name name
   #              Sets an internal "install path" (LC_ID_DYLIB) in a dynamic library. Any clients linked against the
   #              library will record that path as the way dyld should locate this library.  If this option is not speci-
   #              fied, then the -o path will be used.  This option is also called -dylib_install_name for compatibility.
   #
   #It seems better to use the CMake property INSTALL_NAME_DIR instead of the direct linker option.
     set_target_properties(Quantis PROPERTIES
         LINK_FLAGS "-framework IOKit -framework CoreFoundation -lSystem"
         INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
     set_target_properties(Quantis-static PROPERTIES
         LINK_FLAGS "-framework IOKit -framework CoreFoundation -lSystem")
   endif()
endif()

# Install libraries
install(TARGETS
  Quantis Quantis-static
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIB_DIR}
)
########## Quantis Hardware-less Library ##########

# Quantis-NoHw Shared library
add_library(Quantis-NoHw SHARED ${QuantisNoHw_SRCS})
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
     set_target_properties(Quantis-NoHw PROPERTIES
         INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif()
set_target_properties(Quantis-NoHw PROPERTIES
  VERSION ${CPACK_PACKAGE_VERSION}
  SOVERSION ${API_VERSION}
  CLEAN_DIRECT_OUTPUT 1
)

# Quantis-NoHw Static library
add_library(Quantis-NoHw-static STATIC ${QuantisNoHw_SRCS})
set_target_properties(Quantis-NoHw-static PROPERTIES
  OUTPUT_NAME "Quantis-NoHw"
  CLEAN_DIRECT_OUTPUT 1
)

# Install libraries and headers
install(TARGETS
  Quantis-NoHw Quantis-NoHw-static
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIB_DIR}
)

INSTALL(FILES ${Public_Headers} DESTINATION include)


########## Quantis Compatilibity Libraries ##########

if(ENABLE_QUANTIS_COMPAT AND NOT DISABLE_QUANTIS_PCI)
  # QuantisPci-Compat Shared library
  add_library(QuantisPci-Compat SHARED ${QuantisCompat_SRCS})
  set_target_properties(QuantisPci-Compat PROPERTIES
    COMPILE_FLAGS "-DQUANTIS_DEVICE_TYPE=1"
    VERSION ${CPACK_PACKAGE_VERSION}
    SOVERSION ${API_VERSION}
    CLEAN_DIRECT_OUTPUT 1
  )

  # QuantisPci-Compat Static library
  add_library(QuantisPci-Compat-static STATIC ${QuantisCompat_SRCS})
  set_target_properties(QuantisPci-Compat-static PROPERTIES
    COMPILE_FLAGS "-DQUANTIS_DEVICE_TYPE=1"
    OUTPUT_NAME "QuantisPci-Compat"
    CLEAN_DIRECT_OUTPUT 1
  )

  # Install libraries
  install(TARGETS
    QuantisPci-Compat QuantisPci-Compat-static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIB_DIR}
  )
endif()


if(ENABLE_QUANTIS_COMPAT AND NOT DISABLE_QUANTIS_USB)
  # QuantisUsb-Compat Shared library
  add_library(QuantisUsb-Compat SHARED ${QuantisCompat_SRCS})
  set_target_properties(QuantisUsb-Compat PROPERTIES
    COMPILE_FLAGS "-DQUANTIS_DEVICE_TYPE=2"
    VERSION ${CPACK_PACKAGE_VERSION}
    SOVERSION ${API_VERSION}
    CLEAN_DIRECT_OUTPUT 1
  )

  # QuantisUsb-Compat Static library
  add_library(QuantisUsb-Compat-static STATIC ${QuantisCompat_SRCS})
  set_target_properties(QuantisUsb-Compat-static PROPERTIES
    COMPILE_FLAGS "-DQUANTIS_DEVICE_TYPE=2"
    OUTPUT_NAME "QuantisUsb-Compat"
    CLEAN_DIRECT_OUTPUT 1
  )

  # Install libraries
  install(TARGETS
    QuantisUsb-Compat QuantisUsb-Compat-static
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIB_DIR}
  )
endif()
