/*
 * Quantis Java Library
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
#include "Quantis_Java.h"

#ifndef DISABLE_QUANTIS_JAVA

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <cstdlib>
#endif

#include "Quantis.h"

// ---------------------- Internal functions definitions ----------------------

static void _ThrowJavaException(JNIEnv *env, const char *msg)
{
  jclass quantisExceptionClass = env->FindClass("com/idquantique/quantis/QuantisException");
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
 *
 * @param env
 * @param result
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

JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisBoardReset(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  int result = QuantisBoardReset(deviceType, deviceNumber);
  _CheckResult(env, result);
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisCount(
    JNIEnv * /*env*/,
    jclass /*obj*/,
    jint jDeviceType)
{
  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);

  // Perform request
  return static_cast<jint>(QuantisCount(deviceType));
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetBoardVersion(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  int result = QuantisGetBoardVersion(deviceType, deviceNumber);
  _CheckResult(env, result);
  return static_cast<jint>(result);
}

JNIEXPORT jfloat Java_com_idquantique_quantis_Quantis_QuantisGetDriverVersion(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType)
{
  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);

  // Perform request
  float result = QuantisGetDriverVersion(deviceType);
  _CheckResult(env, static_cast<int>(result));
  return static_cast<jfloat>(result);
}

JNIEXPORT jfloat Java_com_idquantique_quantis_Quantis_QuantisGetLibVersion(
    JNIEnv * /*env*/,
    jclass /*obj*/)
{
  // Perform request
  return static_cast<jfloat>(QuantisGetLibVersion());
}

JNIEXPORT jstring Java_com_idquantique_quantis_Quantis_QuantisGetManufacturer(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  return env->NewStringUTF(QuantisGetManufacturer(deviceType, deviceNumber));
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesCount(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  int result = QuantisGetModulesCount(deviceType, deviceNumber);
  _CheckResult(env, result);
  return static_cast<jint>(result);
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesDataRate(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  int result = QuantisGetModulesDataRate(deviceType, deviceNumber);
  _CheckResult(env, result);
  return static_cast<jint>(result);
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesMask(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  int result = QuantisGetModulesMask(deviceType, deviceNumber);
  _CheckResult(env, result);
  return static_cast<jint>(result);
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesPower(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  int result = QuantisGetModulesPower(deviceType, deviceNumber);
  _CheckResult(env, result);
  return static_cast<jint>(result);
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesStatus(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  int result = QuantisGetModulesStatus(deviceType, deviceNumber);
  _CheckResult(env, result);
  return static_cast<jint>(result);
}

JNIEXPORT jstring Java_com_idquantique_quantis_Quantis_QuantisGetSerialNumber(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  return env->NewStringUTF(QuantisGetSerialNumber(deviceType, deviceNumber));
}

JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisModulesDisable(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jint jModulesMask)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);
  int modulesMask = static_cast<int>(jModulesMask);

  // Perform request
  int result = QuantisModulesDisable(deviceType, deviceNumber, modulesMask);
  _CheckResult(env, result);
}

JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisModulesEnable(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jint jModulesMask)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);
  int modulesMask = static_cast<int>(jModulesMask);

  // Perform request
  int result = QuantisModulesEnable(deviceType, deviceNumber, modulesMask);
  _CheckResult(env, result);
}

JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisModulesReset(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jint jModulesMask)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);
  int modulesMask = static_cast<int>(jModulesMask);

  // Perform request
  int result = QuantisModulesReset(deviceType, deviceNumber, modulesMask);
  _CheckResult(env, result);
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetAis31StartupTestsRequestFlag(
    JNIEnv *env,
    jclass /*obj*/,
    jlong jDeviceHandle)
{
  int result;

  // Convert types
  QuantisDeviceHandle *deviceHandle = reinterpret_cast<QuantisDeviceHandle *>(jDeviceHandle);

  // Perform request
  result = QuantisGetAis31StartupTestsRequestFlag(deviceHandle);
  _CheckResult(env, result);

  return static_cast<jint>(result);
}

JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisClearAis31StartupTestsRequestFlag(
    JNIEnv *env,
    jclass /*obj*/,
    jlong jDeviceHandle)
{
  int result;

  // Convert types
  QuantisDeviceHandle *deviceHandle = reinterpret_cast<QuantisDeviceHandle *>(jDeviceHandle);

  // Perform request
  result = QuantisClearAis31StartupTestsRequestFlag(deviceHandle);
  _CheckResult(env, result);
}

JNIEXPORT jbyteArray Java_com_idquantique_quantis_Quantis_QuantisRead(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jsize jSize)
{
  // Consistency check
  int size = static_cast<int>(jSize);
  if (size <= 0)
  {
    // Invalid device number, raise an error
    _CheckResult(env, QUANTIS_ERROR_INVALID_READ_SIZE);
  }

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Allocate buffer's memory
  char *buffer = (char *)malloc(size);
  if (!buffer)
  {
    _ThrowJavaException(env, QuantisStrError(QUANTIS_ERROR_NO_MEMORY));
    return env->NewByteArray(0);
  }

  // Java bytes array
  jbyteArray jBuffer;

  // Read data
  int result = QuantisRead(deviceType, deviceNumber, buffer, static_cast<size_t>(size));
  if (result < 0)
  {
    // Creates empty bytes array
    jBuffer = env->NewByteArray(0);

    // Frees allocated memory
    free(buffer);

    // Throws exception
    QuantisError errorNumber = static_cast<QuantisError>(result);
    _ThrowJavaException(env, QuantisStrError(errorNumber));
  }
  else if (result != size)
  {
    // Creates empty bytes array
    jBuffer = env->NewByteArray(0);

    // Frees allocated memory
    free(buffer);

    // Throws exception
    _ThrowJavaException(env, "Read wrong number of bytes");
  }
  else // result == size
  {
    // Creates bytes array
    jBuffer = env->NewByteArray(jSize);

    // Copy data to Java's buffer
    env->SetByteArrayRegion(jBuffer, 0, jSize, (jbyte *)buffer);

    // Frees allocated memory
    free(buffer);
  }

  // Finally return the bytes buffer
  return jBuffer;
}

JNIEXPORT jdouble Java_com_idquantique_quantis_Quantis_QuantisReadDouble01(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  double value;
  int result = QuantisReadDouble_01(deviceType, deviceNumber, &value);
  _CheckResult(env, result);
  return static_cast<jdouble>(value);
}

JNIEXPORT jdouble Java_com_idquantique_quantis_Quantis_QuantisReadScaledDouble(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jdouble jMin,
    jdouble jMax)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);
  double min = static_cast<double>(jMin);
  double max = static_cast<double>(jMax);

  // Perform request
  double value;
  int result = QuantisReadScaledDouble(deviceType, deviceNumber, &value, min, max);
  _CheckResult(env, result);
  return static_cast<jdouble>(value);
}

JNIEXPORT jfloat Java_com_idquantique_quantis_Quantis_QuantisReadFloat01(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  float value;
  int result = QuantisReadFloat_01(deviceType, deviceNumber, &value);
  _CheckResult(env, result);
  return static_cast<jfloat>(value);
}

JNIEXPORT jfloat Java_com_idquantique_quantis_Quantis_QuantisReadScaledFloat(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jfloat jMin,
    jfloat jMax)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);
  float min = static_cast<float>(jMin);
  float max = static_cast<float>(jMax);

  // Perform request
  float value;
  int result = QuantisReadScaledFloat(deviceType, deviceNumber, &value, min, max);
  _CheckResult(env, result);
  return static_cast<jfloat>(value);
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisReadInt(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  int value;
  int result = QuantisReadInt(deviceType, deviceNumber, &value);
  _CheckResult(env, result);
  return static_cast<jint>(value);
}

JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisReadScaledInt(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jint jMin,
    jint jMax)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);
  int min = static_cast<int>(jMin);
  int max = static_cast<int>(jMax);

  // Perform request
  int value;
  int result = QuantisReadScaledInt(deviceType, deviceNumber, &value, min, max);
  _CheckResult(env, result);
  return static_cast<jint>(value);
}

JNIEXPORT jshort Java_com_idquantique_quantis_Quantis_QuantisReadShort(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);

  // Perform request
  short value;
  int result = QuantisReadShort(deviceType, deviceNumber, &value);
  _CheckResult(env, result);
  return static_cast<jshort>(value);
}

JNIEXPORT jshort Java_com_idquantique_quantis_Quantis_QuantisReadScaledShort(
    JNIEnv *env,
    jclass /*obj*/,
    jint jDeviceType,
    jint jDeviceNumber,
    jshort jMin,
    jshort jMax)
{
  // Consistency check
  _CheckDeviceNumber(env, jDeviceNumber);

  // Convert types
  QuantisDeviceType deviceType = static_cast<QuantisDeviceType>(jDeviceType);
  unsigned int deviceNumber = static_cast<unsigned int>(jDeviceNumber);
  short min = static_cast<short>(jMin);
  short max = static_cast<short>(jMax);

  // Perform request
  short value;
  int result = QuantisReadScaledShort(deviceType, deviceNumber, &value, min, max);
  _CheckResult(env, result);
  return static_cast<jshort>(value);
}

#else
int unused; /* Silence `ISO C forbids an empty translation unit' warning.  */
#endif /* DISABLE_QUANTIS_JAVA */
