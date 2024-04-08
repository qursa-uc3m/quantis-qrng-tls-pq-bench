/*
 * Quantis C++ Library
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
 * GNU General Public License version 2 as published by the Free Software 
 * Foundation.
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
 * ----------------------------------------------------------------------------
 *
 * For history of changes, see ChangeLog.txt
 */

#ifndef QUANTIS_HPP
#define QUANTIS_HPP

#include <string>
#include <stdexcept>

#include "Quantis.h"

#ifdef _WIN32
// Visual C++ does not implement checked exceptions
// Disables "C++ exception specification ignored except to indicate a function is not __declspec(nothrow)"
#pragma warning(disable : 4290)
#endif

namespace idQ
{
class DLL_EXPORT Quantis
{
public:
    /**
       * Constuctor: Open the device
       * @param deviceType specify the type of Quantis device.
       * @param deviceNumber the number of the Quantis device.
       * @throw runtime_error QUANTIS_ERROR code on failure
       */
    Quantis(QuantisDeviceType deviceType, unsigned int deviceNumber) throw(std::runtime_error);

    /**
       * Destructor: Close the device
       */
    ~Quantis();

    /**
      * Resets the Quantis board.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      * @warning This function do not generally has to be called, since the board
      * is automatically reset.
      */
    void BoardReset() const
        throw(std::runtime_error);

    /**
      * Returns the number of specific Quantis type devices that have been detected
      * on the system.
      * @param deviceType specify the type of Quantis device.
      * @return the number of Quantis devices that have been detected on the system.
      * Returns 0 on error or when no card is installed.
      */
    static int Count(QuantisDeviceType deviceType) throw(std::runtime_error);

    /**
      * Get the version of the board.
      * @return the version of the board
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    int GetBoardVersion() const
        throw(std::runtime_error);

    /**
      * Returns the version of the driver as a number composed by the 
      * major and minor number: <code>version = major.minor</code>.
      * @return the version of the driver
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    float GetDriverVersion() const
        throw(std::runtime_error);

    /**
      * Returns the version of the driver as a number composed by the 
      * major and minor number: <code>version = major.minor</code>.
      * @param deviceType specify the type of Quantis device.
      * @return the version of the driver
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    static float GetDriverVersion(QuantisDeviceType deviceType) throw(std::runtime_error);

    /**
      * Returns the library version as a number composed by the major
      * and minor number: <code>version = major.minor</code>
      * @return the library version.
      */
    static float GetLibVersion();

    /**
      * Get a pointer to the manufacturer's string of the Quantis device.
      * @return the manufacturer of the Quantis device or "Not available"
      * when an error occurred or when the device do not support the operation
      * (currently only Quantis USB returns a valid string).
      */
    std::string GetManufacturer() const;

    /**
      * Returns the number of modules that have been detected on a Quantis
      * device.
      * @return the number of detected modules
      * @throw runtime_error QUANTIS_ERROR code on failure.
      * @see QuantisGetModulesMask
      */
    int GetModulesCount() const
        throw(std::runtime_error);

    /**
      * Returns a bitmask of the modules that have been detected on a Quantis
      * device, where bit <em>n</em> is set if module <em>n</em> is present.
      * For instance when 5 (1101 in binary) is returned, it means that modules
      * 0, 2 and 3 have been detected.
      * @return a bitmask of the detected modules
      * @throw runtime_error QUANTIS_ERROR code on failure.
      * @see QuantisGetModulesStatus
      */
    int GetModulesMask() const
        throw(std::runtime_error);

    /**
      * Returns the data rate (in Bytes per second) provided by the Quantis device.
      * @return the data rate provided by the Quantis device
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    int GetModulesDataRate() const
        throw(std::runtime_error);

    /**
      * Get the power status of the modules.
      * @return true if the modules are powered, false if the modules are not powered
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    bool GetModulesPower() const
        throw(std::runtime_error);

    /**
      * Returns the status of the modules on the device as a bitmask as defined 
      * in QuantisGetModulesMask. Bit <em>n</em> is set (equal to 1) only when
      * module <em>n</em> is enabled and functional. 
      * @return A bitmask with the status of the modules
      * @throw runtime_error QUANTIS_ERROR code on failure.
      * @see QuantisGetModulesMask
      */
    int GetModulesStatus() const
        throw(std::runtime_error);

    /**
      * Get a string representing the serial number string of the Quantis device.
      * @return the serial number of the Quantis device or "S/N not available"
      * when an error occurred or when the device do not support the operation
      * (currently only Quantis USB returns a valid serial number).
      */
    std::string GetSerialNumber() const;

    /**
      * Disable one ore more modules.
      * @param modulesMask a bitmask of the modules (as specified in 
      * QuantisGetModulesMask) that must be disabled.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    void ModulesDisable(int modulesMask) const
        throw(std::runtime_error);

    /**
      * Enable one ore more modules.
      * @param modulesMask a bitmask of the modules (as specified in 
      * QuantisGetModulesMask) that must be enabled.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    void ModulesEnable(int modulesMask) const
        throw(std::runtime_error);

    /**
      * Reset one or more modules.
      * @param modulesMask a bitmask of the modules (as specified in 
      * QuantisGetModulesMask) that must be reset.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      * @warning This function just call QuantisModulesDisable and then 
      * QuantisModulesEnable with the provided modulesMask.
      */
    void ModulesReset(int modulesMask) const
        throw(std::runtime_error);

    /**
      * Get the device Id on the PCI or USB bus.
      * @return the deviceId of the board
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    int GetBusDeviceId() const
        throw(std::runtime_error);

    /**
      * Get the AIS-31 Startup tests request flag.
      * @return 1 if startup tests are requested, otherwise 0
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    int GetAis31StartupTestsRequestFlag() const
        throw(std::runtime_error);

    /**
      * Clear the AIS-31 Startup tests request flag.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    void ClearAis31StartupTestsRequestFlag() const
        throw(std::runtime_error);

    /**
      * Reads random data from the Quantis device.
      * @param size the number of bytes to read (not larger than QUANTIS_MAX_READ_SIZE).
      * @return a string holding the random data.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    std::string Read(size_t size) const
        throw(std::runtime_error);

    /**
      * Reads random data from the Quantis device.
      * @param buffer a pointer to a destination buffer. This buffer MUST 
      * already be allocated. Its size must be at least <em>size</em> bytes.
      * @param size the number of bytes to read (not larger than QUANTIS_MAX_READ_SIZE).
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    void Read(void *buffer, size_t size) const
        throw(std::runtime_error);

    /**
      * Reads a random double floating precision value between 0.0 (inclusive)
      * and 1.0 (exclusive) from the Quantis device.
      * @return a double random number.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    double ReadDouble() const
        throw(std::runtime_error);

    /**
      * Reads a random double from the Quantis device and scale it to be between 
      * min (inclusive) and max (exclusive).
      * @param min the minimal value the random number can take.
      * @param max the maximal value the random number can take.
      * @return a double random number.
      * @throw runtime_error QUANTIS_ERROR code on failure.       
      */
    double ReadDouble(double min, double max) const
        throw(std::runtime_error);

    /**
      * Reads a random float floating precision value between 0.0 (inclusive)
      * and 1.0 (exclusive) from the Quantis device.
      * @return a float random number.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    float ReadFloat() const
        throw(std::runtime_error);

    /**
      * Reads a random float from the Quantis device and scale it to be between 
      * min (inclusive) and max (exclusive).
      * @param min the minimal value the random number can take.
      * @param max the maximal value the random number can take.
      * @return a float random number.
      * @throw runtime_error QUANTIS_ERROR code on failure.       
      */
    float ReadFloat(float min, float max) const
        throw(std::runtime_error);

    /**
      * Reads a random integer precision value from the Quantis device.
      * @return a int random number.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    int ReadInt() const
        throw(std::runtime_error);

    /**
      * Reads a random integer from the Quantis device and scale it to be between 
      * min (inclusive) and max (exclusive).
      * @param min the minimal value the random number can take.
      * @param max the maximal value the random number can take.
      * @return a int random number.
      * @throw runtime_error QUANTIS_ERROR code on failure.       
      */
    int ReadInt(int min, int max) const
        throw(std::runtime_error);

    /**
      * Reads a random short integer precision value from the Quantis device.
      * @return a short int random number.
      * @throw runtime_error QUANTIS_ERROR code on failure.
      */
    short ReadShort() const
        throw(std::runtime_error);

    /**
      * Reads a random short integer from the Quantis device and scale it to be between 
      * min (inclusive) and max (exclusive).
      * @param min the minimal value the random number can take.
      * @param max the maximal value the random number can take.
      * @return a short int random number.
      * @throw runtime_error QUANTIS_ERROR code on failure.       
      */
    short ReadShort(short min, short max) const
        throw(std::runtime_error);

private:
    QuantisDeviceType _deviceType;
    unsigned int _deviceNumber;
};
} // namespace idQ

#endif
