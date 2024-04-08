#
#  Quantis C Software: Packageis building configuration file
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
# File:          PackagesDef.cmake
# Last Modified: 8 May 2010
# Descripion:    Configuation file of the tool "CMake" for the
#                definitions used to build the software packages
#                for the different OS.

# http://www.cmake.org/Wiki/CMake:Packaging_With_CPack
# http://www.cmake.org/Wiki/CMake:CPackConfiguration
# http://www.cmake.org/Wiki/CMake:CPackPackageGenerators

include(InstallRequiredSystemLibraries)

#When the variable DESTDIR is used. One can then type the command
#"make DESTDIR=/tmp install" to install all the files with full arborescence
#under the directory "/tmp". This is useful to make the package (specialy
#to manage absolute file names)
set(CPACK_SET_DESTDIR ON)


set(CPACK_PACKAGE_NAME "QuantisRNG")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Quantis Random Number Generator library with EasyQuantis application")
set(CPACK_PACKAGE_VENDOR "IDQ")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
set(API_VERSION "20") # API and ABI version

# Quantis library version. Also modify Quantis/Quantis_Internal.h and QuantisExtensions/QuantisExtractor.h!
set(CPACK_PACKAGE_VERSION "20.2.4")
set(CPACK_PACKAGE_VERSION_MAJOR "20")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${SOFTWARE_ARCHITECTURE}")
set(CPACK_PACKAGE_CONTACT "support@idquantique.com")
#The package will be installed at the install position (defined by CMAKE_INSTALL_PREFIX) inside this dir.
set(CPACK_PACKAGE_INSTALL_DIRECTORY "/")

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/../License.txt")

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/Readme_package_macos.html")
endif()

#if(WIN32 AND NOT UNIX)
#  # There is a bug in NSI that does not handle full unix paths properly. Make
#  # sure there is at least one set of four (4) backlasshes.
#  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
#  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
#  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
#  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
#  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
#  SET(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
#  SET(CPACK_NSIS_MODIFY_PATH ON)
#else(WIN32 AND NOT UNIX)
#  SET(CPACK_STRIP_FILES "bin/MyExecutable")
#  SET(CPACK_SOURCE_STRIP_FILES "")
#endif(WIN32 AND NOT UNIX)

# ---------- DEB-Specific ----------
set(CPACK_DEBIAN_SOFTWARE_ARCHITECTURE ${SOFTWARE_ARCHITECTURE})
#set(CPACK_DEBIAN_PACKAGE_MAINTAINER "") # CPACK_PACKAGE_CONTACT
#set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "") # use CPACK_PACKAGE_DESCRIPTION_SUMMARY
set(CPACK_DEBIAN_PACKAGE_SECTION "devel" )
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtcore4 (>= 4.1), libqtgui4 (>= 4.1), libusb-1.0-0 (>=  2:1.0)")


# ---------- RPM-Specific ----------
set(CPACK_RPM_SOFTWARE_ARCHITECTURE ${SOFTWARE_ARCHITECTURE})
set(CPACK_RPM_PACKAGE_DEPENDS "qt (>= 4.1), libusb1 (>= 1.0")
#set(CPACK_RPM_PACKAGE_DEPENDS "libqtcore4 (>= 4.1), libqtgui4 (>= 4.1), libusb1 (>= 1.0")

# ---------- Build packages ----------
#set(CPACK_GENERATOR "TBZ2;DEB;RPM")
set(CPACK_GENERATOR "TBZ2" "PackageMaker")



include(CPack)


