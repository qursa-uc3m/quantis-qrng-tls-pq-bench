# ########## Project setup ##########
project(QuantisRNG)

cmake_minimum_required(VERSION 2.6.0)

# Uses 2.6.3 policies
# cmake_policy(VERSION 2.6.3)

# Include project's CMake files
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Display welcome message
message(".==============================================================================.")
message("|           QUANTIS Random Number Generator software by ID Quantique           |")
message("|                                                                              |")
message("|         Copyright (C) 2004-2020 ID Quantique SA. All Rights Reserved.        |")
message("|                                                                              |")
message("|                                                                              |")
message("|  Redistribution and use in source and binary forms, with or without          |")
message("|  modification, are permitted provided that the following conditions          |")
message("|  are met:                                                                    |")
message("|  1. Redistributions of source code must retain the above copyright           |")
message("|     notice, this list of conditions, and the following disclaimer,           |")
message("|     without modification.                                                    |")
message("|  2. Redistributions in binary form must reproduce the above copyright        |")
message("|     notice, this list of conditions and the following disclaimer in the      |")
message("|     documentation and/or other materials provided with the distribution.     |")
message("|  3. The name of the author may not be used to endorse or promote products    |")
message("|     derived from this software without specific prior written permission.    |")
message("|                                                                              |")
message("|  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ''AS IS'' AND      |")
message("|  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE       |")
message("|  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  |")
message("|  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR  |")
message("|  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL      |")
message("|  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS     |")
message("|  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)       |")
message("|  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT  |")
message("|  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY.  |")
message("|                                                                              |")
message("+  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -   +")
message("|                                                                              |")
message("|  Alternatively, this software may be distributed under the terms of the      |")
message("|  GNU General Public License version 2 as published by the Free Software      |")
message("|  Foundation.                                                                 |") 
message("|                                                                              |")
message("|  This program is distributed in the hope that it will be useful, but         |")
message("|  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY  |")
message("|  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License     |")
message("|  for more details.                                                           |")
message("|                                                                              |")
message("|  You should have received a copy of the GNU General Public License           |")
message("|  along with this program; if not, write to the Free Software                 |")
message("|  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA    |")
message("|                                                                              |")
message("+------------------------------------------------------------------------------+")
message("|                                                                              |")
message("| Usage: cmake . [OPTIONS]                                                     |")
message("|                                                                              |")
message("|  OPTIONS:                                                                    |")
message("|                                                                              |")
message("|     -DDISABLE_QUANTIS_PCI=1            Disable Quantis PCI support.          |")
message("|                                                                              |")
message("|     -DDISABLE_QUANTIS_USB=1            Disable Quantis USB support.          |")
message("|                                                                              |")
message("|     -DDISABLE_QUANTIS_JAVA=1           Disable Java support.                 |")
message("|                                                                              |")
message("|     -DENABLE_QUANTIS_COMPAT=1          Build API v1 compatibility libraries. |")
message("|                                                                              |")
message("|     -DDISABLE_EASYQUANTIS=1            Don't build EasyQuantis application.  |")
message("|                                                                              |")
message("|     -DDISABLE_EASYQUANTIS_GUI=1        Only build command-line version of    |")
message("|                                        the EasyQuantis application.          |")
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  message("|                                                                              |")
  message("|     -DUSE_DYNAMIC_LIBS=1               Enable dynamic libs on MacOS X.       |")
  message("|                                        Default value is disabled             |")
  message("|                                                                              |")
  message("|     -DUNIVERSAL=1                      Generate i386, x86_64 and PowerPC     |")
  message("|                                        binaries.                             |")
endif()
message("|                                                                              |")
message("|     -DUSE_CXX11=1                      use the features of C++ 11.           |")
message("|                                        Disabled by default.                  |")
message("|                                                                              |")
message("|     -DFORCE32BIT=1 | -DFORCE64BIT=1    Force 32-bit or 64-bit ABI.           |")
message("|                                                                              |")
message("|     -DCMAKE_BUILD_TYPE=None | Debug | Release | RelWithDebInfo | MinSizeRel  |")
message("|                                                                              |")
message("|     -DCMAKE_INSTALL_PREFIX:PATH=/path  Set install directory prefix to /path.|")
message("|                                        Default value is                      |")
message("|                                           /opt/IDQQuantis on Solaris and      ")
message("|                                           Mac OS                              ")
message("|                                           / on all other systems             |")
message("|                                                                              |")
message("|     -DCMAKE_VERBOSE_MAKEFILE=1         Enable Makefile verbose mode.         |")
message("|                                        Verbose can be useful for debugging   |")
message("|                                        and is disabled by default.           |")
message("|                                                                              |")
message("+  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -   +")
message("|                                                                              |")
message("| NOTE:                                                                        |")
message("|   Libraries and binaries are compiled using the GNU Compiler Collection.     |")
message("|   It is possible to use use an alternative compiler by overloading the       |")
message("|   CC and CXX variables:                                                      |")
message("|      CC=gcc CXX=g++ cmake .            Use GNU Compiler Collection (default).|")
message("|      CC=icc CXX=icpc cmake .           Use Intel C/C++ Compiler.             |")
message("|      CC=cc  CXX=CC cmake .             Use Sun Studio  Compiler.             |")
message("°==============================================================================°")
message("")


# Consistency check
if(DEFINED FORCE32BIT  AND DEFINED FORCE64BIT)  
  message(FATAL_ERROR "-- Can't use both FORCE32BIT and FORCE64BIT!")
endif()

if(FORCE64BIT OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" AND NOT DEFINED FORCE32BIT)
  set(SOFTWARE_ARCHITECTURE "amd64")
  message("-- Software architecture: amd64 (x86-64)")
else()
  set(SOFTWARE_ARCHITECTURE "i386")
  message("-- Software architecture: i386")
endif()


# Turns on optimizations. -O3 enables (among other vectorialization)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") 

# Build
if(NOT CMAKE_BUILD_TYPE)
  message("-- CMAKE_BUILD_TYPE not set")
  message("---- Setting build type to 'Release'")
  set (CMAKE_BUILD_TYPE Release)
endif()


# Installation's directory prefix
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(CMAKE_SYSTEM_NAME MATCHES "SunOS" OR CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CMAKE_INSTALL_PREFIX "/opt/IDQQuantis")
  else()
    set(CMAKE_INSTALL_PREFIX "/")
  endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  #On Mac OS, there is no PCI driver.
  message("On Mac OS X, there is no support for PCI devices. The code for using")
  message("Quantis Pci card will be disabled.")
  set(DISABLE_QUANTIS_PCI ON)

  #To ease the distribution, all the programs and libraries are statically
  #linked with all non system stuff.
  if(NOT USE_DYNAMIC_LIBS)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  endif()
endif()

# Set default installation directories
include(InstallDirs)

# Set custom compilers flags
include (CompilersFlags)


# ########## Configure external libraries ##########

# ---------- Boost libs ----------
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

# ########## Various ##########

# Adds current directory (base source directory) to the include path.
# This way includes like
#    #include "../../MyLibrary/MyHeader.h"
# can be simplified to
#    #include "MyLibrary/MyHeader.h"
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

# Also includes base Quantis Software Package directory
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/..")


# ########## Generates installing packages ##########
include(QuantisPackages)

# ########## Includes sub-directories ##########

# Quantis library
add_subdirectory(Quantis)
add_subdirectory(QuantisExtensions)

# EasyQuantis application
if(NOT DISABLE_EASYQUANTIS)
  if (CMAKE_SYSTEM_NAME MATCHES "SunOS" AND SOFTWARE_ARCHITECTURE MATCHES "amd64")
    message(STATUS "warning: Building 64-bit version of EasyQuantis!")
    message(STATUS "warning: This will almost certainly fail if you did not build 64-bit QT libaries manually!")
  endif() 
  add_subdirectory(EasyQuantis)
else()
  message("-- NOT building EasyQuantis")
endif()
  

