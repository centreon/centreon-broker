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

#include <cstdio>
#include <QMutexLocker>
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] file  Splitted file on which the stream will operate.
 */
stream::stream(splitter* file) : _file(file) {}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Read data from the file.
 *
 *  @param[out] d         Bunch of data.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Always true as file never times out.
 */
bool stream::read(
               misc::shared_ptr<io::data>& d,
               time_t deadline) {
  (void)deadline;

  // Lock mutex.
  d.clear();
  QMutexLocker lock(&_mutex);

  // Build data array.
  std::auto_ptr<io::raw> data(new io::raw);
  data->resize(BUFSIZ);

  // Read data.
  long rb(_file->read(data->QByteArray::data(), data->size()));
  if (rb) {
    data->resize(rb);
    d = misc::shared_ptr<io::data>(data.release());
  }

  return (true);
}

/**
 *  Generate statistics about file processing.
 *
 *  @param[out] buffer Output buffer.
 */
void stream::statistics(io::properties& tree) const {
  // std::ostringstream oss;
  // // Easy to print.
  // {
  //   tree.add_property(
  //          "file_read_path",
  //          io::property("file_read_path", misc::string::get(_rid)));
  // }
  // {
  //   tree.add_property(
  //          "file_read_offset",
  //          io::property("file_read_offset", misc::string::get(_roffset)));
  // }
  // {
  //   tree.add_property(
  //          "file_write_path",
  //          io::property("file_write_path", misc::string::get(_wid)));
  // }
  // {
  //   tree.add_property(
  //          "file_write_offset",
  //          io::property("file_write_offset", misc::string::get(_woffset)));
  // }
  // {
  //   tree.add_property(
  //          "file_max_size",
  //          io::property(
  //                "file_max_size",
  //                _max_size != std::numeric_limits<long>::max()
  //                ? misc::string::get(_max_size)
  //                : "unlimited"));
  // }

  // // Need computation.
  // bool write_time_expected(false);
  // {
  //   io::property& p(tree["file_percent_processed"]);
  //   p.set_name("file_percent_processed");
  //   oss.str("");
  //   if (_rid != _wid
  //       && _max_size == std::numeric_limits<long>::max()) {
  //     oss << "unknown";
  //   }
  //   else {
  //     oss << (_roffset * 100.0) / (_woffset + (_wid - _rid) * _max_size)
  //         << "%";
  //     write_time_expected = true;
  //   }
  //   p.set_value(oss.str());
  // }
  // if (write_time_expected) {
  //   time_t now(time(NULL));
  //   unsigned long long roffset(_roffset + _rid * _max_size);
  //   unsigned long long woffset(_woffset + _wid * _max_size);

  //   if (_last_time && (now != _last_time)) {
  //     time_t eta(0);
  //     {
  //       io::property& p(tree["file_expected_terminated_at"]);
  //       oss.str("");
  //       p.set_name("file_expected_terminated_at");

  //       unsigned long long
  //         div(roffset + _last_write_offset - _last_read_offset - woffset);
  //       if (div == 0)
  //         oss << "file not processed fast enough to terminate";
  //       else {
  //         eta = now + (woffset - roffset) * (now - _last_time) / div;
  //         oss << eta;
  //       }
  //       p.set_value(oss.str());
  //     }

  //     if (_max_size == std::numeric_limits<long>::max()) {
  //       io::property& p(tree["file_expected_max_size"]);
  //       oss.str("");
  //       p.set_name("file_expected_max_size");
  //       oss << woffset
  //              + (woffset - _last_write_offset)
  //              * (eta - now)
  //              / (now - _last_time);

  //       p.set_value(oss.str());
  //       p.set_graphable(false);
  //     }
  //   }

  //   _last_time = now;
  //   _last_read_offset = roffset;
  //   _last_write_offset = woffset;
  // }

  return ;
}

/**
 *  Write data to the file.
 *
 *  @param[in] d  Data to write.
 *
 *  @return Number of events acknowledged (1).
 */
int stream::write(misc::shared_ptr<io::data> const& d) {
  // Check that data exists.
  if (!validate(d, "file"))
    return (1);

  if (d->type() == io::raw::static_type()) {
    // Lock mutex.
    QMutexLocker lock(&_mutex);

    // Get data.
    char const* memory;
    unsigned int size;
    {
      io::raw* data(static_cast<io::raw*>(d.data()));
      memory = data->QByteArray::data();
      size = data->size();
    }

    // Write data.
    while (size > 0) {
      unsigned long wb(_file->write(memory, size));
      size -= wb;
      memory += wb;
    }
  }

  return (1);
}
