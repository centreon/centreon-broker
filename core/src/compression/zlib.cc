/*
** Copyright 2011-2017 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <zlib.h>
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/compression/zlib.hh"
#include "com/centreon/broker/exceptions/corruption.hh"
#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker::compression;

/**************************************
*                                     *
*      Public Static Methods          *
*                                     *
**************************************/

/**
 * Compression function
 *
 * @param data the data to compress.
 * @param compression_level The compression level, by default -1.
 *
 * @return The same data compressed.
 */
std::vector<char> zlib::compress(std::vector<char> const& data,
                                 int compression_level) {
  if (data.empty())
    return {'\0', '\0', '\0', '\0'};

  uLongf nbytes = static_cast<uLongf>(data.size());

  if (compression_level < -1 || compression_level > 9)
    compression_level = -1;

  uLongf len = compressBound(nbytes);
  std::vector<char> retval;
  int res;
  do {
    retval.resize(len + 4);
    res = ::compress2(
      reinterpret_cast<Bytef*>(retval.data()) + 4,
      &len,
      reinterpret_cast<Bytef const*>(&data[0]),
      nbytes,
      compression_level);

    switch (res) {
      case Z_OK:
        retval.resize(len + 4);
        retval[0] = (nbytes >> 24) & 0xff;
        retval[1] = (nbytes >> 16) & 0xff;
        retval[2] = (nbytes >> 8) & 0xff;
        retval[3] = (nbytes & 0xff);
        break;
      case Z_MEM_ERROR:
        throw (exceptions::msg()
          << "compression: not enough memory to compress "
          << nbytes << " bytes");
        break;
      case Z_BUF_ERROR:
        len <<= 1;
        break;
    }
  } while (res == Z_BUF_ERROR);

  return retval;
}

/**
 * Uncompress function
 *
 * @param data The data to extract.
 * @param nbytes The data size in bytes.
 *
 * @return the extract data
 */
std::vector<char> zlib::uncompress(unsigned char const* data, uLong nbytes) {
  if (!data) {
    logging::debug(logging::medium)
      << "compression: attempting to uncompress null buffer";
    return std::vector<char>();
  }
  if (nbytes <= 4) {
    if (nbytes < 4 ||
        (data[0] != 0 || data[1] != 0 || data[2] != 0 || data[3] != 0))
      throw exceptions::corruption()
          << "compression: attempting to uncompress data with invalid size";
  }
  ulong expected_size = (data[0] << 24) | (data[1] << 16)
                     | (data[2] <<  8) | data[3];
  ulong len = (expected_size > 1ul) ? expected_size: 1ul;
  if (len > stream::max_data_size)
    throw exceptions::corruption()
        << "compression: data expected size is too big";
  std::vector<char> uncompressed_array(len, '\0');

  ulong alloc = len;

  int res = ::uncompress(reinterpret_cast<Bytef*>(uncompressed_array.data()),
                         &len, static_cast<Bytef const*>(data) + 4, nbytes - 4);

  switch (res) {
    case Z_OK:
      if (len != alloc)
        uncompressed_array.resize(len);
      break;
    case Z_MEM_ERROR:
      throw exceptions::msg()
        << "compression: not enough memory to uncompress " << nbytes
        << " compressed bytes to " << len << " uncompressed bytes";
    case Z_BUF_ERROR:
    case Z_DATA_ERROR:
      throw exceptions::corruption()
        << "compression: compressed input data is corrupted, "
        << "unable to uncompress it";
  }
  return uncompressed_array;
}
