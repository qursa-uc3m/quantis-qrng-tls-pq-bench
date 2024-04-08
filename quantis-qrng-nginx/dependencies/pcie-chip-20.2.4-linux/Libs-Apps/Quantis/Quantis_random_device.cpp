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

#include "Quantis_random_device.hpp"

#include <sstream>
#include <limits>
#include <stdexcept>

#define BUFFER_SIZE 4

using namespace std;

idQ::random_device::random_device(const string &token) throw(runtime_error)
{
  QuantisDeviceType deviceType;
  unsigned int deviceNumber;

  try
  {

    if (token.compare(0, 1, "u") == 0)
    {
      deviceType = QUANTIS_DEVICE_USB;
    }
    else if (token.compare(0, 1, "p") == 0)
    {
      deviceType = QUANTIS_DEVICE_PCI;
    }
    else
    {
      stringstream msg;
      msg << "Quantis_C++11::random_device: Unrecognised device type. ";
      throw runtime_error(msg.str());
    }
    deviceNumber = ConvertFromString<unsigned int>(token.substr(1, token.size()));
  }
  catch (runtime_error e)
  {
    stringstream msg;
    msg << "Quantis_C++11::random_device: Could not read the input parameters. "
        << e.what();
    throw runtime_error(msg.str());
  }
  catch (...)
  {
    stringstream msg;
    msg << "Quantis_C++11::random_device: Could not read the input parameters. "
        << "Exception of unknown type.";
    throw runtime_error(msg.str());
  }

  try
  {
    quantis = new Quantis(static_cast<QuantisDeviceType>(deviceType), deviceNumber);
  }
  catch (runtime_error e)
  {
    stringstream msg;
    msg << "Quantis_C++11::random_device: Could not instantiate Quantis. "
        << e.what();
    throw runtime_error(msg.str());
  }
  catch (...)
  {
    stringstream msg;
    msg << "Quantis_C++11::random_device: Could not instantiate Quantis. "
        << "Exception of unknown type.";
    throw runtime_error(msg.str());
  }
}

idQ::random_device::~random_device()
{
  if (quantis)
  {
    quantis->~Quantis();
  }
}

// generating functions
idQ::random_device::result_type idQ::random_device::operator()() throw(std::runtime_error)
{
  unsigned int returnValue;
  unsigned char buffer[BUFFER_SIZE];

  try
  {
    quantis->Read(buffer, BUFFER_SIZE);
  }
  catch (runtime_error e)
  {
    stringstream msg;
    msg << "Quantis_C++11::operator()(): Could not perform a Quantis read. "
        << e.what();
    throw runtime_error(msg.str());
  }
  catch (...)
  {
    stringstream msg;
    msg << "Quantis_C++11::operator()(): Could not perform a Quantis read. "
        << "Exception of unknown type.";
    throw runtime_error(msg.str());
  }

  try
  {
    Convert(buffer, returnValue);
  }
  catch (runtime_error e)
  {
    stringstream msg;
    msg << "Quantis_C++11::operator()(): Could not convert the result of the Quantis read. "
        << e.what();
    throw runtime_error(msg.str());
  }
  catch (...)
  {
    stringstream msg;
    msg << "Quantis_C++11::operator()(): Could not convert the result of the Quantis read. "
        << "Exception of unknown type.";
    throw runtime_error(msg.str());
  }
  return static_cast<result_type>(returnValue);
}

double idQ::random_device::entropy() const NOEXCEPT
{
  return 0.0;
}

void idQ::random_device::Convert(const unsigned char *in, unsigned int &out)
{
  out = (in[3]) | (in[2] << 8) | (in[1] << 16) | (in[0] << 24);
}

template <class T>
T idQ::random_device::ConvertFromString(
    const std::string &str)
{
  T value;
  std::istringstream myStringStream(str);

  if (myStringStream >> value)
  {
    return value;
  }
  else
  {
    throw std::runtime_error("Unable to convert string");
  }
}

template <class T>
std::vector<T> idQ::random_device::SplitString(const std::string &str,
                                               const std::string &delimiters)
{
  std::vector<T> tokens;

  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos = str.find_first_of(delimiters, lastPos);

  while ((pos != std::string::npos) || (lastPos != std::string::npos))
  {
    // Found a token, add it to the vector.
    std::string token = str.substr(lastPos, pos - lastPos);
    tokens.push_back(ConvertFromString<T>(token));
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of(delimiters, pos);
    // Find next "non-delimiter"
    pos = str.find_first_of(delimiters, lastPos);
  }

  return tokens;
}
