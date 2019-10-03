/*
** Copyright 2009-2012,2017 Centreon
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

#include "com/centreon/broker/logging/file.hh"
#include <climits>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::logging;

// Messages.
#define LOG_CLOSE_STR \
  "Centreon Broker " CENTREON_BROKER_VERSION " log file closed\n"
#define LOG_OPEN_STR \
  "Centreon Broker " CENTREON_BROKER_VERSION " log file opened\n"
#define LOG_ROTATION_STR \
  "Centreon Broker " CENTREON_BROKER_VERSION " log file rotation\n"

// Should file flush output at each log entry.
bool file::_with_flush(true);
// Should thread ID be printed ?
bool file::_with_thread_id(false);
// Should/how timestamp be printed ?
timestamp_type file::_with_timestamp(second_timestamp);
// Should a human readable timestamp be printed?
bool file::_with_human_readable_timestamp(false);

/**************************************
 *                                     *
 *           Local objects.            *
 *                                     *
 **************************************/

// These templates are used to compute the maximum printing size of any
// integer.
template <unsigned long long ll>
struct ll_width {
  static uint32_t const value = 1 + ll_width<ll / 10>::value;
};
template <>
struct ll_width<0ull> {
  static uint32_t const value = 1;
};
template <typename T>
struct integer_width {
  static uint32_t const value =
      2 + ll_width<((((1ull << (sizeof(T) * 8 - 1)) - 1) << 1) | 1)>::value;
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
file::file(std::string const& path, uint64_t max)
    : _file_special(std::cout),
      _filename{path},
      _max(0),
      _special(false),
      _written(0) {
  try {
    _file.open(_filename, std::ofstream::out | std::ofstream::app);
  } catch (std::system_error const& se) {
    throw exceptions::msg()
        << "log: could not open file '" << path << "': " << se.what();
  }

  if (!max)
    _max = ULLONG_MAX;
  else
    _max = ((max < 1000000) ? 1000000 : max) - sizeof(LOG_ROTATION_STR) + 1;
  _write(LOG_OPEN_STR);
  _file.flush();
  _written = _file.tellp();
}

file::file(std::ostream& stream, std::string const& filename)
    : _file_special(stream), _filename{filename}, _special(true) {}

/**
 *  Destructor.
 */
file::~file() {
  if (!_special)
    _write(LOG_CLOSE_STR);
  if (_special) {
    _file_special.flush();
  } else {
    _file.flush();
    _file.close();
  }
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
                   uint32_t len,
                   type log_type,
                   level l) throw() {
  (void)len;
  (void)l;
  if (msg) {
    char const* prefix;
    switch (log_type) {
      case config_type:
        prefix = "config:  ";
        break;
      case debug_type:
        prefix = "debug:   ";
        break;
      case error_type:
        prefix = "error:   ";
        break;
      case info_type:
        prefix = "info:    ";
        break;
      case perf_type:
        prefix = "perf:    ";
        break;
      default:
        prefix = "unknown: ";
    }
    if ((_with_timestamp != no_timestamp) || _with_human_readable_timestamp) {
      struct timespec ts;
      memset(&ts, 0, sizeof(ts));
      clock_gettime(CLOCK_REALTIME, &ts);
      _write("[");
      // 10 comes from the limits of nanoseconds (9) + dot (1).
      char buffer[integer_width<time_t>::value + 10];
      if (_with_timestamp == nano_timestamp)
        snprintf(buffer, sizeof(buffer), "%llu.%09li",
                 static_cast<unsigned long long>(ts.tv_sec), ts.tv_nsec);
      else
        snprintf(buffer, sizeof(buffer), "%llu",
                 static_cast<unsigned long long>(ts.tv_sec));
      _write(buffer);
      _write("] ");
      if (_with_human_readable_timestamp) {
        _write("[");
        time_t now = std::time(nullptr);
        // ctime never write more than 26 characters
        char human_readable_date[26];
        ctime_r(&now, human_readable_date);
        _write(human_readable_date);
        _write("] ");
      }
    }
    if (_with_thread_id) {
      _write("[");
      // 2 characters for 0x
      char buffer[integer_width<unsigned long long>::value + 2];
      snprintf(buffer, sizeof(buffer), "0x%llx",
               (unsigned long long)(pthread_self()));
      _write(buffer);
      _write("] ");
    }
    _write(prefix);
    _write(msg);
    if (_with_flush) {
      if (_special)
        _file_special.flush();
      else
        _file.flush();
    }
  }
}

/**
 *  Check if files should be flushed at each entry.
 *
 *  @return true if files should be flushed.
 */
bool file::with_flush() throw() {
  return (_with_flush);
}

/**
 *  Set if files should be flushed at each entry.
 *
 *  @param[in] enable true to enable file flushing.
 */
void file::with_flush(bool enable) throw() {
  _with_flush = enable;
}

/**
 *  Check if thread ID should be printed.
 *
 *  @return true if thread ID should be printed.
 */
bool file::with_thread_id() throw() {
  return (_with_thread_id);
}

/**
 *  Set if thread ID should be printed or not.
 *
 *  @param[in] enable true to enable thread ID printing.
 */
void file::with_thread_id(bool enable) throw() {
  _with_thread_id = enable;
}

/**
 *  Check if and how timestamp should be printed.
 *
 *  @return Any acceptable value.
 */
timestamp_type file::with_timestamp() throw() {
  return (_with_timestamp);
}

/**
 *  Set if and how timestamp should be printed.
 *
 *  @param[in] ts_type  Any acceptable value.
 */
void file::with_timestamp(timestamp_type ts_type) throw() {
  _with_timestamp = ts_type;
}

/**
 *  Check if a human readable timestamp should be printed.
 *
 *  @return true if a human readable timestamp should be printed.
 */
bool file::with_human_redable_timestamp() throw() {
  return (_with_human_readable_timestamp);
}

/**
 *  Set if a human readable timestamp should be printed.
 *
 *  @param[in] enable  true to enable human readable timestamp printing.
 */
void file::with_human_redable_timestamp(bool enable) throw() {
  _with_human_readable_timestamp = enable;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

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
  std::string backup_name(_filename);
  backup_name.append(".old");
  std::remove(backup_name.c_str());
  std::rename(_filename.c_str(), backup_name.c_str());

  // Reopen file.
  _file.open(_filename, std::ofstream::out | std::ostream::trunc);
  _written = 0;
  _write(LOG_OPEN_STR);
}

/**
 *  Write an amount of data to the file.
 *
 *  @param[in] data Data to write.
 */
void file::_write(char const* data) throw() {
  // Check sizes.
  std::streamsize to_write(strlen(data));
  if (!_special && (_written + to_write > _max))
    _reopen();

  // Write data.
  if (_special)
    _file_special.write(data, to_write);
  else
    _file.write(data, to_write);
  _written += to_write;
}
