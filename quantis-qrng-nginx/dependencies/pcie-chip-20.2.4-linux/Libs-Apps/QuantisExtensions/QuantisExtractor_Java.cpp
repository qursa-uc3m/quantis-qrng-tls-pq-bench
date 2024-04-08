/*
 * Quantis Extensions Java Library
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

// MUST be included before #ifndef!
#include "QuantisExtractor_Java.h"

#ifndef DISABLE_QUANTIS_JAVA

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <cstdlib>
#endif
#include <iostream>
#include <iomanip>
#include "QuantisExtractor.hpp"

// ---------------------- Global, protected variable ----------------------

idQ::QuantisExtractor g_qextr;

// ---------------------- Global, protected functions ----------------------

/**
 * This method throws a Java exception. This method is not public.
 */
static void _ThrowJavaException(JNIEnv *env, const char *msg)
{
  jclass quantisExceptionClass = env->FindClass("com/idquantique/quantisextensions/QuantisExtractorException");
  if (quantisExceptionClass == NULL)
  {
    // Unable to find the Quantis' exception class, trying with general Exception class.
    quantisExceptionClass = env->FindClass("java/lang/Exception");
    if (quantisExceptionClass == NULL)
    {
      // Unable to find the exception class, give up.
      return;
    }
  }

  env->ThrowNew(quantisExceptionClass, msg);
}

/**
 * This method check the device number for correctness. This method is not public.
 */
static void _CheckDeviceNumber(JNIEnv *env, jint jDeviceNumber)
{
  int deviceNumber = static_cast<int>(jDeviceNumber);
  if (deviceNumber < 0)
  {
    // Invalid device number, raise an error
    _ThrowJavaException(env, QuantisStrError(QUANTIS_ERROR_INVALID_DEVICE_NUMBER));
  }
}

/**
 * This method check the result of a CPP-call for correctness. This method is not public.
 */
static void _CheckResult(JNIEnv *env, int result)
{
  if (result >= 0)
  {
    // There is no error
    return;
  }

  QuantisError errorNumber = static_cast<QuantisError>(result);
  _ThrowJavaException(env, QuantisStrError(errorNumber));
}

// ----------------------- JNI functions implementation -----------------------

/**
 * Namespace: com.idquantique.quantisextensions
 * Class:     QuantisExtractor
 * Method:    JniGetLibVersion
 */
JNIEXPORT jfloat Java_com_idquantique_quantisextensions_QuantisExtractor_JniGetLibVersion(
    JNIEnv * /*env*/,
    jclass /*obj*/)
{
  // Perform request
  return static_cast<jfloat>(idQ::QuantisExtractor::GetLibVersion());
}

/**
 * Namespace: com.idquantique.quantisextensions
 * Class:     QuantisExtractor
 * Method:    JniInitializeMatrix
 */
JNIEXPORT void Java_com_idquantique_quantisextensions_QuantisExtractor_JniInitializeMatrix(
    JNIEnv *env,
    jclass /*obj*/,
    jstring jMatrixFileName,
    jshort jMatrixSizeIn,
    jshort jMatrixSizeOut)
{
  //Convert types
  const char *nMatrixFileName = env->GetStringUTFChars(jMatrixFileName, 0);

  try
  {
    // Perform request
    g_qextr.InitializeMatrix(nMatrixFileName, jMatrixSizeIn, jMatrixSizeOut);
  }
  catch (std::runtime_error &std_exc)
  {
    env->ReleaseStringUTFChars(jMatrixFileName, nMatrixFileName);
    _ThrowJavaException(env, std_exc.what());
  }
  catch (...)
  {
    env->ReleaseStringUTFChars(jMatrixFileName, nMatrixFileName);
    _ThrowJavaException(env, "Unknown Exception in JniInitializeMatrix.");
  }
  env->ReleaseStringUTFChars(jMatrixFileName, nMatrixFileName);
}

/**
 * Namespace: com.idquantique.quantisextensions
 * Class:     QuantisExtractor
 * Method:    JniGetDataFromQuantis
 */
JNIEXPORT void Java_com_idquantique_quantisextensions_QuantisExtractor_JniGetDataFromQuantis(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jint jSizeBytes,
    jbyteArray jOutputBuffer)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);
  int sizeBytes = static_cast<int>(jSizeBytes);
  if (sizeBytes <= 0)
  {
    // Invalid device number, raise an error
    _CheckResult(env, QUANTIS_ERROR_INVALID_READ_SIZE);
    return;
  }

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Allocate buffer's memory
  char *outputBuffer = (char *)malloc(sizeBytes);
  if (outputBuffer == NULL)
  {
    _ThrowJavaException(env, QuantisStrError(QUANTIS_ERROR_NO_MEMORY));
    return;
  }

  // Perform request
  try
  {
    // Perform request
    g_qextr.GetDataFromQuantis(deviceType, deviceNumber, outputBuffer, sizeBytes);
    env->SetByteArrayRegion(jOutputBuffer, 0, jSizeBytes, (jbyte *)outputBuffer);
    free(outputBuffer);
  }
  catch (std::runtime_error &std_exc)
  {
    free(outputBuffer);
    _ThrowJavaException(env, std_exc.what());
  }
  catch (...)
  {
    free(outputBuffer);
    _ThrowJavaException(env, "Unknown Exception in JniGetDataFromQuantis.");
  }
}

/**
 * Namespace: com.idquantique.quantisextensions
 * Class:     QuantisExtractor
 * Method:    JniUninitializeMatrix
 */
JNIEXPORT void Java_com_idquantique_quantisextensions_QuantisExtractor_JniUninitializeMatrix(
    JNIEnv * /*env*/,
    jclass /*obj*/)
{
  // Perform request
  g_qextr.UninitializeMatrix();
}

#else
int unused; /* Silence `ISO C forbids an empty translation unit' warning.  */
#endif /* DISABLE_QUANTIS_JAVA */
