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

#include "com/centreon/broker/file/stream.hh"
#include <cstdio>
#include <limits>
#include <sstream>
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
    : io::stream("file"), _file(file), _last_read_offset(0), _last_time(0), _last_write_offset(0) {}

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
  return oss.str();
}

/**
 *  Read data from the file.
 *
 *  @param[out] d         Bunch of data.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Always true as file never times out.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;

  d.reset();

  // Build data array.
  std::unique_ptr<io::raw> data(new io::raw);
  data->resize(BUFSIZ);

  // Read data.
  long rb(_file->read(data->data(), data->size()));
  if (rb) {
    data->resize(rb);
    d.reset(data.release());
  }

  return true;
}

/**
 *  Generate statistics about file processing.
 *
 *  @param[out] buffer Output buffer.
 */
void stream::statistics(json11::Json::object& tree) const {
  // Get base properties.
  long max_file_size(_file->get_max_file_size());
  int rid(_file->get_rid());
  long roffset(_file->get_roffset());
  int wid(_file->get_wid());
  long woffset(_file->get_woffset());

  // Easy to print.
  tree["file_read_path"] = rid;
  tree["file_read_offset"] = static_cast<double>(roffset);
  tree["file_write_path"] =  wid;
  tree["file_write_offset"] = static_cast<double>(woffset);
  if (max_file_size != std::numeric_limits<long>::max())
    tree["file_max_size"] = static_cast<double>(max_file_size);
  else
    tree["file_max_size"] = "unlimited";

  // Need computation.
  bool write_time_expected(false);
  long long froffset(roffset + rid * static_cast<long long>(max_file_size));
  long long fwoffset(woffset + wid * static_cast<long long>(max_file_size));
  if (((rid != wid) && max_file_size == std::numeric_limits<long>::max()) ||
      !fwoffset) {
    tree["file_percent_processed"] = "unknown";
  } else {
    tree["file_percent_processed"] = 100.0 * froffset / fwoffset;
    write_time_expected = true;
  }
  if (write_time_expected) {
    time_t now(time(nullptr));

    if (_last_time && (now != _last_time)) {
      time_t eta(0);
      {
        unsigned long long div(froffset + _last_write_offset -
                               _last_read_offset - fwoffset);
        if (div == 0)
          tree["file_expected_terminated_at"] =
              "file not processed fast enough to terminate";
        else {
          eta = now + (fwoffset - froffset) * (now - _last_time) / div;
          tree["file_expected_terminated_at"] = static_cast<double >(eta);
        }
      }

      if (max_file_size == std::numeric_limits<long>::max()) {
        tree["file_expected_max_size"] = static_cast<double>(
            fwoffset + (fwoffset - _last_write_offset) * (eta - now) /
                              (now - _last_time));
      }
    }

    _last_time = now;
    _last_read_offset = froffset;
    _last_write_offset = fwoffset;
  }

  return;
}

/**
 *  Write data to the file.
 *
 *  @param[in] d  Data to write.
 *
 *  @return Number of events acknowledged (1).
 */
int stream::write(std::shared_ptr<io::data> const& d) {
  // Check that data exists.
  if (!validate(d, get_name()))
    return 1;

  if (d->type() == io::raw::static_type()) {
    // Get data.
    char const* memory;
    uint32_t size;
    {
      io::raw* data(static_cast<io::raw*>(d.get()));
      memory = data->data();
      size = data->size();
    }

    // Write data.
    while (size > 0) {
      long wb(_file->write(memory, size));
      size -= wb;
      memory += wb;
    }
  }

  return 1;
}

/**
 *  Remove all the files this stream in concerned by.
 */
void stream::remove_all_files() {
  _file->remove_all_files();
}
