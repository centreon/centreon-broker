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
#include "com/centreon/broker/compression/stream.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;

class  CompressionStreamWriteMemoryStream : public io::stream {
 public:
        CompressionStreamWriteMemoryStream()
    : _shutdown(false) {}

  bool read(
         misc::shared_ptr<io::data>& d,
         time_t deadline = (time_t)-1) {
    (void)deadline;
    if (_shutdown)
      throw (exceptions::shutdown() << __FUNCTION__
             << " is shutdown");

    d = _buffer;
    _buffer = misc::shared_ptr<io::raw>();
    return (true);
  }

  int  write(misc::shared_ptr<io::data> const& d) {
    if (d.isNull() || (d->type() != io::raw::static_type()))
      throw (exceptions::msg()
             << "invalid data sent to " << __FUNCTION__);
    io::raw const& e(d.ref_as<io::raw>());
    if (_buffer.isNull())
      _buffer = new io::raw(e);
    else
      _buffer->append(e);
    return (1);
  }

  void shutdown(bool shut_it_down = true) {
    _shutdown = shut_it_down;
    return ;
  }

 private:
  misc::shared_ptr<io::raw>
       _buffer;
  bool _shutdown;
};

class   CompressionStreamWrite : public ::testing::Test {
 public:
  void  SetUp() {
    _stream = new compression::stream(-1, 20000);
    _substream = new CompressionStreamWriteMemoryStream();
    _stream->set_substream(_substream);
    return ;
  }

  misc::shared_ptr<io::data> new_data() {
    misc::shared_ptr<io::raw> r(new io::raw);
    for (int i(0); i < 1000; ++i)
      r->append(static_cast<char*>(static_cast<void*>(&i)), sizeof(i));
    return (r);
  }

 protected:
  misc::shared_ptr<compression::stream>
        _stream;
  misc::shared_ptr<CompressionStreamWriteMemoryStream>
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
    misc::shared_ptr<io::data> d;
    _stream->read(d);
  }

  // When
  int retval(_stream->write(new_data()));

  // Then
  ASSERT_EQ(retval, 1);
}

// Given a compression stream
// And write() is called with a data payload
// When flush() is called
// Then the data written to the substream is smaller than the original size
TEST_F(CompressionStreamWrite, Compress) {
  // Given
  misc::shared_ptr<io::raw> r(new_data().staticCast<io::raw>());
  _stream->write(r);

  // When
  _stream->flush();

  // Then
  misc::shared_ptr<io::data> d;
  _substream->read(d);
  ASSERT_LT(d.staticCast<io::raw>()->size(), r->size());
}

// Given a compression stream
// When write() is called with a buffer greater than the maximum allowed size
// Then the method throws
TEST_F(CompressionStreamWrite, TooMuchData) {
  // When
  misc::shared_ptr<io::raw> r(new io::raw);
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
  misc::shared_ptr<io::data> d;
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
  _stream = new compression::stream(-1, 20000);
  _stream->set_substream(_substream);
  _stream->write(new_data());
  misc::shared_ptr<io::data> d;
  _stream->read(d);
  ASSERT_TRUE(d.isNull());

  // When
  int retval(_stream->flush());

  // Then
  ASSERT_EQ(retval, 0);
  _stream->read(d);
  ASSERT_TRUE(!d.isNull());
}
