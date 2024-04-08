/*
 * Quantis compatibility layer
 * 
 * This header is meant to be used by user who do not want to update
 * their application to the new Quantis API.
 *
 * Copyright (C) 2004-2020 ID Quantique SA, Carouge/Geneva, Switzerland
 * All rights reserved.
 *
 * ----------------------------------------------------------------------------
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY.
 *
 * ----------------------------------------------------------------------------
 *
 * Alternatively, this software may be distributed under the terms of the
 * terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#ifndef QUANTIS_COMPATIBILITY_H
#define QUANTIS_COMPATIBILITY_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#include "DllMain.h"

/* Display warning */
#ifdef _MSC_VER
#pragma message("You are using the old Quantis API. Please update as soon as possible to the new API.")
#pragma message("Support for old API will be dropped in the future.")
#else
#warning "You are using the old Quantis API. Please update as soon as possible to the new API."
#warning "Support for old API will be dropped in the future."
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * Resets the Quantis board.
   * @param deviceNumber the number of the Quantis device.
   * @return 0 on success, otherwise the error number.
   * @deprecated use QuantisBoardReset instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisBoardReset(unsigned int deviceNumber)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Count the number of Quantis devices.
   * @return the number of Quantis devices found.
   * @deprecated use QuantisCount instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisCount()
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Get the version of the board.
   * @param deviceNumber the number of the Quantis device.
   * @return The version of the board or a negative value on error.
   * @deprecated use QuantisGetBoardVersion instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisBoardVersion(unsigned int deviceNumber)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Get the version of the driver.
   * @return The version of the driver or a negative value on error.
   * @deprecated use QuantisGetDriverVersion instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisDriverVersion()
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Get the version of the library.
   * @return The version of the library or a negative value on error.
   * @deprecated use QuantisGetLibVersion instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisLibVersion()
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Returns the data rate (in Kbits/s) provided by the Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @return the data rate provided by the Quantis device or a negative 
   * value on error.
   * @deprecated use QuantisGetModulesDataRate instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisModuleDataRate(unsigned int deviceNumber)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Returns a bitmask of the modules that have been detected on a Quantis
   * device, where bit <em>n</em> is set if module <em>n</em> is present.
   * For instance when 5 (1101 in binary) is returned, it means that modules
   * 0, 2 and 3 have been detected.
   * @param deviceNumber the number of the Quantis device.
   * @return a bitmask of the detected modules or a negative value when an error
   * occured.
   * @deprecated use QuantisGetModulesMask instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisGetModules(unsigned int deviceNumber)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Get the power status of the modules.
   * @param deviceNumber the number of the Quantis device.
   * @return 1 if the modules are powered, 0 if the modules are not powered and
   * a negative value when an error occured.
   * @deprecated use QuantisGetModulesPower instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisModulesPower(unsigned int deviceNumber)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Get status of the modules.
   * @param deviceNumber the number of the Quantis device.
   * @return the status of the modules or a negative value on error.
   * @deprecated use QuantisGetModulesStatus instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisModulesStatus(unsigned int deviceNumber)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Get the device's serial number.
   * @param deviceNumber the number of the Quantis device.
   * @return the device's serial number.
   * @deprecated use QuantisGetSerialNumber instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      char *quantisGetSerialNumber(unsigned int deviceNumber)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Disable modules.
   * @param deviceNumber the number of the Quantis device.
   * @param moduleMask a bitmask of the modules that must be disabled.
   * @return 0 on success or a negative value on error.
   * @deprecated use QuantisModulesDisable instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisModulesDisable(unsigned int deviceNumber, int moduleMask)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Enable modules.
   * @param deviceNumber the number of the Quantis device.
   * @param moduleMask a bitmask of the modules that must be disabled.
   * @return 0 on success or a negative value on error.
   * @deprecated use QuantisModulesEnable instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisModulesEnable(unsigned int deviceNumber, int moduleMask)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Reset modules.
   * @param deviceNumber the number of the Quantis device.
   * @param moduleMask a bitmask of the modules that must be disabled.
   * @return 0 on success or a negative value on error.
   * @deprecated use QuantisModulesReset instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisModulesReset(unsigned int deviceNumber, int moduleMask)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

  /**
   * Read random data from a device
   * @param deviceNumber the number of the Quantis device.
   * @param buffer the address of the destination data.
   * @param size the size to read.
   * @return the read size or a negative value on error.
   * @deprecated use QuantisRead instead.
   */
  DLL_EXPORT
#ifdef _WIN32
  __declspec(deprecated)
#endif
      int quantisRead(unsigned int deviceNumber, void *buffer, size_t size)
#ifndef _WIN32
          __attribute__((deprecated))
#endif
          ;

#ifdef __cplusplus
}
#endif

#endif // QUANTIS_COMPATIBILITY_H
