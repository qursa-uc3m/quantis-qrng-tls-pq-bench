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
 * ----------------------------------------------------------------------------
 *
 * For history of changes, see ChangeLog.txt
 */

#include "Quantis.h"
#include "Quantis_Compat.h"

/*
 * Checks if QUANTIS_DEVICE_TYPE has been defined. This define is mandatory
 * and must take one of the values in QuantisDeviceType enumeration
 * defined in Quantis.h
 */
#ifndef QUANTIS_DEVICE_TYPE
#error "QUANTIS_DEVICE_TYPE has not been defined! Unable to continue."
#endif

int quantisBoardReset(unsigned int deviceNumber)
{
  return QuantisBoardReset((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber);
}

int quantisCount()
{
  return QuantisCount((QuantisDeviceType)QUANTIS_DEVICE_TYPE);
}

int quantisBoardVersion(unsigned int deviceNumber)
{
  return QuantisGetBoardVersion((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber);
}

int quantisDriverVersion()
{
  float version = QuantisGetDriverVersion((QuantisDeviceType)QUANTIS_DEVICE_TYPE);
  if (version < 0.0f)
  {
    /* Error */
    return (int)version;
  }
  return (int)(version * 10.0f);
}

int quantisLibVersion()
{
  float version = QuantisGetLibVersion();
  return (int)(version * 10.0f);
}

int quantisModuleDataRate(unsigned int deviceNumber)
{
  return QuantisGetModulesDataRate((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber);
}

int quantisGetModules(unsigned int deviceNumber)
{
  return QuantisGetModulesMask((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber);
}

int quantisModulesPower(unsigned int deviceNumber)
{
  return QuantisGetModulesPower((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber);
}

int quantisModulesStatus(unsigned int deviceNumber)
{
  return QuantisGetModulesStatus((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber);
}

char *quantisGetSerialNumber(unsigned int deviceNumber)
{
  return QuantisGetSerialNumber((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber);
}

int quantisModulesDisable(unsigned int deviceNumber, int moduleMask)
{
  return QuantisModulesDisable((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber, moduleMask);
}

int quantisModulesEnable(unsigned int deviceNumber, int moduleMask)
{
  return QuantisModulesEnable((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber, moduleMask);
}

int quantisModulesReset(unsigned int deviceNumber, int moduleMask)
{
  return QuantisModulesReset((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber, moduleMask);
}

int quantisRead(unsigned int deviceNumber, void *buffer, size_t size)
{
  return QuantisRead((QuantisDeviceType)QUANTIS_DEVICE_TYPE, deviceNumber, buffer, size);
}
