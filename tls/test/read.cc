/*
 * Copyright 2021 Centreon (https://www.centreon.com/)
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
#include <arpa/inet.h>
#include <gtest/gtest.h>
#include "com/centreon/broker/tls/stream.hh"
#include "com/centreon/broker/config/applier/init.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "memory_stream.hh"

using namespace com::centreon::broker;

class TlsStreamRead : public ::testing::Test {
  protected:
    SSL_CTX* _ctx;
    SSL* _ssl;
    BIO* _rbio;
    BIO* _wbio;
  std::shared_ptr<tls::stream> _stream;
  std::shared_ptr<StreamMemoryStream> _substream;

 public:
  void SetUp() override {
    _ctx = SSL_CTX_new(TLS_method());
    _ssl = SSL_new(_ctx);
    _rbio = BIO_new(BIO_s_mem());
    _wbio = BIO_new(BIO_s_mem());
    SSL_set_bio(_ssl, _rbio, _wbio);

    SSL_CTX_set_ecdh_auto(_ctx, 1);
    int ret = SSL_set_cipher_list(_ssl, "aNULL");

    if (!ret) {
      std::cout << "Unable to find cipher for anonymous session" << std::endl;
    }

    try {
      config::applier::init(0, "test_broker");
    } catch (const std::exception& e) {
      (void)e;
    }
    _stream = std::make_shared<tls::stream>(_ssl);
    _substream.reset(new StreamMemoryStream());
    _stream->set_substream(_substream);
  }

  void TearDown() override { config::applier::deinit(); }

  std::shared_ptr<io::raw> predefined_data() {
    std::shared_ptr<io::raw> r(new io::raw);
    for (int i = 0; i < 1000; ++i)
      std::copy(reinterpret_cast<char*>(&i),
                reinterpret_cast<char*>(&i) + sizeof(i),
                std::back_inserter(r->get_buffer()));
    return r;
  }
};

// Given a tls stream
// And the substream has no immediate data
// When read() is called with a 0 timeout
// Then read() returns false
// And the data is null
TEST_F(TlsStreamRead, NoData) {
  // Given
  _substream->timeout(true);

  // When
  std::shared_ptr<io::data> d;
  bool retval(_stream->read(d, 0));

  // Then
  ASSERT_FALSE(retval);
  ASSERT_TRUE(!d);
}

// Given a tls stream
// And write() was called with some data
// And flush() was called
// When read() is called
// Then the return value is true
// And the data is returned
TEST_F(TlsStreamRead, NormalRead) {
  // Given
  _stream->write(predefined_data());
  _stream->flush();

  // When
  std::shared_ptr<io::data> d;
  bool retval(_stream->read(d, 0));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_TRUE(d);
  ASSERT_EQ(d->type(), io::raw::static_type());
  ASSERT_EQ(std::static_pointer_cast<io::raw>(d)->get_buffer(),
            predefined_data()->get_buffer());
}

// Given a tls stream
// And some data is available in the tls buffer
// And the substream is shutdown
// When read() is called
// Then the return value is true
// And the data of the tls buffer is returned
TEST_F(TlsStreamRead, BufferIsReadBack) {
  // Given
  _stream->write(predefined_data());
  _substream->shutdown(true);

  // When
  std::shared_ptr<io::data> d;
  bool retval(_stream->read(d, 0));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_EQ(std::static_pointer_cast<io::raw>(d)->get_buffer(),
            predefined_data()->get_buffer());
}

// Given a tls stream
// And the substream is shutdown
// And no more data is available
// When read() is called
// Then it throws a shutdown exception
TEST_F(TlsStreamRead, Shutdown) {
  // Given
  _stream->write(predefined_data());
  _substream->shutdown(true);
  std::shared_ptr<io::data> d;
  _stream->read(d, 0);

  // When, Then
  ASSERT_THROW(_stream->read(d, 0), exceptions::shutdown);
}

// Given a tls stream
// And the substream has a corrupted compressed data chunk before a valid data
// chunk When read() is called Then the valid data is extracted
TEST_F(TlsStreamRead, CorruptedData) {
  // Given
  _stream->write(predefined_data());
  _stream->flush();
  _stream->write(predefined_data());
  _stream->flush();
  std::shared_ptr<io::raw>& buffer(_substream->get_buffer());
  buffer->get_buffer()[4] = 42;
  buffer->get_buffer()[5] = 42;

  // When
  std::shared_ptr<io::data> d;
  bool retval(_stream->read(d, 0));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_FALSE(!d);
  ASSERT_EQ(d->type(), io::raw::static_type());
  ASSERT_EQ(std::static_pointer_cast<io::raw>(d)->get_buffer(),
            predefined_data()->get_buffer());
}

// Given a tls stream
// And the substream has a corrupted compressed data chunk before a valid data
// chunk When read() is called Then the valid data is extracted
TEST_F(TlsStreamRead, CorruptedDataZippedPart) {
  // Given
  _stream->write(predefined_data());
  _stream->flush();
  _stream->write(predefined_data());
  _stream->flush();
  std::shared_ptr<io::raw>& buffer(_substream->get_buffer());
  buffer->get_buffer()[8] = 42;
  buffer->get_buffer()[9] = 42;

  // When
  std::shared_ptr<io::data> d;
  bool retval(_stream->read(d, 0));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_FALSE(!d);
  ASSERT_EQ(d->type(), io::raw::static_type());
  ASSERT_EQ(std::static_pointer_cast<io::raw>(d)->get_buffer(),
            predefined_data()->get_buffer());
}

// Given a tls stream
// And the substream has data that indicates that the following fragment is
// greater than the allowed max size, followed by valid data When read() is
// called Then the valid data is extracted
TEST_F(TlsStreamRead, FragmentGreaterThanMaxSize) {
  // Given
  _stream->write(predefined_data());
  _stream->flush();
  _stream->write(predefined_data());
  _stream->flush();
  std::shared_ptr<io::raw>& buffer(_substream->get_buffer());
  *static_cast<uint32_t*>(static_cast<void*>(buffer->data())) =
      htonl(0xFFFFFFFF);

  // When
  std::shared_ptr<io::data> d;
  bool retval(_stream->read(d, 0));

  // Then
  ASSERT_TRUE(retval);
  ASSERT_FALSE(!d);
  ASSERT_EQ(d->type(), io::raw::static_type());
  ASSERT_EQ(std::static_pointer_cast<io::raw>(d)->get_buffer(),
            predefined_data()->get_buffer());
}

