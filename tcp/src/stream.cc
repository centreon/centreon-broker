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

#include <QWaitCondition>
#include <sstream>
#include <sys/socket.h>
#include <sys/time.h>
#include <system_error>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] sock  Socket used by this stream.
 *  @param[in] name  Name of this connection.
 */
stream::stream(asio::ip::tcp::socket* sock, std::string const& name)
  : _name(name),
    _parent(NULL),
    _read_timeout(-1),
    _socket(sock),
    _write_timeout(-1) {
  _set_socket_options();
}

/**
 *  Destructor.
 */
stream::~stream() {
  try {
    // Close the socket.
    if (_socket.get())
      _socket->close();
    // Remove from parent.
    if (_parent)
      _parent->remove_child(_name);
  }
  // Ignore exception, whatever the error might be.
  catch (...) {}
}

/**
 *  Get peer name.
 *
 *  @return Peer name.
 */
std::string stream::peer() const {
  std::ostringstream oss;
  oss << "tcp://" << _socket->remote_endpoint().address().to_string()
      << ":" << _socket->remote_endpoint().port();
  return oss.str();
}

/**
 *  Read data with timeout.
 *
 *  @param[out] d         Received event if any.
 *  @param[in]  deadline  Timeout in seconds.
 *
 *  @return Respects io::stream::read()'s return value.
 */
bool stream::read(std::shared_ptr<io::data>& d,
  time_t deadline) {
  // Check that socket exist.
  if (!_socket.get())
    _initialize_socket();

  // Set deadline.
  {
    time_t now = ::time(NULL);
    if (_read_timeout != -1
        && (deadline == (time_t)-1
            || now + _read_timeout < deadline))
      deadline = now + _read_timeout / 1000;
  }

  d.reset();

  std::error_code err;
  asio::streambuf b;

  size_t len = asio::read(*_socket, b, asio::transfer_all(), err);

  if (err)
    throw exceptions::msg() << "TCP peer '"
                             << _name << "' err: " << err.message();
  std::string s((std::istreambuf_iterator<char>(&b)), std::istreambuf_iterator<char>());

  std::shared_ptr<io::raw> data(new io::raw);
  data->append(QString::fromStdString(s).toAscii());
  d = data;
  return true;
}

/**
 *  Set parent socket.
 *
 *  @param[in,out] parent  Parent socket.
 */
void stream::set_parent(acceptor* parent) {
  _parent = parent;
}

/**
 *  Set read timeout.
 *
 *  @param[in] secs  Timeout in seconds.
 */
void stream::set_read_timeout(int secs) {
  if (secs == -1)
    _read_timeout = -1;
  else
    _read_timeout = secs * 1000;
}

/**
 *  Set write timeout.
 *
 *  @param[in] secs  Write timeout in seconds.
 */
void stream::set_write_timeout(int secs) {
  if (secs == -1)
    _write_timeout = -1;
  else
    _write_timeout = secs * 1000;
}

/**
 *  Write data to the socket.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(std::shared_ptr<io::data> const& d) {
  // Check that socket exist.
  if (!_socket.get())
    _initialize_socket();

  // Check that data exists and should be processed.
  if (!validate(d, "TCP"))
    return 1;

  if (d->type() == io::raw::static_type()) {
    std::shared_ptr <io::raw> r(std::static_pointer_cast<io::raw>(d));
    logging::debug(logging::low) << "TCP: write request of "
                                 << r->size() << " bytes to peer '" << _name << "'";

    std::error_code err;

    asio::write(*_socket, asio::buffer(r->QByteArray::data(), r->size()), asio::transfer_all(), err);

    if (err)
      throw exceptions::msg() << "TCP: error while writing to peer '"
                              << _name << "': " << err.message();
  }
  return 1;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Initialize socket if it was not already initialized.
 */
void stream::_initialize_socket() {
  _socket.reset(new asio::ip::tcp::socket{_io_context});
  {
    std::ostringstream oss;
    oss << _socket->remote_endpoint().address().to_string()
        << ":" << _socket->remote_endpoint().port();
    _name = oss.str();
  }
  if (_parent)
    _parent->add_child(_name);
  _set_socket_options();
  return ;
}

/**
 *  Set various socket options.
 */
void stream::_set_socket_options() {
  // Set the SO_KEEPALIVE option.
  asio::socket_base::keep_alive option{true};
  _socket->set_option(option);

  // Set the write timeout option.
  if (_write_timeout >= 0) {

    struct timeval t;
    t.tv_sec = _write_timeout / 1000;
    t.tv_usec = _write_timeout % 1000;
    ::setsockopt(
        _socket->native_handle(),
        SOL_SOCKET,
        SO_SNDTIMEO,
        &t,
        sizeof(t));
  }
  return ;
}
