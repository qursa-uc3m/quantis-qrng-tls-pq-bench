/*
 * Copyright (C) 2004-2020 ID Quantique SA.  All Rights Reserved.
 *
 * THIS COMPUTER PROGRAM IS PROPRIETARY AND CONFIDENTIAL TO ID QUANTIQUE SA
 * AND ITS LICENSORS AND CONTAINS TRADE SECRETS OF ID QUANTIQUE SA THAT ARE
 * PROVIDED PURSUANT TO A WRITTEN AGREEMENT CONTAINING RESTRICTIONS ON USE
 * AND DISCLOSURE. ANY USE, REPRODUCTION, OR TRANSFER EXCEPT AS PROVIDED IN
 * SUCH AGREEMENT IS STRICTLY PROHIBITED.
 */

/*
 * This header is based on the C++11 standard (more specifically on the draft
 * of the 28.02.2011) and contains the same functions and fields
 * as the random_device class used to access True Random Number Generators.
 * This allows those using the random_device class
 * in their implementation already to simply substitute the header file <random>
 * for this header in order to switch from the standard implementation to Quantis.
 *
 * There are exactly two differences to the standard: First, the input to the constructor,
 * namely the string "token", is not interpreted as the path to a device but rather takes as input
 * the device type and device number of Quantis. See the C++11 sample code on how it should
 * be used.
 * Secondly, since we don't access a ready device but create a Quantis object, this object needs
 * to be deleted. We have therefore included a destructor function that is not present in the
 * standard and should be used to free the memory allocated to the Quantis object.
 *
 * The function "entropy" will be implemented in future releases.
 *
 */
#ifndef QUANTIS_RANDOM_DEVICE
#define QUANTIS_RANDOM_DEVICE

#include "Quantis.hpp"
#include <vector>
#include <limits.h>

#ifdef _WIN32
//so far, the C++11 features we need do not exist in
//any Windows compiler, but in the future
//this bracket will contain _MSC_VER >= (version which
//supports our features)
//#if ( _MSC_VER >= )
#define NOEXCEPT
#define CONSTEXPR
//using Visual Studio 2010 or later
//#else
//  #define NOEXCEPT noexcept
//  #define CONSTEXPR constexpr
#endif

#ifdef CXX11_SUPPORTED
#define NOEXCEPT noexcept
#define CONSTEXPR constexpr
#else
#define NOEXCEPT
#define CONSTEXPR
#endif

namespace idQ
{

class random_device
{
public:
  /**
    * Quantis always returns unsigned integers when accessed by this interface.
    */
  typedef unsigned int result_type;

  /**
    * Constructor of the random_device class. Takes as input a string telling it
    * which device type and device number is desired. E.g. to access the Quantis USB device
    * with number 0, give it as input the string "u0".
   * @param token Contains the encoded device type and number
   * @throw runtime_error
    */
  explicit random_device(const std::string &token = "") throw(std::runtime_error);

  /**
    * Deconstructor function not in the C++11 standard.
    * Must be called to free the space allocated to the Quantis pointer.
    */
  ~random_device();

  /**
    * Returns the minimal value which can be returned by Quantis.
    */
  static CONSTEXPR result_type min() { return 0; }

  /**
    * Returns the maximal value which can be returned by Quantis.
    */
  static CONSTEXPR result_type max() { return UINT_MAX; }

  /**
    * Returns an estimate of the entropy of the underlying Quantis.
    * Currently not implemented.
    * @throw runtime_errors
    */
  double entropy() const NOEXCEPT;

  /**
    * Returns the next random number from the underlying Quantis.
    * @param runtime_error
    */
  result_type operator()() throw(std::runtime_error);

private:
  /**
    * A pointer to the Quantis device.
    */
  Quantis *quantis;

  /**
    * Internal function not in the C++11 standard.
    * Converts an unsigned char sequence to an unsigned integer
    * @param in the unsigned char sequence to convert
    * @param out the resulting number
    * @warning the input string must be of size 4!
    */
  void Convert(const unsigned char *in, unsigned int &out);

  /**
      * Internal function not in the C++11 standard.
      * Convert a string to a non-string type.
      * @param str the string to be converted.
      * @return the value contained in the string.
      */
  template <class T>
  T ConvertFromString(const std::string &str);

  /**
      * Internal function not in the C++11 standard.
      * Splits this string around matches of the given delimiters.
      * @param str the string to split.
      * @param delimiters the delimiters.
      * @return a vector containing the individual values.
      */
  template <class T>
  std::vector<T> SplitString(const std::string &str,
                             const std::string &delimiters = " ");

  /**
     * Copy constructors are explicitly disallowed in C++11
     */
#ifdef CXX_11_SUPPORTED
  random_device(const random_device &) = delete;
  void operator=(const random_device &) = delete;
#endif
};
} // namespace idQ

#endif //QUANTIS_RANDOM_DEVICE
