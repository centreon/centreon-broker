/*
** Copyright 2017 Centreon
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

#include <gtest/gtest.h>
#include "com/centreon/broker/compression/zlib.hh"

using namespace com::centreon::broker::compression;

class  CompressionZlib : public ::testing::Test {
 public:
  void SetUp() {}
};

// Given a simple buffer
// When zlib::compress() is called and then zlib::uncompress()
// Then we get as result the same buffer as the input one
TEST_F(CompressionZlib, Simple) {
  // Given
  QByteArray data("Some data compression");
  // When
  QByteArray compressed(zlib::compress(data, -1));
  QByteArray uncompressed(
    zlib::uncompress(
      static_cast<unsigned char const*>(static_cast<void const*>(compressed.constData())),
      compressed.size()));

  // Then
  ASSERT_EQ(uncompressed, data);
}

// Given an empty buffer
// When zlib::compress() is called
// Then we get as result a buffer containing "\0\0\0\0"
TEST_F(CompressionZlib, Empty) {
  // Given
  QByteArray data;
  // When
  QByteArray compressed(zlib::compress(data, -1));

  QByteArray expected(4, '\0');
  ASSERT_EQ(compressed, expected);
}
