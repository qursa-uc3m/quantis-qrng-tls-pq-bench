/*
 * QuantisExtensions C++ Library
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

#ifndef QUANTIS_EXTRACTOR_HPP
#define QUANTIS_EXTRACTOR_HPP

#include <stdexcept>
#include <string>
#include <vector>

#include "../Quantis/Quantis.h"
#include "QuantisExtractor.h"

#ifdef _WIN32
// Visual C++ does not implement checked exceptions
// Disables "C++ exception specification ignored except to indicate a function is not __declspec(nothrow)"
#pragma warning(disable : 4290)
#endif

namespace idQ
{
class DLL_EXPORT QuantisExtractor
{
public:
  QuantisExtractor();
  ~QuantisExtractor();

  /**
      * Returns the library version as a number composed by the major
      * and minor number: <code>version = major.minor</code>
      * @return the library version.
      */
  static float GetLibVersion();

  /**
      * Reads the extractor matrix from the specified file and store in memory
      * @param matrixFilename the filename of the matrix
      * @param matrixSizeIn the number of bits which are input to the extractor
      * @param matrixSizeOut the number of bits which are output to the extractor
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure
      */
  void InitializeMatrix(const std::string &matrixFilename,
                        const uint16_t matrixSizeIn = 1024,
                        const uint16_t matrixSizeOut = 768) throw(std::runtime_error);

  /**
      * Free the memory of the extractor matrix
      * @param extractorMatrix pointer to the pointer to the buffer where to store the extractor matrix
      */
  void UninitializeMatrix();

  /**
      * Reads random data from the Quantis device and apply the randomness extraction.
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @param buffer a pointer to a destination buffer. This buffer MUST 
      * already be allocated. Its size must be at least of "bytesNum" bytes.
      * @param bytesNum the number of bytes to read.      
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  void GetDataFromQuantis(const QuantisDeviceType deviceType,
                          const unsigned int cardNumber,
                          void *buffer,
                          const size_t bytesNum) throw(std::runtime_error);

  /**
      * Reads random data from the Quantis device and apply the randomness extraction.
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @param bytesNum the number of bytes to read.
      * @return a string holding the read bytes on success.
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure        
      */
  std::string GetDataFromQuantis(const QuantisDeviceType deviceType,
                                 const unsigned int cardNumber,
                                 const size_t bytesNum) throw(std::runtime_error);

  /**
      * Reads a random double floating precision value between 0.0 (inclusive)
      * and 1.0 (exclusive) from the Quantis device.
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @return a double value.
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  double GetDoubleFromQuantis(const QuantisDeviceType deviceType,
                              const unsigned int cardNumber) const
      throw(std::runtime_error);

  /**
      * Reads a double random number from the Quantis device and scale it to be between 
      * min (inclusive) and max (exclusive).
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @param min the minimal value the random number can take.
      * @param max the maximal value the random number can take.
      * @return a double value       
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  double GetDoubleFromQuantis(const QuantisDeviceType deviceType,
                              const unsigned int cardNumber,
                              double min,
                              double max) const
      throw(std::runtime_error);

  /**
      * Reads a random float floating precision value between 0.0 (inclusive)
      * and 1.0 (exclusive) from the Quantis device.
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @return a float value.
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  float GetFloatFromQuantis(const QuantisDeviceType deviceType,
                            const unsigned int cardNumber) const
      throw(std::runtime_error);

  /**
      * Reads a float random number from the Quantis device and scale it to be between 
      * min (inclusive) and max (exclusive).
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @param min the minimal value the random number can take.
      * @param max the maximal value the random number can take.
      * @return a float value       
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  float GetFloatFromQuantis(const QuantisDeviceType deviceType,
                            const unsigned int cardNumber,
                            float min,
                            float max) const
      throw(std::runtime_error);

  /**
      * Reads a random integer precision value
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @return a int value.
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  int GetIntFromQuantis(const QuantisDeviceType deviceType,
                        const unsigned int cardNumber) const
      throw(std::runtime_error);

  /**
      * Reads a integer random number from the Quantis device and scale it to be between 
      * min (inclusive) and max (exclusive).
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @param min the minimal value the random number can take.
      * @param max the maximal value the random number can take.
      * @return a int value       
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  int GetIntFromQuantis(const QuantisDeviceType deviceType,
                        const unsigned int cardNumber,
                        int min,
                        int max) const
      throw(std::runtime_error);

  /**
      * Reads a random short int precision value
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @return a short int value.
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  short GetShortFromQuantis(const QuantisDeviceType deviceType,
                            const unsigned int cardNumber) const
      throw(std::runtime_error);

  /**
      * Reads a short integer random number from the Quantis device and scale it to be between 
      * min (inclusive) and max (exclusive).
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @param min the minimal value the random number can take.
      * @param max the maximal value the random number can take.
      * @return a short int value       
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  short GetShortFromQuantis(const QuantisDeviceType deviceType,
                            const unsigned int cardNumber,
                            short min,
                            short max) const
      throw(std::runtime_error);

  /**
      * Reads random data from the input file and apply the randomness extraction.
      * @param inputFilename the path of the file to process.
      * @param outputFilename the path of the file to save the data after randomness extraction.
      * @return the number of bytes after randomness extraction
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  int32_t GetDataFromFile(const std::string &inputFilename,
                          const std::string &outputFilename) throw(std::runtime_error);

  /**
       * Reads random data from the input buffer and apply the randomness extraction.
       * @param inputBuffer pointer to the buffer containing the bytes to be processed
       * @param outputBuffer pointer to the buffer where the result of the extraction should be stored (should be already allocated)
       * @param numberOfBytesAfterExtraction the number of processed bytes which should be produced.
       */
  void GetDataFromBuffer(const void *inputBuffer,
                         void *outputBuffer,
                         size_t numberOfBytesAfterExtraction) throw(std::runtime_error);

  /**
      * The function initializes an output buffer in order to contain the result of the extraction of an input buffer of inputBufferSize bytes
      * @param inputBufferSize number of bytes contained in the input buffer
      * @param outputBuffer  pointer to the pointer to the buffer which should be initialized
      * @return the number of bytes that should be obtained after the extraction, a QUANTIS_EXT_ERROR otherwise
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  uint32_t InitializeOutputBuffer(const uint32_t inputBufferSize,
                                  uint8_t **outputBuffer) throw(std::runtime_error);

  /**
      * Get the number of bits which are input to the extractor
      * @return matrixSizeIn the number of bits which are input to the extractor 
      */
  uint16_t GetMatrixSizeIn() const;

  /**
      * Get the number of bits which are input to the extractor
      * @return matrixSizeOut the number of bits which are output to the extractor 
      */
  uint16_t GetMatrixSizeOut() const;

  /**
      * The function frees the memory eventually allocated for outputBuffer
      * @param outputBuffer pointer to the pointer to the buffer which has to be freed
      */
  void UninitializeOutputBuffer(uint8_t *outputBuffer);

  /**
      * Apply extraction processing function to a block of matrixSizeIn (also called n) bits and produce 
      * matrixSizeOut (also called k) processed bits (n and k global variables specifying the parameters of the extractor)
      * The function consists of a matrix multiplication: 
      *    [output] (k x 1) = {extractor} (k x n) * [input] (n x 1)
      * @param *inputBuffer input of the extractor (uint64_t for multiplication efficiency)
      * @param *outputBuffer output of the extractor (uint64_t for multiplication efficiency)
      */
  void ProcessBlock(const uint64_t *inputBuffer, uint64_t *outputBuffer);

  /** Enable the storage buffer and allocate MAX_BUFFER_SIZE bytes for it
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  void EnableStorageBuffer() throw(std::runtime_error);

  /** Disable the storage buffer (if it was previously activated) and free the correspondigly allocated memory
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  void DisableStorageBuffer() throw(std::runtime_error);

  /**
      * Write to file an elementary matrix created by applying ProcessBufferVonNeumann to the buffer produced by ReadUnderSampled
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @param matrixSizeIn the number of bits which are input to the extractor
      * @param matrixSizeOut the number of bits which are output to the extractor
      * @param underSamplingPeriod number of bytes to down sample. should be >= 13
      * @param *elementaryMatrixFilename path of the file where the seed should be written
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  void CreateElementaryMatrix(const QuantisDeviceType deviceType,
                              const unsigned int deviceNumber,
                              const uint16_t matrixSizeIn,
                              const uint16_t matrixSizeOut,
                              const uint16_t underSamplingPeriod,
                              const std::string &elementaryMatrixFilename) throw(std::runtime_error);

  /** 
      * XOR the bitstreams contained in the number of Elementary Matrices files specified in elementaryMatricesFilename
      * @param matrixSize number of bytes that should be XORed
      * @param elementaryMatricesFilename a string vector containing the name of the files where the different elementary matrices are stored
      * @param extractorMatrixFilename path of the file where the final extractor matrix should be saved
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure 
      */
  void CreateMatrix(const uint32_t matrixSize,
                    const std::vector<std::string> &elementaryMatricesFilename,
                    const std::string &extractorMatrixFilename) throw(std::runtime_error);

  /**
      * Apply the Von Neumann post-processing to the bit sequence which was read from 
      * the inputFile and write the processed sequence into the output buffer
      * @param *inputBuffer input buffer to be processed
      * @param *outputBuffer output buffer which will contain the processed bytes. 
      *        The buffer should be already initialized by the function which invokes this method; 
      *        please note that the number of bytes that will be output by the Von Neumann processing 
      *       is not deterministic, as it depends on the input sequence, but it can never exceed 1/2 of the input buffer size.
      * @return number of bytes at the output of the Von Neumann processing
      */
  uint32_t ProcessBufferVonNeumann(std::vector<uint8_t> &inputBuffer,
                                   std::vector<uint8_t> &outputBuffer);

  /**
      * Sample the QuantisRead output according to samplingRate and save the output stream in sampledBuffer
      * @param deviceType specify the type of Quantis device.
      * @param deviceNumber the number of the Quantis device.
      * @param nbrOfBytesRequested number of bytes which should be output to the user after the sampling 
      *                               of the data read from the Quantis
      * @param underSamplingPeriod number of bytes to down sampled. should >= 13
      * @param *sampledBuffer buffer where to store the sampled sequence
      * @return number of read sampled bytes
      * @throw runtime_error QUANTIS_EXT_ERROR code on failure  
      */
  uint32_t ReadUnderSampled(const QuantisDeviceType deviceType,
                            const unsigned int deviceNumber,
                            const uint32_t nbrOfBytesRequested,
                            const uint16_t underSamplingPeriod,
                            std::vector<uint8_t> &sampledBuffer) throw(std::runtime_error);

  /**
      * Get an error message string for the QuantisExtensions library.
      *
      * This functions interprets the value of errorNumber and generates a string
      * describing the error.
      *
      * The returned pointer points to a statically allocated string, which shall
      * not be modified by the program. Further calls to this function will
      * overwrite its content.
      *
      * @param errorNumber The error number.
      * @return A string containing the error message.
      */
  std::string StrError(const QuantisExtractorError errorNumber);

private:
  bool _matrixInitalized;
  uint64_t *_matrix;
  std::string _matrixFilename;
  uint16_t _matrixSizeIn;  // in bits
  uint16_t _matrixSizeOut; // in bits
};
} // namespace idQ

#endif
