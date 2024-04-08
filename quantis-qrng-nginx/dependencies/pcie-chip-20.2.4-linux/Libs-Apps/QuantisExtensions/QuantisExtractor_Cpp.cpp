/*
 * Quantis Extensions C++ Library
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

#include <stdlib.h>

#include "QuantisExtractor.hpp"

using namespace std;

// -------------------------- Global functions --------------------------
/**
 * Checks if a Quantis function returned an error.
 * @param result an integer value returned by a Quantis* function.
 * @throw std::runtime_error if result is negative. The exception message
 * contains details about the error.
 */
void inline CheckError(const int result, const string &methodName) throw(std::runtime_error)
{
  // All errors are negative
  if (result < 0)
  {
    const QuantisExtractorError err = static_cast<QuantisExtractorError>(result);
    const string err_msg(::QuantisExtractorStrError(err));
    throw runtime_error(methodName + ": " + err_msg);
  }
}

// ------------------------ Functions implementation ------------------------

idQ::QuantisExtractor::QuantisExtractor()
{
  _matrixInitalized = false;
}

idQ::QuantisExtractor::~QuantisExtractor()
{
}

float idQ::QuantisExtractor::GetLibVersion()
{
  return ::QuantisExtractorGetLibVersion();
}

void idQ::QuantisExtractor::InitializeMatrix(const std::string &matrixFilename,
                                             const uint16_t matrixSizeIn,
                                             const uint16_t matrixSizeOut) throw(std::runtime_error)
{

  if (_matrixInitalized)
  {
    ::QuantisExtractorUninitializeMatrix(&_matrix);
  }

  _matrixFilename = matrixFilename;
  _matrixSizeIn = matrixSizeIn;
  _matrixSizeOut = matrixSizeOut;

  const int32_t res = ::QuantisExtractorInitializeMatrix(const_cast<char *>(matrixFilename.c_str()),
                                                         &_matrix,
                                                         matrixSizeIn,
                                                         matrixSizeOut);
  CheckError(res, "InitializeMatrix");
  _matrixInitalized = true;
}

void idQ::QuantisExtractor::UninitializeMatrix()
{
  ::QuantisExtractorUninitializeMatrix(&_matrix);
}

void idQ::QuantisExtractor::GetDataFromQuantis(const QuantisDeviceType deviceType,
                                               const unsigned int cardNumber,
                                               void *buffer,
                                               const size_t bytesNum) throw(std::runtime_error)
{
  if (bytesNum == 0u)
  {
    return;
  }
  else if (bytesNum > QUANTIS_MAX_READ_SIZE)
  {
    // Throws error
    CheckError(QUANTIS_ERROR_INVALID_READ_SIZE, "GetDataFromQuantis");
  }

  if (!_matrixInitalized)
  {
    throw runtime_error("QuantisExtensions: Matrix not initialized");
  }

  int readBytes;

  readBytes = ::QuantisExtractorGetDataFromQuantis(deviceType,
                                                   cardNumber,
                                                   static_cast<uint8_t *>(buffer),
                                                   static_cast<uint32_t>(bytesNum),
                                                   _matrix);

  CheckError(readBytes, "GetDataFromQuantis");
}

std::string idQ::QuantisExtractor::GetDataFromQuantis(const QuantisDeviceType deviceType,
                                                      const unsigned int cardNumber,
                                                      const size_t bytesNum) throw(std::runtime_error)
{
  string buffer;
  buffer.resize(bytesNum);

  // The "&buffer[0]" assumes std::string implemented in contiguous memory.
  // Or, in current ISO C++ (C++98 and C++03), std::string is not fully
  // required to store its data contiguously, but most of known std::string
  // implementation are in fact contiguous (GNU GCC, Visual Studio). So,
  // although it isn't formally guaranteed, in practice we can probably get
  // away with calling &buffer[0] to get a pointer to a contiguous and
  // null-terminated string.

  this->GetDataFromQuantis(deviceType,
                           cardNumber,
                           (uint8_t *)&buffer[0],
                           bytesNum);

  return buffer;
}

int32_t idQ::QuantisExtractor::GetDataFromFile(const std::string &inputFilename,
                                               const std::string &outputFilename) throw(std::runtime_error)
{
  int32_t readBytes;

  readBytes = ::QuantisExtractorGetDataFromFile(const_cast<char *>(inputFilename.c_str()),
                                                const_cast<char *>(outputFilename.c_str()),
                                                _matrix);
  CheckError(readBytes, "GetDataFromQuantis");

  return readBytes;
}

void idQ::QuantisExtractor::GetDataFromBuffer(const void *inputBuffer,
                                              void *outputBuffer,
                                              size_t numberOfBytesAfterExtraction) throw(std::runtime_error)
{
  ::QuantisExtractorGetDataFromBuffer(static_cast<const uint8_t *>(inputBuffer),
                                      static_cast<uint8_t *>(outputBuffer),
                                      _matrix,
                                      static_cast<uint32_t>(numberOfBytesAfterExtraction));
}

uint32_t idQ::QuantisExtractor::InitializeOutputBuffer(const uint32_t inputBufferSize,
                                                       uint8_t **outputBuffer) throw(std::runtime_error)
{
  int32_t result;

  result = ::QuantisExtractorInitializeOutputBuffer(inputBufferSize, outputBuffer);

  CheckError(result, "InitializeOutputBuffer");

  return static_cast<uint32_t>(result);
}

uint16_t idQ::QuantisExtractor::GetMatrixSizeIn() const
{
  return _matrixSizeIn;
}

uint16_t idQ::QuantisExtractor::GetMatrixSizeOut() const
{
  return _matrixSizeOut;
}

void idQ::QuantisExtractor::UninitializeOutputBuffer(uint8_t *outputBuffer)
{
  ::QuantisExtractorUnitializeOutputBuffer(&outputBuffer);
}

void idQ::QuantisExtractor::ProcessBlock(const uint64_t *inputBuffer, uint64_t *outputBuffer)
{
  ::QuantisExtractorProcessBlock(inputBuffer, outputBuffer, _matrix);
}

void idQ::QuantisExtractor::CreateElementaryMatrix(const QuantisDeviceType deviceType,
                                                   const unsigned int deviceNumber,
                                                   const uint16_t matrixSizeIn,
                                                   const uint16_t matrixSizeOut,
                                                   const uint16_t underSamplingPeriod,
                                                   const std::string &elementaryMatrixFilename) throw(std::runtime_error)
{
  const int32_t res = ::QuantisExtractorMatrixCreateElementary(deviceType,
                                                               deviceNumber,
                                                               matrixSizeIn,
                                                               matrixSizeOut,
                                                               underSamplingPeriod,
                                                               const_cast<char *>(elementaryMatrixFilename.c_str()));
  CheckError(res, "CreateElementaryMatrix");
}

void idQ::QuantisExtractor::CreateMatrix(const uint32_t matrixSize,
                                         const std::vector<std::string> &elementaryMatricesFilenames,
                                         const std::string &extractorMatrixFilename) throw(std::runtime_error)
{
  uint32_t numberOfElementaryFiles = static_cast<uint32_t>(elementaryMatricesFilenames.size());
  //char *elementaryMatricesToXor[numberOfElementaryFiles];
  char **elementaryMatricesToXor = new char *[numberOfElementaryFiles];

  for (uint32_t i = 0; i < numberOfElementaryFiles; i++)
  {
    char *toto = new char[256];

    elementaryMatricesToXor[i] = toto;

    strcpy(elementaryMatricesToXor[i], elementaryMatricesFilenames[i].c_str());
  }

  int32_t res = ::QuantisExtractorMatrixCreate(numberOfElementaryFiles,
                                               matrixSize,
                                               elementaryMatricesToXor,
                                               const_cast<char *>(extractorMatrixFilename.c_str()));
  CheckError(res, "CreateMatrix");

  //free memory
  for (uint32_t i = 0; i < numberOfElementaryFiles; i++)
  {
    delete elementaryMatricesToXor[i];
  }

  delete elementaryMatricesToXor;
}

uint32_t idQ::QuantisExtractor::ProcessBufferVonNeumann(std::vector<uint8_t> &inputBuffer,
                                                        std::vector<uint8_t> &outputBuffer)
{
  return ::QuantisExtractorMatrixProcessBufferVonNeumann(&inputBuffer[0],
                                                         &outputBuffer[0],
                                                         static_cast<uint32_t>(inputBuffer.size()));
}

uint32_t idQ::QuantisExtractor::ReadUnderSampled(const QuantisDeviceType deviceType,
                                                 const unsigned int deviceNumber,
                                                 const uint32_t nbrOfBytesRequested,
                                                 const uint16_t underSamplingPeriod,
                                                 std::vector<uint8_t> &sampledBuffer) throw(std::runtime_error)
{
  uint32_t result;

  result = QuantisExtractorMatrixUnderSamplingRead(deviceType,
                                                   deviceNumber,
                                                   nbrOfBytesRequested,
                                                   underSamplingPeriod,
                                                   &sampledBuffer[0]);

  CheckError(result, "ReadUnderSampled");

  return static_cast<uint32_t>(result);
}

void idQ::QuantisExtractor::EnableStorageBuffer() throw(std::runtime_error)
{
  CheckError(::QuantisExtractorStorageBufferEnable(), "EnableStorageBuffer");
}

void idQ::QuantisExtractor::DisableStorageBuffer() throw(std::runtime_error)
{
  CheckError(::QuantisExtractorStorageBufferDisable(), "DisableStorageBuffer");
}

std::string idQ::QuantisExtractor::StrError(const QuantisExtractorError errorNumber)
{
  return string(::QuantisExtractorStrError(errorNumber));
}

double idQ::QuantisExtractor::GetDoubleFromQuantis(const QuantisDeviceType deviceType,
                                                   const unsigned int cardNumber) const
    throw(std::runtime_error)
{
  double value;
  CheckError(::QuantisExtractorReadDouble_01(deviceType, cardNumber, &value, _matrix), "GetDoubleFromQuantis");

  return value;
}

double idQ::QuantisExtractor::GetDoubleFromQuantis(const QuantisDeviceType deviceType,
                                                   const unsigned int cardNumber,
                                                   double min,
                                                   double max) const
    throw(std::runtime_error)
{
  double value;
  CheckError(::QuantisExtractorReadScaledDouble(deviceType, cardNumber, &value, min, max, _matrix), "GetDoubleFromQuantis");

  return value;
}

float idQ::QuantisExtractor::GetFloatFromQuantis(const QuantisDeviceType deviceType,
                                                 const unsigned int cardNumber) const
    throw(std::runtime_error)
{
  float value;
  CheckError(::QuantisExtractorReadFloat_01(deviceType, cardNumber, &value, _matrix), "GetFloatFromQuantis");

  return value;
}

float idQ::QuantisExtractor::GetFloatFromQuantis(const QuantisDeviceType deviceType,
                                                 const unsigned int cardNumber,
                                                 float min,
                                                 float max) const
    throw(std::runtime_error)
{
  float value;
  CheckError(::QuantisExtractorReadScaledFloat(deviceType, cardNumber, &value, min, max, _matrix), "GetFloatFromQuantis");

  return value;
}

int idQ::QuantisExtractor::GetIntFromQuantis(const QuantisDeviceType deviceType,
                                             const unsigned int cardNumber) const
    throw(std::runtime_error)
{
  int value;
  CheckError(::QuantisExtractorReadInt(deviceType, cardNumber, &value, _matrix), "GetIntFromQuantis");

  return value;
}

int idQ::QuantisExtractor::GetIntFromQuantis(const QuantisDeviceType deviceType,
                                             const unsigned int cardNumber,
                                             int min,
                                             int max) const
    throw(std::runtime_error)
{
  int value;
  CheckError(::QuantisExtractorReadScaledInt(deviceType, cardNumber, &value, min, max, _matrix), "GetIntFromQuantis");

  return value;
}

short idQ::QuantisExtractor::GetShortFromQuantis(const QuantisDeviceType deviceType,
                                                 const unsigned int cardNumber) const
    throw(std::runtime_error)
{
  short value;
  CheckError(::QuantisExtractorReadShort(deviceType, cardNumber, &value, _matrix), "GetShortFromQuantis");

  return value;
}

short idQ::QuantisExtractor::GetShortFromQuantis(const QuantisDeviceType deviceType,
                                                 const unsigned int cardNumber,
                                                 short min,
                                                 short max) const
    throw(std::runtime_error)
{
  short value;
  CheckError(::QuantisExtractorReadScaledShort(deviceType, cardNumber, &value, min, max, _matrix), "GetShortFromQuantis");

  return value;
}
