/*
** Copyright 2013,2015,2017 Centreon
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

#ifndef CCB_BBDO_OUTPUT_HH
#define CCB_BBDO_OUTPUT_HH

#include <memory>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bbdo {
/**
 *  @class output output.hh "com/centreon/broker/bbdo/output.hh"
 *  @brief BBDO output destination.
 *
 *  The class converts events to an output stream using the BBDO
 *  (Broker Binary Data Objects) protocol.
 */
class output : virtual public io::stream {
 public:
  output();
  output(output const& other) = delete;
  virtual ~output();
  output& operator=(output const& other) = delete;
  int flush();
  void statistics(io::properties& tree) const;
  virtual int write(std::shared_ptr<io::data> const& e);
};
}  // namespace bbdo

CCB_END()

#endif  // !CCB_BBDO_OUTPUT_HH
