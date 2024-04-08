#
#  Quantis C Software: CMake Compilers definitions
#
#  Copyright (c) 2004-2020 id Quantique SA, Carouge/Geneva, Switzerland
#  All rights reserved.
#
#  ----------------------------------------------------------------------------
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions, and the following disclaimer,
#     without modification.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. The name of the author may not be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY.
#
#  ----------------------------------------------------------------------------
#
#  Alternatively, this software may be distributed under the terms of the
#  GNU General Public License version 2 as published by the Free Software
#  Foundation.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
#  for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  ----------------------------------------------------------------------------
#
#  For history of changes, see ChangeLog.txt
#
#
# Once done, this will define
#   64BITS_FLAGS - Compiler and linker flags to
#   Rt_INCLUDE_DIRS - the librt include directories
#   Rt_LIBRARIES - link these to use librt

# In some cases the function check_flags of CMake seems to be buggy
# (With Sun Studio, "-m64" flag test is always false even if the
# flag exist). The reason for that is an object mismatch (between
# 32 and 64bits) when linking. To be able to test correctly
# the flags of compilers for Solaris (and perhaps other systems)
# a new test should be created.
#
# The macro "MyCheckFlags" accepts 2 parameters
#     1) The flags to test
#     2) The name of a variable which will be set to TRUE or FALSE
#        if the Flags are accepted.

include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)

#------------------------------------------------------------------------------------#
#                                                                                    #
#                            Macros for specific tests                               #
#                                                                                    #
#------------------------------------------------------------------------------------#
macro(MyCheckFlags Flags Result)
  set(${Result} OFF)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeTemp)
  file(WRITE
       ${CMAKE_BINARY_DIR}/CMakeTemp/testCompilerFlags.c
       "int main(int argc, char* argv[]){return 0;} "
       ""
  )

  try_compile(MyCheckFlags_Result
              ${CMAKE_BINARY_DIR}
              ${CMAKE_BINARY_DIR}/CMakeTemp/testCompilerFlags.c
              # When testing for 64bits compilation flags an error occurs at
              # link time due to a mismatch between 32 and 64 bits objects.
              # Thus in case of compile error the detection of the correctness
              # of the flag should be done more carefully.
              COMPILE_DEFINITIONS "${Flags}"
              OUTPUT_VARIABLE MyCheckFlags_OutputCompile
  )

  # Compilation or link might failed or succeed but this is without importance.
  # A careful look inside the generated output should indicate if the option
  # was invalid or if the failure (or success) was caused by other reasons.
  # The search for the strings are mainly related to the Sun Studio compiler
  # and should probably be adapted for some other compilers
  string (REGEX MATCH ".*[Ww]arning:.*${Flags}.*deprecated.*"
          MyCheckFlags_Deprecated
          ${MyCheckFlags_OutputCompile})

  string (REGEX MATCH ".*[Ii]llegal.*.*${Flags}.*"
          MyCheckFlags_Illegal
          ${MyCheckFlags_OutputCompile})

  if (MyCheckFlags_Deprecated STREQUAL "" AND MyCheckFlags_Illegal STREQUAL "")
    set (${Result} ON)
  endif()
endmacro (MyCheckFlags Flags Result)

macro(MyCheckCxx11 Flags Result)
  message("-- Performing Test HAS_CXX_11")
  set(${Result} 0)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeTemp)
  file(WRITE
       ${CMAKE_BINARY_DIR}/CMakeTemp/testCxx11.cpp
       "class foo
       {void fun1() const noexcept{}
       void fun2() = delete;
       static constexpr int fun3() { return 0; }
       };
       int main(){return 0;}"
  )


  try_compile(MyCheckCxx11_Result
  #Check to see if the last version of C++ standard is supported.
              ${CMAKE_BINARY_DIR}
              ${CMAKE_BINARY_DIR}/CMakeTemp/testCxx11.cpp
              COMPILE_DEFINITIONS "${Flags}"
              OUTPUT_VARIABLE MyCheckCxx11_OutputCompile
  )

  if (MyCheckCxx11_Result)
    set (${Result} 1)
    message("-- Performing Test HAS_CXX_11 - Succeed")
  else()
    message("-- Performing Test HAS_CXX_11 - Failed")
  endif()
endmacro (MyCheckCxx11 Flags Result)


#------------------------------------------------------------------------------------#
#                                                                                    #
#                            Generation of Makefiles                                 #
#                                                                                    #
#------------------------------------------------------------------------------------#


# #############################################################################
# ################# Set FLAGS for C++ version 11 features    ##################
# #############################################################################

set(CXX11_SUPPORTED 0)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
   MyCheckCxx11("/Qstd=c++0x" HAS_CXX_11)
   if (DEFINED USE_CXX11 AND ${HAS_CXX_11})
     if(${USE_CXX11})
     # Turn on C++0x / C++11 support
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Qstd=c++0x")
      set(CXX11_SUPPORTED 1)
     endif()
   endif()
else()
  if(NOT CMAKE_SYSTEM_NAME MATCHES "SunOS")
      MyCheckCxx11("-std=c++0x" HAS_CXX_11)
      if (DEFINED USE_CXX11 AND ${HAS_CXX_11})
        if (${USE_CXX11})
        # Turn on C++0x / C++11 support
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
          set(CXX11_SUPPORTED 1)
        endif()
      endif()
  else()
     set(HAS_CXX_11 0)
  endif(NOT CMAKE_SYSTEM_NAME MATCHES "SunOS")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

if (NOT ${HAS_CXX_11} AND DEFINED USE_CXX11)
    if (${USE_CXX11})
      message("C++ version 11 is requested, but your compiler does not support it.")
      message("This feature will be ignored.")
    endif()
endif()

#Pass C++11 support as a preprocessor macro to the Quantis code
if(NOT ${CXX11_SUPPORTED} EQUAL 0)
  add_definitions( -DCXX11_SUPPORTED)
endif()

# #############################################################################
# ################# Set Stack protection                     ##################
# #############################################################################
CHECK_CXX_COMPILER_FLAG("-fstack-protector -fstack-protector-all" HAVE_STACK_PROTECTION)

if(HAVE_STACK_PROTECTION)
  # protects applications from stack-smashing attacks
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector -fstack-protector-all")
endif()

# #############################################################################
# ################# Set FLAGS for GCC/G++ and INTEL Compiler ##################
# #############################################################################
if ("${CMAKE_C_COMPILER}" MATCHES "icc$" AND NOT CMAKE_COMPILER_IS_GNUCC)
  set(COMPILER_IS_INTELCC ON)
endif()

if ("${CMAKE_C_COMPILER}" MATCHES "icpc$" AND NOT CMAKE_COMPILER_IS_GNUCXX)
  set(COMPILER_IS_INTELCXX ON)
endif()


if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR
   COMPILER_IS_INTELCC OR COMPILER_IS_INTELCXX)

  # 32-bit/64-bit ABI
  if(FORCE32BIT)
    set(GNU_COMMON_FLAGS "${GNU_COMMON_FLAGS} -m32")
    set(GNU_COMMON_LINK_FLAGS "${GNU_COMMON_LINK_FLAGS} -m32")
  elseif(FORCE64BIT)
    set(GNU_COMMON_FLAGS "${GNU_COMMON_FLAGS} -m64")
    set(GNU_COMMON_LINK_FLAGS "${GNU_COMMON_LINK_FLAGS} -m64")
  endif()

  # Enables warnings for many common errors
  set(GNU_COMMON_FLAGS "${GNU_COMMON_FLAGS} -Wall")


  # Warns about a selection of common programming errors, such as functions
  # which can return without a value
  set(GNU_COMMON_FLAGS "${GNU_COMMON_FLAGS} -Wextra")

  # Adds threads support
  if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
    set(GNU_COMMON_FLAGS "${GNU_COMMON_FLAGS} -pthreads")
    set(GNU_COMMON_LINK_FLAGS "${GNU_COMMON_LINK_FLAGS} -pthreads")
  else()
    set(GNU_COMMON_FLAGS "${GNU_COMMON_FLAGS} -pthread")
    set(GNU_COMMON_LINK_FLAGS "${GNU_COMMON_LINK_FLAGS} -pthread")
  endif()

  # GCC only flags
  if(CMAKE_COMPILER_IS_GNUCC OR COMPILER_IS_INTELCC)
    # Adds common flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GNU_COMMON_FLAGS}")
    set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} ${GNU_COMMON_LINK_FLAGS}")


    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

    if(CMAKE_COMPILER_IS_GNUCC)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic")
    endif()

    if(COMPILER_IS_INTELCC)
      # Disable Intel C compiler-specific warnings:
      #   * 1418 - external definition with no prior declaration
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -wd1418")
    endif()
  endif(CMAKE_COMPILER_IS_GNUCC OR COMPILER_IS_INTELCC)

  # G++ only flags
  if (CMAKE_COMPILER_IS_GNUCXX OR COMPILER_IS_INTELCXX)
    # Adds common flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GNU_COMMON_FLAGS}")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${GNU_COMMON_LINK_FLAGS}")


    # Warns about implicit type conversions that could cause unexpected results,
    # such as conversions between floating-point and integer types, between
    # signed and unsigned types and between types of different width (e.g. long
    # and short integers)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")

    # Warns about pointers that are cast to remove a type qualifier, such as const
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual")

    # Warns when a derived class declares a function that may be an erroneous
    # attempt to define a virtual function
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")

    # Causes gcc to reject all GNU C extensions, not just those that are
    # incompatible with the ANSI/ISO standard.
    # (do not warn if long long type is used.)
    #
    # "-pedantic" is currently disabled since there are several errors of type
    # "ISO C++ forbids variable-size array name"
    # For help see #http://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/compiling_warnings.html#variable%20len%20arrays
    #
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi -pedantic -Wno-long-long")
    # Should be enabled when when C++98 compatibility is desired
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi")

    if(COMPILER_IS_INTELCXX)
      # Disable Intel C++ compiler-specific warnings:
      #   * 177  - handler parameter was declared but never referenced
      #   * 383  - value copied to temporary, reference to temporary used
      #   * 981  - operands are evaluated in unspecified order
      #   * 1418 - external definition with no prior declarationremark
      #   * 1419 - external declaration in primary source file
      #   * 2259 - non-pointer conversion from "unsigned int" to "float" may lose
      #            significant bits
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd177,383,981,1418,1419,2259")

    endif(COMPILER_IS_INTELCXX)
  endif(CMAKE_COMPILER_IS_GNUCXX OR COMPILER_IS_INTELCXX)
endif()


# #############################################################################
# ##################### Set FLAGS for Sun Studio Compiler #####################
# #############################################################################

# One more check of that the Sun Studio is used
CHECK_C_COMPILER_FLAG("-xO3" HAVE_SUNSTUDIO_OPTIMIZE_PARAM)

# Sun studio name is ".../cc" and ".../CC" thus checking for the two last
# characters plus a possible dir separator should be sufficient to determine
#if the C compiler is used.
if(${CMAKE_C_COMPILER} MATCHES "/?cc$" AND HAVE_SUNSTUDIO_OPTIMIZE_PARAM AND NOT CMAKE_COMPILER_IS_GNUCC)
  set(COMPILER_IS_SUNSTUDIOCC 1)
endif()

if(${CMAKE_CXX_COMPILER} MATCHES "/?CC$" AND HAVE_SUNSTUDIO_OPTIMIZE_PARAM AND NOT CMAKE_COMPILER_IS_GNUCXX)
  set(COMPILER_IS_SUNSTUDIOCXX 1)
endif()

if(COMPILER_IS_SUNSTUDIOCC OR COMPILER_IS_SUNSTUDIOCXX)

  # Optimization flags
  if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -xO0")
  else()
    set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -xO3")
  endif()

  # 32-bit/64-bit ABI
  MyCheckFlags("-m32" HAVE_M32_FLAG)
  MyCheckFlags("-m64" HAVE_M64_FLAG)
  if(FORCE32BIT)
    if(HAVE_M32_FLAG)
      set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -m32")
      set(SUNSTUDIO_COMMON_LINK_FLAGS "${SUNSTUDIO_COMMON_LINK_FLAGS} -m32")
    else()
      # TODO
    endif()
  elseif(FORCE64BIT)
    if(HAVE_M64_FLAG)
      set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -m64")
      set(SUNSTUDIO_COMMON_LINK_FLAGS "${SUNSTUDIO_COMMON_LINK_FLAGS} -m64")
    else()
      if(CMAKE_SYSTEM_PROCESSOR MATCHES "^sparc")
        set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -xarch=v9")
        set(SUNSTUDIO_COMMON_LINK_FLAGS "${SUNSTUDIO_COMMON_LINK_FLAGS} -xarch=v9")
      elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "*amd64*")
        set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -xarch=amd64")
        set(SUNSTUDIO_COMMON_LINK_FLAGS "${SUNSTUDIO_COMMON_LINK_FLAGS} -xarch=amd64")
      endif()
    endif()
  endif()

  # Adds threads support
  set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -mt")
  set(SUNSTUDIO_COMMON_LINK_FLAGS "${SUNSTUDIO_COMMON_LINK_FLAGS} -mt")

  # Removes SunStudio specific warnings
  #  - E_WHITE_SPACE_IN_DIRECTIVE: White space in a directive is generally
  #                                caused by a DOS enline
  #  - E_EMPTY_TRANSLATION_UNIT:   Empty compilation Unit is produced by the
  #                                removal of some parts (like USB, PCI or JAVA)
  #
  #  -errtags=yes displays the tag of the warning. This tag can be added to
  #  -erroff switch to suppress the warning.
  set(WARNS_TO_IGNORE "E_ATTRIBUTE_UNKNOWN,E_WHITE_SPACE_IN_DIRECTIVE,E_EMPTY_TRANSLATION_UNIT")
  #set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -errtags=yes -erroff=${WARNS_TO_IGNORE}")
  set(SUNSTUDIO_COMMON_FLAGS "${SUNSTUDIO_COMMON_FLAGS} -errtags=yes")

  if(COMPILER_IS_SUNSTUDIOCC)
    # Adds common flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SUNSTUDIO_COMMON_FLAGS}")
    set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} ${SUNSTUDIO_COMMON_LINK_FLAGS}")

    # Turn on recognition of supported C99 language features and enable the
    # 1999 C standard library semantics of routines.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SUNSTUDIO_COMMON_FLAGS} -xc99")
  endif()

  if(COMPILER_IS_SUNSTUDIOCXX)
    # Adds common flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SUNSTUDIO_COMMON_FLAGS}")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${SUNSTUDIO_COMMON_LINK_FLAGS}")
    endif()
endif()


# Manually adds linker flags to EXE, SHARED and MODULE on SunOS
if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_C_LINK_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_C_LINK_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${CMAKE_C_LINK_FLAGS}")
endif()

#
#Specific flags for compiling under Mac OS. Mainly to produce PowerPC and i386
#binaries when the "UNIVERSAL" CMake variable has been defined.
#
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if (DEFINED UNIVERSAL)
    set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -arch i386 -arch ppc -arch x86_64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch i386 -arch ppc -arch x86_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch i386 -arch ppc -arch x86_64")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -arch i386 -arch ppc -arch x86_64")
  endif()
  #The framework Cocoa is not found when the EasyQuantis program is compiled statically
  #with QT libs. The link with this framework should be added manually.
  if (NOT DEFINED DISABLE_EASYQUANTIS_GUI AND DEFINED CMAKE_FIND_LIBRARY_SUFFIXES)
    if (${CMAKE_FIND_LIBRARY_SUFFIXES} STREQUAL ".a")
       set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -framework Cocoa")
       set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -framework Cocoa")
    endif()
  endif()
endif()


# Display Flags
message("-- C Flags: ${CMAKE_C_FLAGS}")
message("-- C Linker Flags: ${CMAKE_C_LINK_FLAGS}")
message("-- CXX Flags: ${CMAKE_CXX_FLAGS}")
message("-- CXX Linker Flags: ${CMAKE_CXX_LINK_FLAGS}")
