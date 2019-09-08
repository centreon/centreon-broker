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

#ifndef CCB_IO_DATA_HH
#define CCB_IO_DATA_HH

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace io {
/**
 *  @class data data.hh "com/centreon/broker/io/data.hh"
 *  @brief Data abstraction.
 *
 *  Data is the core element that is transmitted through Centreon
 *  Broker. It is an interface that is implemented by all specific
 *  module data that wish to be transmitted by the multiplexing
 *  engine.
 */
class data {
 public:
  data();
  data(data const& other);
  virtual ~data();
  data& operator=(data const& other);
  virtual unsigned int type() const = 0;

  unsigned int source_id;
  unsigned int destination_id;

  static unsigned int broker_id;
};
}  // namespace io

CCB_END()

#endif  // !CCB_IO_DATA_HH
