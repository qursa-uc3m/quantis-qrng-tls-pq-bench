/*
 * Quantis_Extensions C library
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

#ifndef QUANTIS_EXTRACTOR_H
#define QUANTIS_EXTRACTOR_H

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

#ifdef _MSC_VER
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;

typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;

typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;

#else
#include <stdint.h>
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "DllMain.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * List of errors for the Quantis.
   */
  DLL_EXPORT typedef enum {
    /** Unable to allocate memory */
    QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY = -10,

    /** Extractor file not found */
    QUANTIS_EXT_ERROR_MATRIX_FILE_NOT_FOUND = -11,

    /** Unable to read file */
    QUANTIS_EXT_ERROR_UNABLE_TO_READ_FILE = -12,

    /** Unable to open file */
    QUANTIS_EXT_ERROR_UNABLE_TO_OPEN_FILE = -13,

    /** Unable to write file */
    QUANTIS_EXT_ERROR_UNABLE_TO_WRITE_FILE = -14,

    /** Matrix file is too small for requested matrix size */
    QUANTIS_EXT_ERROR_MATRIX_FILE_TOO_SMALL = -15,

    /** Matrix file is too small for requested matrix size */
    QUANTIS_EXT_ERROR_READ_SIZE_DIFFER_FROM_REQUESTED_SIZE = -16,

    /** Error in the creation of the seed */
    QUANTIS_EXT_ERROR_SEED_CREATION_FAILURE = -17,

    /** Error in sampled read, while creating the seed */
    QUANTIS_EXT_ERROR_SAMPLED_READ = -18,

    /** Error in sampled read, while creating the seed */
    QUANTIS_EXT_ERROR_VON_NEUMANN = -19,

    /** Error in the randomness extraction process */
    QUANTIS_EXT_ERROR_EXTRACTION_FAILED = -20,

    /** The passed extractor parameters (number of input and output bits, n and k respectively) 
     * are not consistent (they should be multiples of 64 bits and n>k) */
    QUANTIS_EXT_ERROR_WRONG_EXTRACTION_PARAMETERS = -21,

    /** Error in the storage buffer management */
    QUANTIS_EXT_ERROR_NOT_ENOUGH_BYTES_IN_STORAGE_BUFFER = -22,

    /** input buffer provided to QuantisExtProcessBufferTroyerRenner does not contain enough bytes */
    QUANTIS_EXT_ERROR_NOT_ENOUGH_INPUT_BYTES = -23,

    /** trying to access storage buffer, but storage buffer was not enabled*/
    QUANTIS_EXT_ERROR_STORAGE_BUFFER_DISABLED = -24,

    /** less than 1 elementary matrix was provided to QuantisExtCreateExtractorMatrix */
    QUANTIS_EXT_ERROR_NOT_ENOUGH_INPUT_ELEMENTARY_MATRICES = -25

  } QuantisExtractorError;

#define QUANTIS_EXTRACTOR_LIBRARY_VERSION 20.2f

  /**
   * Returns the library version as a number composed by the major
   * and minor number: <code>version = major.minor</code>
   * @return the library version.
   */
  float QuantisExtractorGetLibVersion();

  /**
   * Reads the extractor matrix from the specified file and store in memory
   * @param matrixFilename the filename of the matrix
   * @param extractorMatrix pointer to the pointer the buffer where to store the extractor matrix
   * @param matrixSizeIn the number of bits which are input to the extractor
   * @param matrixSizeOut the number of bits which are output to the extractor
   * @return QUANTIS_SUCCESS if success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorInitializeMatrix(const char *matrixFilename,
                                                      uint64_t **extractorMatrix,
                                                      uint16_t matrixSizeIn,
                                                      uint16_t matrixSizeOut);

  /**
   * Free the memory of the extractor matrix
   * @param extractorMatrix pointer to the pointer to the buffer where to store the extractor matrix
   */
  DLL_EXPORT void QuantisExtractorUninitializeMatrix(uint64_t **extractorMatrix);

  /**
   * Reads random data from the Quantis device and apply the extractor post-processing.
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param outputBuffer a pointer to a destination buffer. This buffer MUST 
   * already be allocated. Its size must be at least of "numberOfBytesRequested" bytes.
   * @param numberOfBytesRequested the number of bytes to read (not larger than QUANTIS_MAX_READ_SIZE).
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return The number of read bytes on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorGetDataFromQuantis(QuantisDeviceType deviceType,
                                                        unsigned int deviceNumber,
                                                        uint8_t *outputBuffer,
                                                        uint32_t numberOfBytesRequested,
                                                        const uint64_t *extractorMatrix);

  /**
   * The function applied the extraction to the input file and stores the processed data into the output filename
   * @param inputFilePath string containing the path of the input file
   * @param outputFilePath string containing the path of the output file 
   * @param extractorMatrix pointer the extractor matrix (uint64_t for multiplication efficiency)
   * @return number of bytes written to file on success, QUANTIS_EXT_ERROR on failure
   */
  DLL_EXPORT int32_t QuantisExtractorGetDataFromFile(char *inputFilePath,
                                                     char *outputFilePath,
                                                     const uint64_t *extractorMatrix);

  /**
   * The function applies the extraction post processing to the input buffer, with the specified extractor matrix with given parameters
   * and store the processed buffer into outputBuffer
   * @param inputBuffer pointer to the buffer containing the bytes to be processed
   * @param outputBuffer pointer to the buffer where the result of the extraction should be stored (should be already allocated)
   * @param extractorMatrix pointer the extractor matrix (uint64_t for multiplication efficiency)
   * @param numberOfBytesAfterExtraction the number of processed bytes which should be produced.
   */
  DLL_EXPORT void QuantisExtractorGetDataFromBuffer(const uint8_t *inputBuffer,
                                                    uint8_t *outputBuffer,
                                                    const uint64_t *extractorMatrix,
                                                    uint32_t numberOfBytesAfterExtraction);

  /**
   * The function gives the size of the matrix in value (aka n) of the extraction
   * @return  the BitsIn value
   */
  DLL_EXPORT uint16_t QuantisExtractorGetMatrixSizeIn();

  /**
   * The function gives the size of the matrix out value (aka k) of the extraction
   * @return  the BitsOut value
   */
  DLL_EXPORT uint16_t QuantisExtractorGetMatrixSizeOut();

  /**
   * Get the parameters for reading the bytes to perform Troyer-Renner extraction
   * @param numberOfBytesRequested the number of bytes to read (not larger than QUANTIS_MAX_READ_SIZE).
   * @param numberOfBytesAfterExtraction the number of bytes that will be output after the extraction
   * @param numberOfBytesBeforeExtraction 
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
  */
  DLL_EXPORT int32_t QuantisExtractorComputeBufferSize(uint32_t numberOfBytesRequested,
                                                       uint32_t *numberOfBytesAfterExtraction,
                                                       uint32_t *numberOfBytesBeforeExtraction);

  /**
   * The function initializes an output buffer in order to contain the result of the extraction of an input buffer of inputBufferLength bytes
   * @param inputBufferSize number of bytes contained in the input buffer
   * @param outputBuffer  pointer to the pointer to the buffer which should be initialized
   * @return the number of bytes that should be obtained after the extraction, a QUANTIS_EXT_ERROR otherwise
   */
  DLL_EXPORT int32_t QuantisExtractorInitializeOutputBuffer(uint32_t inputBufferSize,
                                                            uint8_t **outputBuffer);

  /**
   * The function frees the memory eventually allocated for outputBuffer
   * @param outputBuffer pointer to the pointer to the buffer which has to be freed
   */
  DLL_EXPORT void QuantisExtractorUnitializeOutputBuffer(uint8_t **outputBuffer);

  /**
   * Apply extraction processing function to a block of n bits and produce k processed bits 
   * (n and k global variables specifying the parameters of the extractor)
   * The function consists of a matrix multiplication: 
   *    [output] (k x 1) = {extractor} (k x n) * [input] (n x 1)
   * @param inputBuffer input of the extractor (uint64_t for multiplication efficiency)
   * @param outputBuffer output of the extractor (uint64_t for multiplication efficiency)
   * @param extractorMatrix pointer to the extractor matrix (uint64_t for multiplication efficiency)
   */
  DLL_EXPORT void QuantisExtractorProcessBlock(const uint64_t *inputBuffer,
                                               uint64_t *outputBuffer,
                                               const uint64_t *extractorMatrix);

  /** 
   * XOR the bitstreams contained in the nbrElementaryMatrices files specified in elementaryMatricesToXor
   * @param nbrElementaryMatrices   number of elementary matrices that should be XORed
   * @param nbrBytesToXor number of bytes that should be XORed
   * @param elementaryMatricesFilenames char array containing the name of the files where the different elementary matrices are stored
   * @param extractorMatrixFilename path of the file where the final extractor matrix should be saved
   * @return QUANTIS_SUCCESS on success, QUANTIS_EXT_ERROR on failure
   */
  DLL_EXPORT int32_t QuantisExtractorMatrixCreate(uint32_t nbrElementaryMatrices,
                                                  uint32_t nbrBytesToXor,
                                                  char *elementaryMatricesFilenames[],
                                                  char *extractorMatrixFilename);

  /**
   * Write to file an elementary matrix created by applying QuantisExtVonNeumannProcess to the buffer produced by QuantisExtSampledRead
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param matrixSizeIn the number of bits which are input to the extractor
   * @param matrixSizeOut the number of bits which are output to the extractor
   * @param underSamplingPeriod number of bytes to down sample. should be >= 13
   * @param elementaryMatrixFilename path of the file where the seed should be written
   * @return QUANTIS_SUCCESS if success, QUANTIS_EXT_ERROR otherwise
   */
  DLL_EXPORT int32_t QuantisExtractorMatrixCreateElementary(QuantisDeviceType deviceType,
                                                            unsigned int deviceNumber,
                                                            uint16_t matrixSizeIn,
                                                            uint16_t matrixSizeOut,
                                                            uint16_t underSamplingPeriod,
                                                            char *elementaryMatrixFilename);

  /**
   * Sample the QuantisRead output according to samplingRate and save the output stream in sampledBuffer
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param nbrOfBytesRequested number of bytes which should be output to the user after the sampling 
   *                               of the data read from the Quantis
   * @param underSamplingPeriod number of bytes to down sampled. should >= 13
   * @param sampledBuffer buffer where to store the sampled sequence
   * @return number of read sampled bytes if success, QUANTIS_EXT_ERROR otherwise
   */
  DLL_EXPORT int32_t QuantisExtractorMatrixUnderSamplingRead(QuantisDeviceType deviceType,
                                                             unsigned int deviceNumber,
                                                             uint32_t nbrOfBytesRequested,
                                                             uint16_t underSamplingPeriod,
                                                             uint8_t *sampledBuffer);

  /**
   * Apply the Von Neumann post-processing to the bit sequence which was read from the inputFile and write the processed sequence into the output buffer
   * @param *inputBuffer input buffer to be processed
   * @param *outputBuffer output buffer which will contain the processed bytes. The buffer should be already initialized by the function which invokes this method; please note that the number of bytes that will be output by the Von Neumann processing is not deterministic, as it depends on the input sequence, but it can never exceed 1/2 of the input buffer size.
   * @param inputBufferSize in bytes
   * @return number of bytes at the output of the Von Neumann processing
   */
  DLL_EXPORT uint32_t QuantisExtractorMatrixProcessBufferVonNeumann(uint8_t *inputBuffer,
                                                                    uint8_t *outputBuffer,
                                                                    uint32_t inputBufferSize);

  /** Enable the storage buffer and allocate MAX_BUFFER_SIZE bytes for it
   * @return QUANTIS_SUCCESS if enabling is successful, QUANTIS_EXT_ERROR otherwise
  */
  DLL_EXPORT int32_t QuantisExtractorStorageBufferEnable();

  /** Disable the storage buffer (if it was previously activated) and free the correspondigly allocated memory
   * @return QUANTIS_SUCCESS if enabling is successful, QUANTIS_EXT_ERROR otherwise
  */
  DLL_EXPORT int32_t QuantisExtractorStorageBufferDisable();

  /** Reset the storage buffer (free and reallocate memory)
   * @return QUANTIS_SUCCESS if reset is successful, QUANTIS_EXT_ERROR otherwise
  */
  DLL_EXPORT int32_t QuantisExtractorStorageBufferClear();

  /** Set the first 'bytesToCopy' bytes of the storage buffer to the 'bytesToCopy' bytes pointed by bufferToCopy
  * Any data previously written in the storage buffer will be overwritten
  * If the buffer to copy is bigger than MAX_STORAGE_BUFFER_SIZE, extra data will be dropped
  * @param bufferToCopy pointer to the buffer which should be copied into the storage buffer
  * @param bytesToCopy number of bytes in bufferToCopy to be copied into the storage buffer
  * @return always QUANTIS_SUCCESS 
  */
  DLL_EXPORT int32_t QuantisExtractorStorageBufferSet(uint8_t *bufferToCopy,
                                                      uint32_t bytesToCopy);

  /** Append 'bytesToAppend' bytes of the bufferToCopy to the storage buffer
   * Existing data in the storage buffer will be preserved
   * Please note that appendable bytes are limited by MAX_STORAGE_BUFFER_SIZE, i.e., bytes are appended as long as the storage buffer is not full.
   * @param bufferToCopy pointer to the buffer which should be copied into the storage buffer
   * @param bytesToCopy number of bytes in bufferToCopy to be copied into the storage buffer
   * @return the number of actually appended bytes .
  */
  DLL_EXPORT int32_t QuantisExtractorStorageBufferAppend(uint8_t *bufferToAppend,
                                                         uint32_t bytesToAppend);

  /** Read numberOfBytesRequested bytes from the storage buffer
   * @param outputBuffer pointer to the buffer where the read bytes should be written
   * @param numberOfBytesRequested  number of bytes to read
   * @return QUANTIS_SUCCESS if reading is successful, QUANTIS_EXT_ERROR otherwise
  */
  DLL_EXPORT int32_t QuantisExtractorStorageBufferRead(uint8_t *outputBuffer,
                                                       uint32_t numberOfBytesRequested);

  /** Return the number of bytes which has been written into the buffer
   * @return the storage buffer size in bytes
  */
  DLL_EXPORT uint32_t QuantisExtractorStorageBufferGetSize();

  /** Return the state (enabled/disabled) of the storage buffer
   * @return bufferEnabled (0 = disabled, 1 = enabled)
  */
  DLL_EXPORT uint8_t QuantisExtractorStorageBufferIsEnabled();

  /** ----------------------------------------------------------------------------------- */
  /**                                       READ OPTIONS                                  */
  /** ----------------------------------------------------------------------------------- */

  /**
   * Reads a random double floating precision value between 0.0 (inclusive)
   * and 1.0 (exclusive) from the Quantis device.
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param value a pointer to a destination value.
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorReadDouble_01(QuantisDeviceType deviceType,
                                                   unsigned int deviceNumber,
                                                   double *value,
                                                   const uint64_t *extractorMatrix);

  /**
   * Reads a random single floating precision value between 0.0 (inclusive)
   * and 1.0 (exclusive) from the Quantis device.
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param value a pointer to a destination value.
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorReadFloat_01(QuantisDeviceType deviceType,
                                                  unsigned int deviceNumber,
                                                  float *value,
                                                  const uint64_t *extractorMatrix);

  /**
   * Reads a random number from the Quantis device.
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param value a pointer to a destination value.
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorReadInt(QuantisDeviceType deviceType,
                                             unsigned int deviceNumber,
                                             int *value,
                                             const uint64_t *extractorMatrix);

  /**
   * Reads a random number from the Quantis device.
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param value a pointer to a destination value.
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorReadShort(QuantisDeviceType deviceType,
                                               unsigned int deviceNumber,
                                               short *value,
                                               const uint64_t *extractorMatrix);

  /**
   * Reads a random number from the Quantis device and scale it to be between 
   * min (inclusive) and max (exclusive).
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param value a pointer to a destination value.
   * @param min the minimal value the random number can take.
   * @param max the maximal value the random number can take.
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorReadScaledDouble(QuantisDeviceType deviceType,
                                                      unsigned int deviceNumber,
                                                      double *value,
                                                      double min,
                                                      double max,
                                                      const uint64_t *extractorMatrix);

  /**
   * Reads a random number from the Quantis device and scale it to be between
   * min (inclusive) and max (exclusive).
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param value a pointer to a destination value.
   * @param min the minimal value the random number can take.
   * @param max the maximal value the random number can take.
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorReadScaledFloat(QuantisDeviceType deviceType,
                                                     unsigned int deviceNumber,
                                                     float *value,
                                                     float min,
                                                     float max,
                                                     const uint64_t *extractorMatrix);

  /**
   * Reads a random number from the Quantis device and scale it to be between
   * min and max (inclusive).
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param value a pointer to a destination value.
   * @param min the minimal value the random number can take.
   * @param max the maximal value the random number can take.
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorReadScaledInt(QuantisDeviceType deviceType,
                                                   unsigned int deviceNumber,
                                                   int *value,
                                                   int min,
                                                   int max,
                                                   const uint64_t *extractorMatrix);

  /**
   * Reads a random number from the Quantis device and scale it to be between
   * min and max (inclusive).
   * @param deviceType specify the type of Quantis device.
   * @param deviceNumber the number of the Quantis device.
   * @param value a pointer to a destination value.
   * @param min the minimal value the random number can take.
   * @param max the maximal value the random number can take.
   * @param extractorMatrix pointer to the buffer where the extractor matrix has been stored (initialization should be done with QuantisExtensionsInitExtMatrix)
   * @return QUANTIS_SUCCESS on success or a QUANTIS_EXT_ERROR code on failure.
   */
  DLL_EXPORT int32_t QuantisExtractorReadScaledShort(QuantisDeviceType deviceType,
                                                     unsigned int deviceNumber,
                                                     short *value,
                                                     short min,
                                                     short max,
                                                     const uint64_t *extractorMatrix);

  /**
 * Get a pointer to the error message string for the QuantisExtensions library.
 *
 * This functions interprets the value of errorNumber and generates a string
 * describing the error.
 *
 * The returned pointer points to a statically allocated string, which shall
 * not be modified by the program. Further calls to this function will
 * overwrite its content.
 *
 * @param errorNumber The error number.
 * @return A pointer to the error message string.
*/
  DLL_EXPORT char *QuantisExtractorStrError(QuantisExtractorError errorNumber);

#ifdef __cplusplus
}
#endif

#endif /* QUANTIS_EXTENSIONS_H_H */
