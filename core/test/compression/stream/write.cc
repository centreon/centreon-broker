/*
 * Copyright 2011 - 2019 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */
#include <gtest/gtest.h>
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "memory_stream.hh"

using namespace com::centreon::broker;

class   CompressionStreamWrite : public ::testing::Test {
 public:
  void  SetUp() override {
    try {
      config::applier::init();
    }
    catch (std::exception const& e) {
      (void) e;
    }
    _stream.reset(new compression::stream(-1, 20000));
    _substream.reset(new CompressionStreamMemoryStream());
    _stream->set_substream(_substream);
  }

  void TearDown() override {
    _stream.reset();
    _substream.reset();
    // The cache must be destroyed before the applier deinit() call.
    config::applier::deinit();
  }

  std::shared_ptr<io::data> new_data() {
    std::shared_ptr<io::raw> r(new io::raw);
    r->get_buffer().reserve(1000 * sizeof(int));
    for (int i(0); i < 1000; ++i)
      std::copy(reinterpret_cast<char*>(&i), reinterpret_cast<char*>(&i) + sizeof(i), std::back_inserter(r->get_buffer()));
    return r;
  }

 protected:
  std::shared_ptr<compression::stream>
        _stream;
  std::shared_ptr<CompressionStreamMemoryStream>
        _substream;
};

// Given a compression stream
// When write() is called
// Then the return value is 1
TEST_F(CompressionStreamWrite, Returns1FirstCall) {
  // When
  int retval(_stream->write(new_data()));

  // Then
  ASSERT_EQ(retval, 1);
}

// Given a compression stream
// When write() is called multiple times
// Then the return value is always 1
TEST_F(CompressionStreamWrite, Returns1AfterMultipleCalls) {
  // Given
  for (int i(0); i < 10; ++i) {
    // When
    int retval(_stream->write(new_data()));

    // Then
    ASSERT_EQ(retval, 1);
  }
}

// Given a compression stream
// And write() and read() were called multiple times
// When write() is called
// Then the return value is 1
TEST_F(CompressionStreamWrite, Returns1WithInterleavedRead) {
  // Given
  for (int i(0); i < 10; ++i) {
    for (int j(0); j < 10; ++j)
      _stream->write(new_data());
    std::shared_ptr<io::data> d;
    _stream->read(d);
  }

  // When
  int retval(_stream->write(new_data()));

  // Then
  ASSERT_EQ(retval, 1);
}

// Given an compression stream
// And write() is called with no data
// When flush() is called
// Then nothing is written
TEST_F(CompressionStreamWrite, CompressNothing) {
  // Given
  std::shared_ptr<io::raw> r(new io::raw);
  _stream->write(r);

  // When
  _stream->flush();

  // Then
  std::shared_ptr<io::data> d;
  ASSERT_THROW(_substream->read(d), exceptions::msg);
}

// Given a compression stream
// And write() is called with a data payload
// When flush() is called
// Then the data written to the substream is smaller than the original size
TEST_F(CompressionStreamWrite, Compress) {
  // Given
  std::shared_ptr<io::raw> r(std::static_pointer_cast<io::raw>(new_data()));
  _stream->write(r);

  // When
  _stream->flush();

  // Then
  std::shared_ptr<io::data> d;
  _substream->read(d);
  ASSERT_LT(std::static_pointer_cast<io::raw>(d)->size(), r->size());
}

// Given a compression stream
// When write() is called with a buffer greater than the maximum allowed size
// Then the method throws
TEST_F(CompressionStreamWrite, TooMuchData) {
  // When
  std::shared_ptr<io::raw> r(new io::raw);
  r->resize(compression::stream::max_data_size + 10);

  // Then
  ASSERT_THROW(_stream->write(r), exceptions::msg);
}

// Given a compression stream
// And the substream thrown a shutdown exception during a read() call of the compression stream
// When write() is called
// Then it throws a shutdown exception
TEST_F(CompressionStreamWrite, WriteOnShutdown) {
  // Given
  _substream->shutdown(true);
  std::shared_ptr<io::data> d;
  ASSERT_THROW(_stream->read(d), exceptions::shutdown);

  // When, Then
  ASSERT_THROW(_stream->write(new_data()), exceptions::shutdown);
}

// Given a compression stream
// And write() was called with a data payload smaller than the buffer size
// When flush() is called
// Then the return value is 0
// And compressed data is written to the substream
TEST_F(CompressionStreamWrite, Flush) {
  // Given
  _stream.reset(new compression::stream(-1, 20000));
  _stream->set_substream(_substream);
  _stream->write(new_data());
  ASSERT_TRUE(!_substream->get_buffer() || _substream->get_buffer()->empty());

  // When
  int retval(_stream->flush());

  // Then
  ASSERT_EQ(retval, 0);
  std::shared_ptr<io::data> d;
  _stream->read(d);
  ASSERT_TRUE(d);
}
