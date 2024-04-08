project(QuantisExtensions)
cmake_minimum_required(VERSION 2.6.0)

# ########## Define dependencies #############################

include(CheckIncludeFile)
include(CheckIncludeFileCXX)

# Configuration checks
CHECK_INCLUDE_FILE("malloc.h" HAVE_MALLOC_H)

# Include directory containing generated files
include_directories(${PROJECT_BINARY_DIR})

if(NOT DISABLE_QUANTIS_JAVA)
  find_package(JNI REQUIRED)
  include_directories(${JNI_INCLUDE_DIRS})
endif()

# ########## Define sources of this library ##################

set(Quantis_Extensions_SRCS
  QuantisExtractor_C.c
  QuantisExtractor_Cpp.cpp
  #QuantisExtractor_Java.cpp
)

set(Public_Headers
  QuantisExtractor.h
  QuantisExtractor.hpp
)


# ########## Define Shared library ###########################

find_library(Quantis_LIBRARY NAMES Quantis PATHS . /usr/local/lib64 /usr/local/lib /usr/lib)

add_library(Quantis_Extensions SHARED ${Quantis_Extensions_SRCS})
set_target_properties(Quantis_Extensions PROPERTIES
  VERSION ${CPACK_PACKAGE_VERSION}
  SOVERSION ${API_VERSION}
  CLEAN_DIRECT_OUTPUT 1
)
target_link_libraries(Quantis_Extensions # Set dependencies to other libraries
  Quantis
)

# ########## Define Static library ###########################

add_library(Quantis_Extensions-static STATIC ${Quantis_Extensions_SRCS})
set_target_properties(Quantis_Extensions-static PROPERTIES
  OUTPUT_NAME "Quantis_Extensions"
  CLEAN_DIRECT_OUTPUT 1
)
target_link_libraries(Quantis_Extensions-static # Set dependencies to other libraries
  Quantis-static
)

# ########## Define Installation #############################

install(TARGETS
  Quantis_Extensions
  Quantis_Extensions-static
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIB_DIR}
)

install(FILES ${Public_Headers} DESTINATION include)
