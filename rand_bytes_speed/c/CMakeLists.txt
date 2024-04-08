cmake_minimum_required(VERSION 3.10)

project(RandomBytesSpeed)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -O2")

option(USE_OPENSSL "Use OpenSSL for random bytes" OFF)
if(USE_OPENSSL)
    add_definitions(-DUSE_OPENSSL)
endif()

set(DEVICE "USB" CACHE STRING "Select the device type (USB or PCIE)")

if(DEVICE STREQUAL "USB")
    add_definitions(-DDEVICE_USB)
elseif(DEVICE STREQUAL "PCIE")
    add_definitions(-DDEVICE_PCIE)
else()
    message(FATAL_ERROR "Invalid value for DEVICE. Allowed values are 'USB' and 'PCIE'.")
endif()

# Let the user specify the path to the Quantis library and apps
set(LIB_AND_APPS_PATH "" CACHE PATH "Path to the Quantis libraries and apps")
if("${LIB_AND_APPS_PATH}" STREQUAL "")
    message(FATAL_ERROR "Please provide a value for LIB_AND_APPS_PATH")
endif()

# Set the path to the Quantis include directory and include it
set(QUANTIS_INCLUDE_DIR "${LIB_AND_APPS_PATH}/Quantis")
include_directories(${QUANTIS_INCLUDE_DIR})

# Find the Quantis library
find_library(QUANTIS_LIBRARY NAMES Quantis HINTS "${LIB_AND_APPS_PATH}/build/Quantis")
if(NOT QUANTIS_LIBRARY)
    message(FATAL_ERROR "Quantis library not found")
endif()

# Add the path to the QuantisExtensions library
set(QUANTIS_EXT_INCLUDE_DIR "${LIB_AND_APPS_PATH}/QuantisExtensions")
include_directories(${QUANTIS_EXT_INCLUDE_DIR})

# Find the QuantisExtensions library
find_library(QUANTIS_EXT_LIBRARY NAMES Quantis_Extensions HINTS "${LIB_AND_APPS_PATH}/build/QuantisExtensions")
if(NOT QUANTIS_EXT_LIBRARY)
    message(FATAL_ERROR "Quantis Extensions library not found")
endif()

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Add executable targets
add_executable(rand_bytes_speed_interval rand_bytes_speed_interval.c)
add_executable(rand_bytes_speed_vs_time rand_bytes_speed_vs_time.c)

# Link the Quantis libraries
target_link_libraries(rand_bytes_speed_interval ${QUANTIS_LIBRARY} ${QUANTIS_EXT_LIBRARY} Threads::Threads ${OPENSSL_LIBRARIES})
target_link_libraries(rand_bytes_speed_vs_time ${QUANTIS_LIBRARY} ${QUANTIS_EXT_LIBRARY} Threads::Threads ${OPENSSL_LIBRARIES})

include_directories(${OPENSSL_INCLUDE_DIR})