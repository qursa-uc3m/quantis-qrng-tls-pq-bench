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

#ifndef QUANTIS_JAVA_H
#define QUANTIS_JAVA_H

#ifndef _WIN32
/* On Windows DISABLE_QUANTIS_JAVA is provided by the compiler */
#include "QuantisLibConfig.h"
#endif

#ifndef DISABLE_QUANTIS_JAVA

#include <jni.h>

#ifdef __cplusplus
extern "C"
{
#endif

  JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisBoardReset(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisCount(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetBoardVersion(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jfloat Java_com_idquantique_quantis_Quantis_QuantisGetDriverVersion(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType);

  JNIEXPORT jfloat Java_com_idquantique_quantis_Quantis_QuantisGetLibVersion(
      JNIEnv *env,
      jclass obj);

  JNIEXPORT jstring Java_com_idquantique_quantis_Quantis_QuantisGetManufacturer(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesCount(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesDataRate(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesMask(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesPower(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetModulesStatus(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jstring Java_com_idquantique_quantis_Quantis_QuantisGetSerialNumber(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisModulesDisable(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber,
      jint jModulesMask);

  JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisModulesEnable(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber,
      jint jModulesMask);

  JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisModulesReset(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber,
      jint jModulesMask);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisGetAis31StartupTestsRequestFlag(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT void Java_com_idquantique_quantis_Quantis_QuantisClearAis31StartupTestsRequestFlag(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jbyteArray Java_com_idquantique_quantis_Quantis_QuantisRead(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber,
      jsize jSize);

  JNIEXPORT jdouble Java_com_idquantique_quantis_Quantis_QuantisReadDouble01(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jfloat Java_com_idquantique_quantis_Quantis_QuantisReadFloat01(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisReadInt(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jshort Java_com_idquantique_quantis_Quantis_QuantisReadShort(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber);

  JNIEXPORT jdouble Java_com_idquantique_quantis_Quantis_QuantisReadScaledDouble(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber,
      jdouble jMin,
      jdouble jMax);

  JNIEXPORT jfloat Java_com_idquantique_quantis_Quantis_QuantisReadScaledFloat(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber,
      jfloat jMin,
      jfloat jMax);

  JNIEXPORT jint Java_com_idquantique_quantis_Quantis_QuantisReadScaledInt(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber,
      jint jMin,
      jint jMax);

  JNIEXPORT jshort Java_com_idquantique_quantis_Quantis_QuantisReadScaledShort(
      JNIEnv *env,
      jclass obj,
      jint jDeviceType,
      jint jDeviceNumber,
      jshort jMin,
      jshort jMax);

#ifdef __cplusplus
}
#endif

#endif /* DISABLE_QUANTIS_JAVA */
#endif /* QUANTIS_JAVA_H */
