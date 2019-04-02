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
#include <limits>
#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/file/stream.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/misc/string.hh"

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
stream::stream(splitter* file)
  : _file(file),
    _last_read_offset(0),
    _last_time(0),
    _last_write_offset(0) {}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Get peer name.
 *
 *  @return Peer name.
 */
std::string stream::peer() const {
  std::ostringstream oss;
  oss << "file://" << _file->get_file_path();
  return (oss.str());
}

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
  std::unique_ptr<io::raw> data(new io::raw);
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
  // Get base properties.
  long max_file_size(_file->get_max_file_size());
  int rid(_file->get_rid());
  long roffset(_file->get_roffset());
  int wid(_file->get_wid());
  long woffset(_file->get_woffset());

  // Easy to print.
  std::ostringstream oss;
  {
    tree.add_property(
           "file_read_path",
           io::property("file_read_path", misc::string::get(rid)));
  }
  {
    tree.add_property(
           "file_read_offset",
           io::property("file_read_offset", misc::string::get(roffset)));
  }
  {
    tree.add_property(
           "file_write_path",
           io::property("file_write_path", misc::string::get(wid)));
  }
  {
    tree.add_property(
           "file_write_offset",
           io::property("file_write_offset", misc::string::get(woffset)));
  }
  {
    tree.add_property(
           "file_max_size",
           io::property(
                 "file_max_size",
                 max_file_size != std::numeric_limits<long>::max()
                 ? misc::string::get(max_file_size)
                 : "unlimited"));
  }

  // Need computation.
  bool write_time_expected(false);
  long long froffset(roffset + rid * static_cast<long long>(max_file_size));
  long long fwoffset(woffset + wid * static_cast<long long>(max_file_size));
  {
    io::property& p(tree["file_percent_processed"]);
    p.set_name("file_percent_processed");
    oss.str("");
    if (((rid != wid)
         && max_file_size == std::numeric_limits<long>::max())
        || !fwoffset) {
      oss << "unknown";
    }
    else {
      oss << 100.0 * froffset / fwoffset << "%";
      write_time_expected = true;
    }
    p.set_value(oss.str());
  }
  if (write_time_expected) {
    time_t now(time(NULL));

    if (_last_time && (now != _last_time)) {
      time_t eta(0);
      {
        io::property& p(tree["file_expected_terminated_at"]);
        oss.str("");
        p.set_name("file_expected_terminated_at");

        unsigned long long
          div(froffset + _last_write_offset - _last_read_offset - fwoffset);
        if (div == 0)
          oss << "file not processed fast enough to terminate";
        else {
          eta = now + (fwoffset - froffset) * (now - _last_time) / div;
          oss << eta;
        }
        p.set_value(oss.str());
      }

      if (max_file_size == std::numeric_limits<long>::max()) {
        io::property& p(tree["file_expected_max_size"]);
        oss.str("");
        p.set_name("file_expected_max_size");
        oss << fwoffset
               + (fwoffset - _last_write_offset)
               * (eta - now)
               / (now - _last_time);

        p.set_value(oss.str());
        p.set_graphable(false);
      }
    }

    _last_time = now;
    _last_read_offset = froffset;
    _last_write_offset = fwoffset;
  }

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
      long wb(_file->write(memory, size));
      size -= wb;
      memory += wb;
    }
  }

  return (1);
}

/**
 *  Remove all the files this stream in concerned by.
 */
void stream::remove_all_files() {
  _file->remove_all_files();
}
