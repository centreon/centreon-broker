/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_RRD_BACKEND_HH
#define CCB_RRD_BACKEND_HH

#include <ctime>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace rrd {
/**
 *  @class backend backend.hh "com/centreon/broker/rrd/backend.hh"
 *  @brief Generic access to RRD files.
 *
 *  Provide a unified access to RRD files. Files can be accessed
 *  either through librrd or with rrdcached.
 *
 *  @see rrd::lib
 *  @see rrd::cached
 */
class backend {
 public:
  backend();
  backend(backend const& b) = delete;
  virtual ~backend();
  backend& operator=(backend const& b) = delete;
  virtual void begin() = 0;
  virtual void clean() = 0;
  virtual void close() = 0;
  virtual void commit() = 0;
  virtual void open(std::string const& filename) = 0;
  virtual void open(std::string const& filename,
                    uint32_t length,
                    time_t from,
                    uint32_t step,
                    short value_type = 0) = 0;
  virtual void remove(std::string const& filename) = 0;
  virtual void update(time_t t, std::string const& value) = 0;
};
}  // namespace rrd

CCB_END()

#endif  // !CCB_RRD_BACKEND_HH
