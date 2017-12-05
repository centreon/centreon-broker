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

#include <arpa/inet.h>
#include <gtest/gtest.h>
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "memory_stream.hh"

using namespace com::centreon::broker;

class  CompressionStreamRead : public ::testing::Test {
 public:
  void SetUp() {
    _stream = new compression::stream(-1, 20000);
    _substream = new CompressionStreamMemoryStream();
    _stream->set_substream(_substream);
    return ;
  }

  misc::shared_ptr<io::raw> predefined_data() {
    misc::shared_ptr<io::raw> r(new io::raw);
    for (int i(0); i < 1000; ++i)
      r->append(static_cast<char*>(static_cast<void*>(&i)), sizeof(i));
    return (r);
  }

 protected:
  misc::shared_ptr<compression::stream>
       _stream;
  misc::shared_ptr<CompressionStreamMemoryStream>
       _substream;
};

// Given a compression stream
// And the substream has no immediate data
// When read() is called with a 0 timeout
// Then read() returns false
// And the data is null
TEST_F(CompressionStreamRead, NoData) {
  // Given
  _substream->timeout(true);

  // When
  misc::shared_ptr<io::data> d;
  bool retval(_stream->read(d, 0));

  // Then
  ASSERT_FALSE(retval);
  ASSERT_TRUE(d.isNull());
}

// Given a compression stream
// And write() was called with some data
// And flush() was called
// When read() is called
// Then the return value is true
// And the data is returned
TEST_F(CompressionStreamRead, NormalRead) {
  // Given
  _stream->write(predefined_data());
  _stream->flush();

  // When
  misc::shared_ptr<io::data> d;
  bool retval(_stream->read(d));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_TRUE(!d.isNull());
  ASSERT_EQ(d->type(), io::raw::static_type());
  ASSERT_EQ(d.ref_as<io::raw>(), *predefined_data());
}

// Given a compression stream
// And some data is available in the compression buffer
// And the substream is shutdown
// When read() is called
// Then the return value is true
// And the data of the compression buffer is returned
TEST_F(CompressionStreamRead, BufferIsReadBack) {
  // Given
  _stream->write(predefined_data());
  _substream->shutdown(true);

  // When
  misc::shared_ptr<io::data> d;
  bool retval(_stream->read(d));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_EQ(d.ref_as<io::raw>(), *predefined_data());
}

// Given a compression stream
// And the substream is shutdown
// And no more data is available
// When read() is called
// Then it throws a shutdown exception
TEST_F(CompressionStreamRead, Shutdown) {
  // Given
  _stream->write(predefined_data());
  _substream->shutdown(true);
  misc::shared_ptr<io::data> d;
  _stream->read(d);

  // When, Then
  ASSERT_THROW(_stream->read(d), exceptions::shutdown);
}

// Given a compression stream
// And the substream has a corrupted compressed data chunk before a valid data chunk
// When read() is called
// Then the valid data is extracted
TEST_F(CompressionStreamRead, CorruptedData) {
  // Given
  _stream->write(predefined_data());
  _stream->flush();
  _stream->write(predefined_data());
  _stream->flush();
  misc::shared_ptr<io::raw>& buffer(_substream->get_buffer());
  (*buffer)[4] = 42;
  (*buffer)[5] = 42;

  // When
  misc::shared_ptr<io::data> d;
  bool retval(_stream->read(d));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_FALSE(d.isNull());
  ASSERT_EQ(d->type(), io::raw::static_type());
  ASSERT_EQ(d.ref_as<io::raw>(), *predefined_data());
}

// Given a compression stream
// And the substream has a corrupted compressed data chunk before a valid data chunk
// When read() is called
// Then the valid data is extracted
TEST_F(CompressionStreamRead, CorruptedDataZippedPart) {
  // Given
  _stream->write(predefined_data());
  _stream->flush();
  _stream->write(predefined_data());
  _stream->flush();
  misc::shared_ptr<io::raw>& buffer(_substream->get_buffer());
  (*buffer)[8] = 42;
  (*buffer)[9] = 42;

  // When
  misc::shared_ptr<io::data> d;
  bool retval(_stream->read(d));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_FALSE(d.isNull());
  ASSERT_EQ(d->type(), io::raw::static_type());
  ASSERT_EQ(d.ref_as<io::raw>(), *predefined_data());
}

// Given a compression stream
// And the substream has data that indicates that the following fragment is greater than the allowed max size, followed by valid data
// When read() is called
// Then the valid data is extracted
TEST_F(CompressionStreamRead, FragmentGreaterThanMaxSize) {
  // Given
  _stream->write(predefined_data());
  _stream->flush();
  _stream->write(predefined_data());
  _stream->flush();
  misc::shared_ptr<io::raw>& buffer(_substream->get_buffer());
  *static_cast<uint32_t*>(static_cast<void*>(buffer->QByteArray::data())) = htonl(0xFFFFFFFF);

  // When
  misc::shared_ptr<io::data> d;
  bool retval(_stream->read(d));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_FALSE(d.isNull());
  ASSERT_EQ(d->type(), io::raw::static_type());
  ASSERT_EQ(d.ref_as<io::raw>(), *predefined_data());
}
