/*
** Copyright 2009-2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <QThread>
#include <stdlib.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/file.hh"

using namespace com::centreon::broker::logging;

// Messages.
#define LOG_CLOSE_STR "Centreon Broker log file closed\n"
#define LOG_OPEN_STR "Centreon Broker log file opened\n"
#define LOG_ROTATION_STR "Centreon Broker log file rotation\n"

// Should thread ID be printed ?
bool file::_with_thread_id(false);
// Should timestamp be printed ?
bool file::_with_timestamp(true);

/**************************************
*                                     *
*           Local objects.            *
*                                     *
**************************************/

// These templates are used to compute the maximum printing size of any
// integer.
template <unsigned long long ll>
struct ll_width {
  static unsigned int const value = 1 + ll_width<ll / 10>::value;
};
template <>
struct ll_width<0ull> {
  static unsigned int const value = 1;
};
template <typename T>
struct integer_width {
  static unsigned int const value
    = 2 + ll_width<((((1ull << (sizeof(T) * 8 - 1)) - 1) << 1) | 1)>::value;
};

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Regular file constructor.
 *
 *  @param[in] path Path to the log file.
 *  @param[in] max  Maximum file size of log file.
 */
file::file(QString const& path, unsigned long long max)
  : _file(path), _max(0), _special(false), _written(0) {
  if (!_file.open(QIODevice::WriteOnly | QIODevice::Append))
    throw (exceptions::msg() << "log: could not open file '" << path
             << "': " << _file.errorString());
  _max = ((max < 1000000) ? 1000000 : max)
         - sizeof(LOG_ROTATION_STR) + 1;
  _write(LOG_OPEN_STR);
  _file.flush();
  _written = _file.size();
}

/**
 *  Special file constructor.
 *
 *  @param[in] special Special file handle.
 */
file::file(FILE* special) : _special(true) {
  if (!_file.open(special, QIODevice::WriteOnly))
    throw (exceptions::msg() << "log: could not open special file: "
             << _file.errorString());
}

/**
 *  Destructor.
 */
file::~file() {
  if (!_special)
    _write(LOG_CLOSE_STR);
  _file.flush();
  _file.close();
}

/**
 *  Write log message to stream.
 *
 *  @param[in] msg      Log message.
 *  @param[in] len      Message length.
 *  @param[in] log_type Type of the log message.
 *  @param[in] l        Log level.
 */
void file::log_msg(char const* msg,
                   unsigned int len,
                   type log_type,
                   level l) throw () {
  (void)len;
  (void)l;
  if (msg) {
    char const* prefix;
    switch (log_type) {
     case config_type:
      prefix = "config:  ";
      break ;
     case debug_type:
      prefix = "debug:   ";
      break ;
     case error_type:
      prefix = "error:   ";
      break ;
     case info_type:
      prefix = "info:    ";
      break ;
     default:
      prefix = "unknown: ";
    }
    if (_with_timestamp) {
      _write("[");
      char buffer[integer_width<time_t>::value];
      snprintf(buffer,
        sizeof(buffer),
        "%llu",
        static_cast<unsigned long long>(time(NULL)));
      _write(buffer);
      _write("] ");
    }
    if (_with_thread_id) {
      _write("[");
      // 2 characters for 0x
      char buffer[integer_width<QThread*>::value + 2];
      snprintf(buffer,
        sizeof(buffer),
        "0x%llx",
        (unsigned long long)(QThread::currentThread()));
      _write(buffer);
      _write("] ");
    }
    _write(prefix);
    _write(msg);
    _file.flush();
  }
  return ;
}

/**
 *  Check if thread ID should be printed.
 *
 *  @return true if thread ID should be printed.
 */
bool file::with_thread_id() {
  return (_with_thread_id);
}

/**
 *  Set if thread ID should be printed or not.
 *
 *  @param[in] enable true to enable thread ID printing.
 */
void file::with_thread_id(bool enable) {
  _with_thread_id = enable;
  return ;
}

/**
 *  Check if timestamp should be printed.
 *
 *  @return true if timestamp should be printed.
 */
bool file::with_timestamp() {
  return (_with_timestamp);
}

/**
 *  Set if timestamp should be printed or not.
 *
 *  @param[in] enable true to enable timestamp printing.
 */
void file::with_timestamp(bool enable) {
  _with_timestamp = enable;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy constructor.
 *
 *  @param[in] f Unused.
 */
file::file(file const& f) : backend() {
  _internal_copy(f);
}

/**
 *  Assignment operator.
 *
 *  @param[in] f Unused.
 *
 *  @return This object.
 */
file& file::operator=(file const& f) {
  _internal_copy(f);
  return (*this);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] f Unused.
 */
void file::_internal_copy(file const& f) {
  (void)f;
  assert(!"logging file is not copyable");
  abort();
  return ;
}

/**
 *  Reopen log file.
 */
void file::_reopen() {
  // Write last log line.
  _written = 0;
  _write(LOG_ROTATION_STR);
  _file.flush();
  _file.close();

  // Move log file to .old
  QString backup_name(_file.fileName());
  backup_name.append(".old");
  QFile::remove(backup_name);
  QFile::rename(_file.fileName(), backup_name);

  // Reopen file.
  _file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  _written = 0;
  _write(LOG_OPEN_STR);

  return ;
}

/**
 *  Write an amount of data to the file.
 *
 *  @param[in] data Data to write.
 */
void file::_write(char const* data) throw () {
  // Check sizes.
  qint64 to_write(strlen(data));
  if (!_special && (_written + to_write > _max))
    _reopen();

  // Write data.
  qint64 wb(_file.write(data, to_write));
  to_write -= wb;
  data += wb;
  if (wb > 0)
    _written += wb;
  while ((to_write > 0) && (wb >= 0)) {
    _file.waitForBytesWritten(-1);
    wb = _file.write(data, to_write);
    to_write -= wb;
    data += wb;
    if (wb > 0)
      _written += wb;
  }

  return ;
}
