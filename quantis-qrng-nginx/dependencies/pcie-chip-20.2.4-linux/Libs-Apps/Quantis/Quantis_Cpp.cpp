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

#include "Conversion.h"
#include "Quantis.hpp"
#include "Quantis_Internal.h"

using namespace std;

// -------------------------- Private members --------------------------

/** 
 * Quantis device handler.
 * 
 * This internal variable is declared here to avoid inclusion of 
 * Quantis_Internal.h in Quantis.hpp.
 */
QuantisDeviceHandle *deviceHandle = NULL;

/**
 * Checks if a Quantis function returned an error.
 * @param result an integer value returned by a Quantis* function.
 * @throw std::runtime_error if result is negative. The exception message
 * contains details about the error.
 */
void inline CheckError(int result) throw(std::runtime_error)
{
  // All errors are negative
  if (result < 0)
  {
    QuantisError err = static_cast<QuantisError>(result);
    throw runtime_error("Quantis: " + string(QuantisStrError(err)));
  }
}

/**
 * Checks if a Quantis function returned an error.
 * @param result an integer value returned by a Quantis* function.
 * @throw std::runtime_error if result is negative. The exception message
 * contains details about the error.
 */
void inline CheckFullError(QuantisDeviceType deviceType, int result) throw(std::runtime_error)
{
  // All errors are negative
  if (result < 0)
  {
    QuantisError err = static_cast<QuantisError>(result);
    throw runtime_error("Quantis: " + string(QuantisFullStrError(deviceType, err)));
  }
}

// ------------------------ Functions implementation ------------------------

idQ::Quantis::Quantis(QuantisDeviceType deviceType, unsigned int deviceNumber) throw(std::runtime_error)
    : _deviceType(deviceType),
      _deviceNumber(deviceNumber)
{
  CheckError(QuantisOpenInternal(deviceType, deviceNumber, &deviceHandle));
}

idQ::Quantis::~Quantis()
{
  QuantisCloseInternal(deviceHandle);
}

void idQ::Quantis::BoardReset() const
    throw(std::runtime_error)
{
  CheckError(deviceHandle->ops->BoardReset(deviceHandle));
}

int idQ::Quantis::Count(QuantisDeviceType deviceType) throw(std::runtime_error)
{
  int result = QuantisCount(deviceType);
  CheckError(result);
  return result;
}

int idQ::Quantis::GetBoardVersion() const
    throw(std::runtime_error)
{
  int result = deviceHandle->ops->GetBoardVersion(deviceHandle);
  CheckError(result);
  return result;
}

float idQ::Quantis::GetDriverVersion() const
    throw(std::runtime_error)
{
  float result = deviceHandle->ops->GetDriverVersion();
  CheckError(static_cast<int>(result));
  return result;
}

float idQ::Quantis::GetDriverVersion(QuantisDeviceType deviceType) throw(std::runtime_error)
{
  float result = QuantisGetDriverVersion(deviceType);
  CheckError(static_cast<int>(result));
  return result;
}

float idQ::Quantis::GetLibVersion()
{
  return QuantisGetLibVersion();
}

std::string idQ::Quantis::GetManufacturer() const
{
  return string(deviceHandle->ops->GetManufacturer(deviceHandle));
}

int idQ::Quantis::GetModulesCount() const
    throw(std::runtime_error)
{
  int modulesMask = GetModulesMask();
  return QuantisCountSetBits(modulesMask);
}

int idQ::Quantis::GetModulesMask() const
    throw(std::runtime_error)
{
  int result = deviceHandle->ops->GetModulesMask(deviceHandle);
  CheckError(result);
  return result;
}

int idQ::Quantis::GetModulesDataRate() const
    throw(std::runtime_error)
{
  int result = deviceHandle->ops->GetModulesDataRate(deviceHandle);
  CheckError(result);
  return result;
}

int idQ::Quantis::GetModulesStatus() const
    throw(std::runtime_error)
{
  int result = deviceHandle->ops->GetModulesStatus(deviceHandle);
  CheckError(result);
  return result;
}

bool idQ::Quantis::GetModulesPower() const
    throw(std::runtime_error)
{
  int result = deviceHandle->ops->GetModulesPower(deviceHandle);
  CheckError(result);
  return (result != 0);
}

std::string idQ::Quantis::GetSerialNumber() const
{
  return string(deviceHandle->ops->GetSerialNumber(deviceHandle));
}

void idQ::Quantis::ModulesDisable(int modulesMask) const
    throw(std::runtime_error)
{
  CheckError(deviceHandle->ops->ModulesDisable(deviceHandle, modulesMask));
}

void idQ::Quantis::ModulesEnable(int modulesMask) const
    throw(std::runtime_error)
{
  CheckError(deviceHandle->ops->ModulesEnable(deviceHandle, modulesMask));
}

void idQ::Quantis::ModulesReset(int modulesMask) const
    throw(std::runtime_error)
{
  ModulesDisable(modulesMask);
  ModulesEnable(modulesMask);
}

int idQ::Quantis::GetBusDeviceId() const
    throw(std::runtime_error)
{
  int result = deviceHandle->ops->GetBusDeviceId(deviceHandle);
  CheckError(result);
  return result;
}

int idQ::Quantis::GetAis31StartupTestsRequestFlag() const
    throw(std::runtime_error)
{
  int result = deviceHandle->ops->GetAis31StartupTestsRequestFlag(deviceHandle);
  CheckError(result);
  return result;
}

void idQ::Quantis::ClearAis31StartupTestsRequestFlag() const
    throw(std::runtime_error)
{
  int result = deviceHandle->ops->ClearAis31StartupTestsRequestFlag(deviceHandle);
  CheckError(result);
}

std::string idQ::Quantis::Read(size_t size) const
    throw(std::runtime_error)
{
  string buffer;
  buffer.resize(size);

  // The "&buffer[0]" assumes std::string implemented in contiguous memory.
  // Or, in current ISO C++ (C++98 and C++03), std::string is not fully
  // required to store its data contiguously, but most of known std::string
  // implementation are in fact contiguous (GNU GCC, Visual Studio). So,
  // although it isn't formally guaranteed, in practice we can probably get
  // away with calling &buffer[0] to get a pointer to a contiguous and
  // null-terminated string.
  Read(&buffer[0], size);
  return buffer;
}

void idQ::Quantis::Read(void *buffer, size_t size) const
    throw(std::runtime_error)
{
  if (size == 0u)
  {
    return;
  }
  else if (size > QUANTIS_MAX_READ_SIZE)
  {
    // Throws error
    CheckError(QUANTIS_ERROR_INVALID_READ_SIZE);
  }

  int readBytes = deviceHandle->ops->Read(deviceHandle, buffer, size);
  CheckFullError(_deviceType, readBytes);
}

double idQ::Quantis::ReadDouble() const
    throw(std::runtime_error)
{
  string buffer = Read(sizeof(double));
  return ConvertToDouble_01(buffer.c_str());
}

double idQ::Quantis::ReadDouble(double min, double max) const
    throw(std::runtime_error)
{
  if (min > max)
  {
    // Throw error
    CheckError(QUANTIS_ERROR_INVALID_PARAMETER);
  }

  return ReadDouble() * (max - min) + min;
}

float idQ::Quantis::ReadFloat() const
    throw(std::runtime_error)
{
  string buffer = Read(sizeof(float));
  return ConvertToFloat_01(buffer.c_str());
}

float idQ::Quantis::ReadFloat(float min, float max) const
    throw(std::runtime_error)
{
  if (min > max)
  {
    // Throw error
    CheckError(QUANTIS_ERROR_INVALID_PARAMETER);
  }

  return ReadFloat() * (max - min) + min;
}

int idQ::Quantis::ReadInt() const
    throw(std::runtime_error)
{
  string buffer = Read(sizeof(int));
  return ConvertToInt(buffer.c_str());
}

int idQ::Quantis::ReadInt(int min, int max) const
    throw(std::runtime_error)
{
  int tmp;
  const int BITS = sizeof(tmp) * 8;
  const unsigned long long RANGE = max - min + 1;
  const unsigned long long MAX_RANGE = 1ull << BITS;
  const unsigned long long LIMIT = MAX_RANGE - (MAX_RANGE % RANGE);

  if (min > max)
  {
    // Throw error
    CheckError(QUANTIS_ERROR_INVALID_PARAMETER);
  }

  // Chooses the highest number that is the largest multiple of the output range
  // (discard values higher the output range)
  do
  {
    tmp = ReadInt();
  } while ((tmp > 0) && (static_cast<unsigned long long>(tmp) >= LIMIT));

  return static_cast<int>(tmp % RANGE + min);
}

short idQ::Quantis::ReadShort() const
    throw(std::runtime_error)
{
  string buffer = Read(sizeof(short));
  return ConvertToShort(buffer.c_str());
}

short idQ::Quantis::ReadShort(short min, short max) const
    throw(std::runtime_error)
{
  short tmp;
  const int BITS = sizeof(tmp) * 8;
  const unsigned int RANGE = max - min + 1;
  const unsigned int MAX_RANGE = 1u << BITS;
  const unsigned int LIMIT = MAX_RANGE - (MAX_RANGE % RANGE);

  if (min > max)
  {
    // Throw error
    CheckError(QUANTIS_ERROR_INVALID_PARAMETER);
  }

  // Chooses the highest number that is the largest multiple of the output range
  // (discard values higher the output range)
  do
  {
    tmp = ReadShort();
  } while ((tmp > 0) && (static_cast<unsigned int>(tmp) >= LIMIT));

  return static_cast<short>(tmp % RANGE + min);
}
