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

#include "../Quantis/Quantis.h"
#include "QuantisExtractor.h"
#include "../Quantis/Conversion.h"
#include <stdio.h>
#include <math.h>
#include <malloc.h>

/* Size of the buffer used for QuantisReadXXX methods */
#define QUANTIS_EXTENSIONS_READ_XXX_BUFFER_SIZE 8
/* Max size of the storage buffer */
#define MAX_STORAGE_BUFFER_SIZE 65535

// global variables

//used to fast the processing
uint16_t g_n = 0; // extractor: number of input bits
uint16_t g_k = 0; // extractor: number of output bits

uint8_t g_storageBufferEnabled = 0;
uint32_t g_storageBufferSize = 0;
uint8_t *g_storageBuffer = NULL;

float QuantisExtractorGetLibVersion()
{
  return QUANTIS_EXTRACTOR_LIBRARY_VERSION;
}

int32_t QuantisExtractorInitializeMatrix(const char *matrixFilename,
                                         uint64_t **extractorMatrix,
                                         uint16_t matrixSizeIn,
                                         uint16_t matrixSizeOut)
{
  int32_t result;
  FILE *extractorFileHandler;
  uint32_t elementsExtractorMatrix;

  elementsExtractorMatrix = matrixSizeIn * matrixSizeOut / 64;

  *extractorMatrix = malloc(elementsExtractorMatrix * sizeof(uint64_t));

  if (*extractorMatrix == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  // read the extractor file and write it to extractorMatrixTR
  extractorFileHandler = fopen(matrixFilename, "rb");
  if (!extractorFileHandler)
  {
    return QUANTIS_EXT_ERROR_MATRIX_FILE_NOT_FOUND;
  }

  // store the bits read from the extractor file as uint64_t into extractorMatrixTR
  result = (int32_t)fread(*extractorMatrix,
                          sizeof(uint64_t),
                          elementsExtractorMatrix,
                          extractorFileHandler);
  if (result < 0)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_READ_FILE;
  }
  else
  {
    if ((uint32_t)result != elementsExtractorMatrix)
    {
      return QUANTIS_EXT_ERROR_MATRIX_FILE_TOO_SMALL;
    }
  }

  fclose(extractorFileHandler);

  //Store the the matrix size in global var to fast the processing
  g_n = matrixSizeIn;
  g_k = matrixSizeOut;

  return QUANTIS_SUCCESS;
}

void QuantisExtractorUninitializeMatrix(uint64_t **extractorMatrix)
{
  if (*extractorMatrix)
  {
    free(*extractorMatrix);
  }
}

int32_t QuantisExtractorGetDataFromQuantis(QuantisDeviceType deviceType,
                                           unsigned int deviceNumber,
                                           uint8_t *outputBuffer,
                                           uint32_t numberOfBytesRequested,
                                           const uint64_t *extractorMatrix)
{
  int32_t result;

  uint32_t numberOfBytesAfterExtraction;  // overall number of bytes that are output to the extractor
                                          // (integer multiple of extractorBitsOut/8)
  uint32_t numberOfBytesBeforeExtraction; // actual number of bytes which should be read from the Quantis device
                                          // in order to output the required numberOfBytesRequested bytes (depends
                                          // on the efficiency of the extractor)

  uint8_t *inputBufferExtractor;  // overall input buffer (which will contain the raw data read from the Quantis device)
  uint8_t *outputBufferExtractor; // overall output buffer, which will contain at least numberOfBytesRequested bytes, but
                                  // eventually more (due to the discreteness of the extractor, which works on blocks)

  uint32_t currentOutputSize = 0; // number of bytes that were written to the output buffer

  // read raw output of the Quantis and process it (extraction process is blockwise)

  int32_t localStorageBufferEnabled = QuantisExtractorStorageBufferIsEnabled();
  int32_t localStorageBufferSize = QuantisExtractorStorageBufferGetSize();

  uint32_t chunkSize;
  uint32_t remaining;
  uint32_t i;

  /* check correctness of the extractor parameters (extractorBitsIn and extractorBitsOut 
  should be multiples of 64 and extractorBitsIn > extractorBitsOut) */
  if (QuantisExtractorGetMatrixSizeIn() <= QuantisExtractorGetMatrixSizeOut())
  {
    return QUANTIS_EXT_ERROR_WRONG_EXTRACTION_PARAMETERS;
  }
  if (QuantisExtractorGetMatrixSizeIn() % 64)
  {
    return QUANTIS_EXT_ERROR_WRONG_EXTRACTION_PARAMETERS;
  }
  if (QuantisExtractorGetMatrixSizeOut() % 64)
  {
    return QUANTIS_EXT_ERROR_WRONG_EXTRACTION_PARAMETERS;
  }

  // if the storageBuffer is enabled and not empty, copy its content into the output buffer and update buffer pointers accordingly
  if (localStorageBufferEnabled && localStorageBufferSize > 0)
  {
    if ((uint32_t)localStorageBufferSize < numberOfBytesRequested)
    {
      // read all the bytes written in the storageBuffer
      currentOutputSize = localStorageBufferSize;

      QuantisExtractorStorageBufferRead(&outputBuffer[0], localStorageBufferSize);

      numberOfBytesRequested = numberOfBytesRequested - currentOutputSize;
    }
    else // there are enough bytes in the storageBuffer in order not to perform a new QuantisRead
    {
      QuantisExtractorStorageBufferRead(&outputBuffer[0], numberOfBytesRequested);

      return numberOfBytesRequested;
    }
  }

  result = QuantisExtractorComputeBufferSize(numberOfBytesRequested,
                                             &numberOfBytesAfterExtraction,
                                             &numberOfBytesBeforeExtraction);

  if (result != QUANTIS_SUCCESS)
  {
    return result;
  }

  inputBufferExtractor = malloc(numberOfBytesBeforeExtraction);
  if (inputBufferExtractor == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  outputBufferExtractor = malloc(numberOfBytesAfterExtraction);
  if (outputBufferExtractor == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  // read "numberOfBytesRequested" bytes from the Quantis device
  result = QuantisRead(deviceType,
                       deviceNumber,
                       inputBufferExtractor,
                       numberOfBytesBeforeExtraction);

  chunkSize = 4096;
  remaining = numberOfBytesBeforeExtraction;
  i = 0;

  while (remaining > 0u)
  {
    /* Chunk size */
    if (remaining < chunkSize)
    {
      chunkSize = remaining;
    }

    result = QuantisRead(deviceType,
                         deviceNumber,
                         (void *)(&inputBufferExtractor[i++ * chunkSize]),
                         chunkSize);

    if (result < 0)
    {
      return result;
    }
    else if ((uint32_t)result != chunkSize)
    {
      return QUANTIS_EXT_ERROR_READ_SIZE_DIFFER_FROM_REQUESTED_SIZE;
    }

    remaining -= chunkSize;
  }

  QuantisExtractorGetDataFromBuffer(inputBufferExtractor,
                                    outputBufferExtractor,
                                    extractorMatrix,
                                    numberOfBytesAfterExtraction);

  // copy the number of bytes that were output by the extractor in order to provide all the bytes that the user required
  memcpy(&outputBuffer[currentOutputSize], &outputBufferExtractor[0], numberOfBytesRequested);
  currentOutputSize += numberOfBytesRequested;

  if (localStorageBufferEnabled && numberOfBytesAfterExtraction > numberOfBytesRequested)
  {
    QuantisExtractorStorageBufferAppend(&outputBufferExtractor[numberOfBytesRequested],
                                        numberOfBytesAfterExtraction - numberOfBytesRequested);
  }

  free(inputBufferExtractor);
  free(outputBufferExtractor);

  return currentOutputSize;
}

int32_t QuantisExtractorGetDataFromFile(char *inputFilePath,
                                        char *outputFilePath,
                                        const uint64_t *extractorMatrix)
{
  int32_t result;          // variable for checking correctness of a function output
  FILE *inputFileHandler;  // handler for the input file
  FILE *outputFileHandler; // handler for the output file
  uint32_t inputFileSize;  // number of bytes contained in the input file

  uint8_t *inputBuffer;  // buffer containing the input bytes to be processed
  uint8_t *outputBuffer; // buffer containing the output bytes that were processed

  uint32_t numberOfBytesAfterExtraction; // number of bytes that has to be produced by the extractor

  // ---------------------------------------------------------------------------------//
  //                      FILE HANDLING AND BUFFER ALLOCATION                         //
  // ---------------------------------------------------------------------------------//

  inputFileHandler = fopen(inputFilePath, "rb");

  if (inputFileHandler == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_OPEN_FILE;
  }

  outputFileHandler = fopen(outputFilePath, "wb");

  if (outputFileHandler == NULL)
  {
    fclose(inputFileHandler);
    return QUANTIS_EXT_ERROR_UNABLE_TO_OPEN_FILE;
  }

  // Get input file size
  fseek(inputFileHandler, 0, SEEK_END);
  inputFileSize = ftell(inputFileHandler);
  rewind(inputFileHandler);

  // Allocate memory to contain the whole input file
  inputBuffer = malloc(inputFileSize);
  if (inputBuffer == NULL)
  {
    fclose(inputFileHandler);
    fclose(outputFileHandler);
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  // Copy the input file into the input buffer
  result = (int32_t)fread(inputBuffer, 1, inputFileSize, inputFileHandler);
  if (result != (int32_t)inputFileSize)
  {
    fclose(inputFileHandler);
    fclose(outputFileHandler);
    return QUANTIS_EXT_ERROR_READ_SIZE_DIFFER_FROM_REQUESTED_SIZE;
  }

  // initialize the output buffer and get the number of blocks to process (or a QUANTIS_ERROR code)
  result = QuantisExtractorInitializeOutputBuffer(inputFileSize, &outputBuffer);

  if (result < 0)
  {
    fclose(inputFileHandler);
    fclose(outputFileHandler);
    return result;
  }
  else
  {
    numberOfBytesAfterExtraction = (uint32_t)result;
  }

  // apply the Troyer-Renner post-processing to the input buffer and store the result into the output buffer
  QuantisExtractorGetDataFromBuffer(inputBuffer,
                                    outputBuffer,
                                    extractorMatrix,
                                    numberOfBytesAfterExtraction);

  // write the processed bytes to file
  result = (int32_t)fwrite(outputBuffer, numberOfBytesAfterExtraction, sizeof(uint8_t), outputFileHandler);

  if (result < 0)
  {
    fclose(inputFileHandler);
    fclose(outputFileHandler);
    return QUANTIS_EXT_ERROR_UNABLE_TO_WRITE_FILE;
  }

  QuantisExtractorUnitializeOutputBuffer(&outputBuffer);

  fclose(inputFileHandler);
  fclose(outputFileHandler);

  return numberOfBytesAfterExtraction;
}

void QuantisExtractorGetDataFromBuffer(const uint8_t *inputBuffer,
                                       uint8_t *outputBuffer,
                                       const uint64_t *extractorMatrix,
                                       uint32_t numberOfBytesAfterExtraction)

{
  uint32_t i;
  uint32_t elementsExtractorInput = QuantisExtractorGetMatrixSizeIn() / 64;
  uint32_t elementsExtractorOutput = QuantisExtractorGetMatrixSizeOut() / 64;

  uint64_t *inputBuffer64 = (uint64_t *)inputBuffer;
  uint64_t *outputBuffer64 = (uint64_t *)outputBuffer;

  uint32_t numberOfBlocksToProcess = numberOfBytesAfterExtraction / (QuantisExtractorGetMatrixSizeOut() / 8);

  // apply post-processing to the read block
  for (i = 0; i < numberOfBlocksToProcess; i++)
  {
    // perform the extraction on the current chunk of the input buffer and store the result in the output buffer
    QuantisExtractorProcessBlock(&inputBuffer64[i * elementsExtractorInput],
                                 &outputBuffer64[i * elementsExtractorOutput],
                                 extractorMatrix);
  }
}

/** ----------------------------------------------------------------------------------- */
/**                                LOWER LEVEL FUNCTIONS                                */
/** ----------------------------------------------------------------------------------- */

uint16_t QuantisExtractorGetMatrixSizeIn()
{
  return (g_n);
}

uint16_t QuantisExtractorGetMatrixSizeOut()
{
  return (g_k);
}

int32_t QuantisExtractorComputeBufferSize(uint32_t numberOfBytesRequested,
                                          uint32_t *numberOfBytesAfterExtraction,
                                          uint32_t *numberOfBytesBeforeExtraction)
{
  uint32_t numberOfBlocksToProcess;

  if (QuantisExtractorGetMatrixSizeOut() == 0)
  {
    return QUANTIS_EXT_ERROR_WRONG_EXTRACTION_PARAMETERS;
  }

  numberOfBlocksToProcess = (uint32_t)ceil((double)(numberOfBytesRequested * 8) / QuantisExtractorGetMatrixSizeOut());
  *numberOfBytesAfterExtraction = numberOfBlocksToProcess * (QuantisExtractorGetMatrixSizeOut() / 8);
  *numberOfBytesBeforeExtraction = numberOfBlocksToProcess * (QuantisExtractorGetMatrixSizeIn() / 64) * 8;

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorInitializeOutputBuffer(uint32_t inputBufferSize,
                                               uint8_t **outputBuffer)
{
  uint32_t numberOfBlocksToProcess;
  uint32_t numberOfBytesAfterExtraction;

  uint16_t extractorBytesIn = (QuantisExtractorGetMatrixSizeIn() / 8);
  uint16_t extractorBytesOut = (QuantisExtractorGetMatrixSizeOut() / 8);

  if ((inputBufferSize / extractorBytesIn < 1))
  {
    return QUANTIS_EXT_ERROR_NOT_ENOUGH_INPUT_BYTES;
  }

  // compute the number of processable blocks in which the input buffer can be segmented
  numberOfBlocksToProcess = (uint32_t)floor(inputBufferSize / extractorBytesIn);
  // compute the number of bytes which will be output by the extractor
  numberOfBytesAfterExtraction = numberOfBlocksToProcess * extractorBytesOut;

  // allocate the output buffer
  *outputBuffer = malloc(numberOfBytesAfterExtraction);

  if (*outputBuffer == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  return (int32_t)numberOfBytesAfterExtraction;
}

void QuantisExtractorUnitializeOutputBuffer(uint8_t **outputBuffer)
{
  if (*outputBuffer)
  {
    free(*outputBuffer);
  }
}

void QuantisExtractorProcessBlock(const uint64_t *inputBuffer,
                                  uint64_t *outputBuffer,
                                  const uint64_t *extractorMatrix)
{
  int index = 0;
  uint32_t i;
  unsigned int j;
  unsigned int l;

  // do a matrix-vector multiplication by looping over all rows
  // the outer loop over all words

  for (i = 0; i < (uint32_t)(g_k / 64); ++i)
  {
    outputBuffer[i] = 0;

    // the inner loop over all bits in the word
    for (j = 0; j < 64; ++j)
    {
      uint64_t parity = extractorMatrix[index++] & inputBuffer[0];

      // do it as a vector-vector multiplication using bit operations
      for (l = 1; l < (uint32_t)(g_n / 64); ++l)
      {
        parity ^= extractorMatrix[index++] & inputBuffer[l];
      }

      // finally count the bit parity
      parity ^= parity >> 1;
      parity ^= parity >> 2;
      parity = (parity & 0x1111111111111111UL) * 0x1111111111111111UL;
      // and set the j-th output bit of the i-th output word
      outputBuffer[i] |= ((parity >> 60) & 1) << j;
    }
  }
}

/** ----------------------------------------------------------------------------------- */
/**                              MATRIX CREATION                                        */
/** ----------------------------------------------------------------------------------- */

int32_t QuantisExtractorMatrixCreate(uint32_t nbrElementaryMatrices,
                                     uint32_t nbrBytesToXor,
                                     char *elementaryMatricesFilenames[],
                                     char *extractorMatrixFilename)
{
  int result;
  int returnValue;
  FILE *elementaryMatrixFileHandler;
  FILE *extractorMatrixFileHandler;
  uint32_t i;
  uint32_t j;
  //uint8_t extractorMatrix[nbrBytesToXor];    // buffer where the final seed is stored
  //uint8_t elementaryMatrix[nbrBytesToXor];  // buffer where the single seeds are stored once they have been read from file

  uint8_t *extractorMatrix;
  uint8_t *elementaryMatrix;

  if (nbrElementaryMatrices < 2)
  {
    return QUANTIS_EXT_ERROR_NOT_ENOUGH_INPUT_ELEMENTARY_MATRICES;
  }

  extractorMatrix = (uint8_t *)malloc(nbrBytesToXor); // buffer where the final seed is stored
  if (extractorMatrix == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  elementaryMatrix = (uint8_t *)malloc(nbrBytesToXor); // buffer where the single seeds are stored once they have been read from file
  if (elementaryMatrix == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  returnValue = QUANTIS_SUCCESS;

  // initialize the final seed to be the all-zero vector (XORing a bit with 0 gives the bit itself)
  memset(extractorMatrix, 0, nbrBytesToXor);

  for (i = 0; i < nbrElementaryMatrices; i++)
  {
    elementaryMatrixFileHandler = fopen(elementaryMatricesFilenames[i], "rb");

    if (elementaryMatrixFileHandler == NULL)
    {
      returnValue = QUANTIS_EXT_ERROR_MATRIX_FILE_NOT_FOUND;
      break;
    }

    // read the i-th partial seed
    result = (int32_t)fread(elementaryMatrix, sizeof(uint8_t), nbrBytesToXor, elementaryMatrixFileHandler);
    if (result < 0)
    {
      fclose(elementaryMatrixFileHandler);
      returnValue = QUANTIS_EXT_ERROR_UNABLE_TO_READ_FILE;
      break;
    }
    else
    {
      if ((uint32_t)result != nbrBytesToXor)
      {
        fclose(elementaryMatrixFileHandler);
        returnValue = QUANTIS_EXT_ERROR_READ_SIZE_DIFFER_FROM_REQUESTED_SIZE;
        break;
      }
    }

    fclose(elementaryMatrixFileHandler);

    // do the bit-wise xor byte per byte
    for (j = 0; j < nbrBytesToXor; j++)
    {
      extractorMatrix[j] ^= elementaryMatrix[j];
    }
  }

  //Write output file
  if (returnValue == QUANTIS_SUCCESS)
  {
    extractorMatrixFileHandler = fopen(extractorMatrixFilename, "wb");

    if (extractorMatrixFileHandler == NULL)
    {
      returnValue = QUANTIS_EXT_ERROR_UNABLE_TO_OPEN_FILE;
    }
    else
    {
      result = (int32_t)fwrite(extractorMatrix, 1, nbrBytesToXor, extractorMatrixFileHandler);

      if (result < 0)
      {
        fclose(extractorMatrixFileHandler);
        returnValue = QUANTIS_EXT_ERROR_UNABLE_TO_WRITE_FILE;
      }
      else
      {
        fclose(extractorMatrixFileHandler);
      }
    }
  }

  free(extractorMatrix);
  free(elementaryMatrix);

  return returnValue;
}

int32_t QuantisExtractorMatrixCreateElementary(QuantisDeviceType deviceType,
                                               unsigned int deviceNumber,
                                               uint16_t matrixSizeIn,
                                               uint16_t matrixSizeOut,
                                               uint16_t underSamplingPeriod,
                                               char *elementaryMatrixFilename)

{

  uint8_t *elementaryMatrixBuffer; // buffer containing the output elementary matrix
  uint32_t sizeOfElementaryMatrix; // size of the elementaryMatrixBuffer in bytes

  FILE *elementaryMatrixFileHandler; // handler where the elementaryMatrixBuffer is written

  uint32_t rawChunkSize = 32768; // size in bytes of the chunk which is read with each call to QuantisRead

  uint8_t *underSampledChunkBuffer;      // buffer holding a chunk of under sampled data
  int32_t sizeOfUnderSampledChunkBuffer; // size of underSampledChunkBuffer in bytes

  uint8_t *vonNeumannChunkBuffer;       // buffer holding a chunk of Von Neumann processed bytes
  uint32_t sizeOfVonNeumannChunkBuffer; // size of vonNeumannChunkBuffer in bytes

  uint32_t processedBytes; // number of bytes which were output (after under sampling and Von Neumann processing) up to now
  int32_t bytesToAppend;
  int result;

  processedBytes = 0;
  sizeOfElementaryMatrix = matrixSizeIn * matrixSizeOut / 8;

  elementaryMatrixBuffer = malloc(sizeOfElementaryMatrix);
  if (elementaryMatrixBuffer == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  while (processedBytes < sizeOfElementaryMatrix)
  {
    //read and undersample
    underSampledChunkBuffer = malloc(rawChunkSize / 8);
    if (underSampledChunkBuffer == NULL)
    {
      free(elementaryMatrixBuffer);
      return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
    }

    sizeOfUnderSampledChunkBuffer = QuantisExtractorMatrixUnderSamplingRead(deviceType, deviceNumber, rawChunkSize, underSamplingPeriod, underSampledChunkBuffer);

    if (sizeOfUnderSampledChunkBuffer < 0)
    {
      free(elementaryMatrixBuffer);
      free(underSampledChunkBuffer);
      return sizeOfUnderSampledChunkBuffer;
    }

    // resize tmpSampledBuffer in order to contain just the bytes that were output by QuantisExtensionsSampledRead
    underSampledChunkBuffer = realloc(underSampledChunkBuffer, sizeOfUnderSampledChunkBuffer);
    if (underSampledChunkBuffer == NULL)
    {
      free(elementaryMatrixBuffer);
      return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
    }

    // Perform von Neumann processing
    vonNeumannChunkBuffer = malloc(rawChunkSize / 16);
    if (vonNeumannChunkBuffer == NULL)
    {
      free(elementaryMatrixBuffer);
      free(underSampledChunkBuffer);
      return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
    }

    sizeOfVonNeumannChunkBuffer = QuantisExtractorMatrixProcessBufferVonNeumann(underSampledChunkBuffer, vonNeumannChunkBuffer, sizeOfUnderSampledChunkBuffer);

    // append the processed bytes to vonNeumannBuffer
    if (processedBytes + sizeOfVonNeumannChunkBuffer > sizeOfElementaryMatrix)
    {
      bytesToAppend = sizeOfElementaryMatrix - processedBytes;
    }
    else
    {
      bytesToAppend = sizeOfVonNeumannChunkBuffer;
    }

    // copy the newly processed bytes into the overall buffer which contains the output of the Von Neumann algorithm
    memcpy(&elementaryMatrixBuffer[processedBytes], vonNeumannChunkBuffer, bytesToAppend);

    // update the number of processed bytes
    processedBytes += (uint32_t)bytesToAppend;

    free(underSampledChunkBuffer);
    free(vonNeumannChunkBuffer);

  } // end while (processedBytes < sizeOfElementaryMatrix)

  // write the generated seed to file
  elementaryMatrixFileHandler = fopen(elementaryMatrixFilename, "wb");

  if (elementaryMatrixFileHandler == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_OPEN_FILE;
  }

  result = (int32_t)fwrite(elementaryMatrixBuffer, 1, sizeOfElementaryMatrix, elementaryMatrixFileHandler);

  free(elementaryMatrixBuffer);

  if (result < 0)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_WRITE_FILE;
  }

  fclose(elementaryMatrixFileHandler);

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorMatrixUnderSamplingRead(QuantisDeviceType deviceType,
                                                unsigned int deviceNumber,
                                                uint32_t nbrOfBytesRequested,
                                                uint16_t underSamplingPeriod,
                                                uint8_t *sampledBuffer)

{
  int result; // variable for checking the corrected execution of a called method
  //uint8_t sampledReadBuffer[nbrOfBytesRequested];    // buffer where the output of the quantis is stored
  uint8_t readByte;                // an output byte of the Quantis
  uint8_t storageByte;             // byte containing 8 bits obtained from 8 sampled Quantis bytes
                                   // (every 'samplingRate' bytes, the first bit of the read byte
                                   // is stored in storageByte)
  uint8_t mask;                    // mask for bitwise operations
  uint8_t bitCounter;              // count the number of bits written in storageByte
  int32_t byteCounter;             // count the number of bytes stored into sampledReadBuffer
  uint32_t sampledReadBufferIndex; // last read index of sampledReadBuffer

  uint8_t *sampledReadBuffer;

  sampledReadBuffer = (uint8_t *)malloc(nbrOfBytesRequested); // buffer where the final seed is stored
  if (sampledReadBuffer == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  // initialize variables
  storageByte = 0x00;
  bitCounter = 0;
  byteCounter = 0;
  sampledReadBufferIndex = 0;

  result = QuantisRead(deviceType, deviceNumber, sampledReadBuffer, nbrOfBytesRequested);

  if (result < 0)
  {
    free(sampledReadBuffer);
    return result;
  }
  else if ((uint32_t)result != nbrOfBytesRequested)
  {
    free(sampledReadBuffer);
    return QUANTIS_EXT_ERROR_READ_SIZE_DIFFER_FROM_REQUESTED_SIZE;
  }

  while (sampledReadBufferIndex * (underSamplingPeriod) < nbrOfBytesRequested)
  {
    // extract the bit[0] of the readBuffer, i.e., the first bit of the first byte
    readByte = sampledReadBuffer[sampledReadBufferIndex * (underSamplingPeriod)] & 0x01;

    // shift the extracted bit so that it can be written into the next position of storageByte
    mask = readByte << bitCounter;
    storageByte = storageByte | mask;

    // when all the 8 bits of storageByte are written, write storageByte to file
    if (bitCounter == 7)
    {
      sampledBuffer[byteCounter] = storageByte;
      byteCounter++;
      storageByte = 0x00;
      bitCounter = 0;
    }
    else
    {
      bitCounter++;
    }
    sampledReadBufferIndex++;
  }

  free(sampledReadBuffer);

  return byteCounter;
}

uint32_t QuantisExtractorMatrixProcessBufferVonNeumann(uint8_t *inputBuffer,
                                                       uint8_t *outputBuffer,
                                                       uint32_t inputBufferSize)
{
  uint8_t readByte;       // byte read from the sampled extractor file
  uint8_t vonNeumannByte; // byte obtained by applying the Von Neumann extraction algorithm
                          // to the sampled extractor bitstream
  uint8_t mask;           // mask used for bitwise operations
  uint8_t maskedByte;     // readByte masked with mask
  uint8_t tempByte;       // temporary byte used during the read-and-extract process
  uint8_t bitCounter;     // count the number of written bits in vonNeumannBytes
  uint32_t byteCounter;   // count the number of bytes written in outputBuffer

  uint32_t sampledReadBufferIndex; // last read index of sampledReadBuffer
  int i;

  // initialize variables
  vonNeumannByte = 0x00;
  bitCounter = 0;
  byteCounter = 0;
  sampledReadBufferIndex = 0;

  while (sampledReadBufferIndex < inputBufferSize)
  {
    readByte = inputBuffer[sampledReadBufferIndex];
    // process readByte in pairs (4x) and apply Von Neumann Extractor
    for (i = 0; i < 4; i++)
    {
      // i = 0 --> mask = 00000011
      // i = 1 --> mask = 00001100
      // i = 2 --> mask = 00110000
      // i = 3 --> mask = 11000000
      mask = 0x03 << 2 * i;
      // apply the mask to readByte
      maskedByte = (readByte & mask) >> 2 * i;

      // consider only pairs with a transition (01/10)
      if (maskedByte == 0x01 || maskedByte == 0x02)
      {
        if (maskedByte == 0x01)
        {
          // 0x01 ---> 1
          tempByte = 0x01;
        }
        else
        {
          // 0x02 ---> 0
          tempByte = 0x00;
        }
        // write the extracted bit in the next position of vonNeumannByte
        mask = tempByte << bitCounter;
        vonNeumannByte = vonNeumannByte | mask;
        // when a new vonNeumannByte is ready, write it to file and reset it
        if (bitCounter == 7)
        {
          outputBuffer[byteCounter] = vonNeumannByte;

          // reset vonNeumannByte
          vonNeumannByte = 0x00;
          bitCounter = 0;
          byteCounter++;
        }
        else
        {
          bitCounter++;
        } // end if (bitCounter == 7)
      }
    } // end for (int i = 0; i < 4; i++)

    sampledReadBufferIndex++;
  } // end while (byteCounter < vonNeumannSeedLength)

  return byteCounter;
}

/** ----------------------------------------------------------------------------------- */
/**                              STORAGE BUFFER MANAGEMENT                              */
/** ----------------------------------------------------------------------------------- */

int32_t QuantisExtractorStorageBufferEnable()
{
  g_storageBufferEnabled = 1;

  g_storageBuffer = malloc(MAX_STORAGE_BUFFER_SIZE);

  if (g_storageBuffer == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }
  g_storageBufferSize = 0;

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorStorageBufferDisable()
{
  if (!g_storageBufferEnabled)
  {
    return QUANTIS_EXT_ERROR_STORAGE_BUFFER_DISABLED;
  }

  g_storageBufferEnabled = 0;
  g_storageBufferSize = 0;
  free(g_storageBuffer);

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorStorageBufferClear()
{
  if (!g_storageBufferEnabled)
  {
    return QUANTIS_EXT_ERROR_STORAGE_BUFFER_DISABLED;
  }

  free(g_storageBuffer);

  g_storageBufferSize = 0;
  g_storageBuffer = malloc(MAX_STORAGE_BUFFER_SIZE);

  if (g_storageBuffer == NULL)
  {
    return QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY;
  }

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorStorageBufferSet(uint8_t *bufferToCopy,
                                         uint32_t bytesToCopy)
{
  if (!g_storageBufferEnabled)
  {
    return QUANTIS_EXT_ERROR_STORAGE_BUFFER_DISABLED;
  }

  if (bytesToCopy > MAX_STORAGE_BUFFER_SIZE)
  {
    bytesToCopy = MAX_STORAGE_BUFFER_SIZE;
  }

  memcpy(g_storageBuffer, bufferToCopy, bytesToCopy);
  g_storageBufferSize = bytesToCopy;

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorStorageBufferAppend(uint8_t *bufferToAppend,
                                            uint32_t bytesToAppend)
{
  if (!g_storageBufferEnabled)
  {
    return QUANTIS_EXT_ERROR_STORAGE_BUFFER_DISABLED;
  }

  if ((g_storageBufferSize + bytesToAppend) > MAX_STORAGE_BUFFER_SIZE)
  {
    bytesToAppend = MAX_STORAGE_BUFFER_SIZE - g_storageBufferSize;
  }

  memcpy(&g_storageBuffer[g_storageBufferSize], bufferToAppend, bytesToAppend);
  g_storageBufferSize += bytesToAppend;

  return bytesToAppend;
}

int32_t QuantisExtractorStorageBufferRead(uint8_t *outputBuffer,
                                          uint32_t numberOfBytesRequested)
{
  if (!g_storageBufferEnabled)
  {
    return QUANTIS_EXT_ERROR_STORAGE_BUFFER_DISABLED;
  }

  if (g_storageBufferSize < numberOfBytesRequested)
  {
    return QUANTIS_EXT_ERROR_NOT_ENOUGH_BYTES_IN_STORAGE_BUFFER;
  }

  memcpy(outputBuffer, g_storageBuffer, numberOfBytesRequested);
  g_storageBufferSize -= numberOfBytesRequested;

  // if there are still some bytes in the storageBuffer, shift them in the first positions of the array
  if (g_storageBufferSize)
  {
    QuantisExtractorStorageBufferSet(&g_storageBuffer[numberOfBytesRequested], g_storageBufferSize);
  }

  return QUANTIS_SUCCESS;
}

uint32_t QuantisExtractorStorageBufferGetSize()
{
  return g_storageBufferSize;
}

uint8_t QuantisExtractorStorageBufferIsEnabled()
{
  return g_storageBufferEnabled;
}

/** ----------------------------------------------------------------------------------- */
/**                                       READ OPTIONS                                  */
/** ----------------------------------------------------------------------------------- */

int32_t QuantisExtractorReadDouble_01(QuantisDeviceType deviceType,
                                      unsigned int deviceNumber,
                                      double *value,
                                      const uint64_t *extractorMatrix)
{
  int size = sizeof(*value);
  uint8_t buffer[QUANTIS_EXTENSIONS_READ_XXX_BUFFER_SIZE];

  int32_t result = QuantisExtractorGetDataFromQuantis(deviceType,
                                                      deviceNumber,
                                                      buffer,
                                                      size,
                                                      extractorMatrix);
  if (result < 0)
  {
    return result;
  }
  else if (result != size)
  {
    return QUANTIS_ERROR_IO;
  }

  *value = ConvertToDouble_01((char *)buffer);

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorReadFloat_01(QuantisDeviceType deviceType,
                                     unsigned int deviceNumber,
                                     float *value,
                                     const uint64_t *extractorMatrix)
{
  int size = sizeof(*value);
  uint8_t buffer[QUANTIS_EXTENSIONS_READ_XXX_BUFFER_SIZE];

  int32_t result = QuantisExtractorGetDataFromQuantis(deviceType,
                                                      deviceNumber,
                                                      buffer,
                                                      size,
                                                      extractorMatrix);
  if (result < 0)
  {
    return result;
  }
  else if (result != size)
  {
    return QUANTIS_ERROR_IO;
  }

  *value = ConvertToFloat_01((char *)buffer);

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorReadInt(QuantisDeviceType deviceType,
                                unsigned int deviceNumber,
                                int *value,
                                const uint64_t *extractorMatrix)
{
  int size = sizeof(*value);
  uint8_t buffer[QUANTIS_EXTENSIONS_READ_XXX_BUFFER_SIZE];

  int32_t result = QuantisExtractorGetDataFromQuantis(deviceType,
                                                      deviceNumber,
                                                      buffer,
                                                      size,
                                                      extractorMatrix);
  if (result < 0)
  {
    return result;
  }
  else if (result != size)
  {
    return QUANTIS_ERROR_IO;
  }

  *value = ConvertToInt((char *)buffer);

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorReadShort(QuantisDeviceType deviceType,
                                  unsigned int deviceNumber,
                                  short *value,
                                  const uint64_t *extractorMatrix)
{
  int size = sizeof(*value);
  uint8_t buffer[QUANTIS_EXTENSIONS_READ_XXX_BUFFER_SIZE];

  int32_t result = QuantisExtractorGetDataFromQuantis(deviceType,
                                                      deviceNumber,
                                                      buffer,
                                                      size,
                                                      extractorMatrix);
  if (result < 0)
  {
    return result;
  }
  else if (result != size)
  {
    return QUANTIS_ERROR_IO;
  }

  *value = ConvertToShort((char *)buffer);

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorReadScaledDouble(QuantisDeviceType deviceType,
                                         unsigned int deviceNumber,
                                         double *value,
                                         double min,
                                         double max,
                                         const uint64_t *extractorMatrix)
{
  double tmp;
  int32_t result;

  if (min > max)
  {
    return QUANTIS_ERROR_INVALID_PARAMETER;
  }

  result = QuantisExtractorReadDouble_01(deviceType,
                                         deviceNumber,
                                         &tmp,
                                         extractorMatrix);
  if (result != QUANTIS_SUCCESS)
  {
    return result;
  }

  *value = tmp * (max - min) + min;

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorReadScaledFloat(QuantisDeviceType deviceType,
                                        unsigned int deviceNumber,
                                        float *value,
                                        float min,
                                        float max,
                                        const uint64_t *extractorMatrix)
{
  float tmp;
  int32_t result;

  if (min > max)
  {
    return QUANTIS_ERROR_INVALID_PARAMETER;
  }

  result = QuantisExtractorReadFloat_01(deviceType,
                                        deviceNumber,
                                        &tmp,
                                        extractorMatrix);
  if (result != QUANTIS_SUCCESS)
  {
    return result;
  }

  *value = tmp * (max - min) + min;

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorReadScaledInt(QuantisDeviceType deviceType,
                                      unsigned int deviceNumber,
                                      int *value,
                                      int min,
                                      int max,
                                      const uint64_t *extractorMatrix)
{
  int tmp;
  int32_t result;

  const int BITS = sizeof(tmp) * 8;
  const unsigned long long RANGE = max - min + 1;
  const unsigned long long MAX_RANGE = 1ull << BITS;
  const unsigned long long LIMIT = MAX_RANGE - (MAX_RANGE % RANGE);

  if (min > max)
  {
    return QUANTIS_ERROR_INVALID_PARAMETER;
  }

  // Chooses the highest number that is the largest multiple of the output range
  // (discard values higher the output range)
  do
  {
    result = QuantisExtractorReadInt(deviceType,
                                     deviceNumber,
                                     &tmp,
                                     extractorMatrix);
    if (result != QUANTIS_SUCCESS)
    {
      return result;
    }
  } while ((tmp > 0) && ((unsigned long long)tmp >= LIMIT));

  *value = (tmp % RANGE) + min;

  return QUANTIS_SUCCESS;
}

int32_t QuantisExtractorReadScaledShort(QuantisDeviceType deviceType,
                                        unsigned int deviceNumber,
                                        short *value,
                                        short min,
                                        short max,
                                        const uint64_t *extractorMatrix)
{
  short tmp;
  int32_t result;
  const int BITS = sizeof(tmp) * 8;
  const unsigned int RANGE = max - min + 1;
  const unsigned int MAX_RANGE = 1u << BITS;
  const unsigned int LIMIT = MAX_RANGE - (MAX_RANGE % RANGE);

  if (min > max)
  {
    return QUANTIS_ERROR_INVALID_PARAMETER;
  }

  // Chooses the highest number that is the largest multiple of the output range
  // (discard values higher the output range)
  do
  {
    result = QuantisExtractorReadShort(deviceType,
                                       deviceNumber,
                                       &tmp,
                                       extractorMatrix);
    if (result != QUANTIS_SUCCESS)
    {
      return result;
    }
  } while ((tmp > 0) && ((unsigned int)tmp >= LIMIT));

  *value = (tmp % RANGE) + min;

  return QUANTIS_SUCCESS;
}

char *QuantisExtractorStrError(QuantisExtractorError errorNumber)
{
  char *msg = NULL;

  // Errors are listed alphabetically
  switch ((int)errorNumber)
  {
  case QUANTIS_ERROR_INVALID_DEVICE_NUMBER:
  case QUANTIS_ERROR_NO_DRIVER:
  case QUANTIS_ERROR_INVALID_PARAMETER:
  case QUANTIS_ERROR_INVALID_READ_SIZE:
  case QUANTIS_ERROR_IO:
  case QUANTIS_ERROR_NO_DEVICE:
  case QUANTIS_ERROR_NO_MEMORY:
  case QUANTIS_ERROR_NO_MODULE:
  case QUANTIS_ERROR_OPERATION_NOT_SUPPORTED:
  case QUANTIS_SUCCESS:
  case QUANTIS_ERROR_OTHER:
    msg = QuantisStrError((QuantisError)errorNumber);
    break;

  case QUANTIS_EXT_ERROR_UNABLE_TO_ALLOCATE_MEMORY:
    msg = "Unable to allocate memory";
    break;

  case QUANTIS_EXT_ERROR_MATRIX_FILE_NOT_FOUND:
    msg = "Extractor matrix file not found";
    break;

  case QUANTIS_EXT_ERROR_UNABLE_TO_READ_FILE:
    msg = "Unable to read file";
    break;

  case QUANTIS_EXT_ERROR_UNABLE_TO_OPEN_FILE:
    msg = "Unable to open file";
    break;

  case QUANTIS_EXT_ERROR_UNABLE_TO_WRITE_FILE:
    msg = "Unable to write file";
    break;

  case QUANTIS_EXT_ERROR_MATRIX_FILE_TOO_SMALL:
    msg = "Extractor matrix file is too small for the requested matrix size";
    break;

  case QUANTIS_EXT_ERROR_READ_SIZE_DIFFER_FROM_REQUESTED_SIZE:
    msg = "The size of the read differ from the requested size";
    break;

  case QUANTIS_EXT_ERROR_SEED_CREATION_FAILURE:
    msg = "Error in the creation of the seed";
    break;

  case QUANTIS_EXT_ERROR_SAMPLED_READ:
    msg = "Error in sampled read, while creating the seed";
    break;

  case QUANTIS_EXT_ERROR_VON_NEUMANN:
    msg = "Error in sampled read, while creating the seed";
    break;

  case QUANTIS_EXT_ERROR_EXTRACTION_FAILED:
    msg = "Error in the randomness extraction process";
    break;

  case QUANTIS_EXT_ERROR_WRONG_EXTRACTION_PARAMETERS:
    msg = "Extractor parameters (number of input and output bits, n and k respectively) are not consistent (they should be multiples of 64 bits and n>k)";
    break;

  case QUANTIS_EXT_ERROR_NOT_ENOUGH_BYTES_IN_STORAGE_BUFFER:
    msg = "Read failure: not enough bytes in the storage buffer";
    break;

  case QUANTIS_EXT_ERROR_NOT_ENOUGH_INPUT_BYTES:
    msg = "inputBufferSize/extractorBytesIn < 1 but at least one block should be input to the extractor";
    break;

  case QUANTIS_EXT_ERROR_STORAGE_BUFFER_DISABLED:
    msg = "trying to access storage buffer, but storage buffer was not enabled";
    break;

  case QUANTIS_EXT_ERROR_NOT_ENOUGH_INPUT_ELEMENTARY_MATRICES:
    msg = "less than 2 elementary matrices were provided as input, but at least 2 are required to produce an extractor matrix";
    break;

  default:
    sprintf(msg, "Undefined error: %d", (int)errorNumber);
    break;
  }

  return (char *)msg;
}
