/*
** Copyright 2013,2017 Centreon
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

#ifndef CCB_BBDO_INPUT_HH
#define CCB_BBDO_INPUT_HH

#include <memory>
#include <string>
#include "com/centreon/broker/bbdo/input_buffer.hh"
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bbdo {
/**
 *  @class input input.hh "com/centreon/broker/bbdo/input.hh"
 *  @brief BBDO input source.
 *
 *  The class converts an input stream into events using the BBDO
 *  (Broker Binary Data Objects) protocol.
 */
class input : virtual public io::stream {
 public:
  input();
  input(input const& other);
  virtual ~input();
  input& operator=(input const& other);
  virtual bool read(std::shared_ptr<io::data>& d, time_t deadline = (time_t)-1);
  bool read_any(std::shared_ptr<io::data>& d, time_t deadline = (time_t)-1);
  virtual void acknowledge_events(unsigned int events) = 0;

 private:
  void _buffer_must_have_unprocessed(int bytes, time_t deadline = (time_t)-1);

  input_buffer _buffer;
  int _skipped;
};
}  // namespace bbdo

CCB_END()

#endif  // !CCB_BBDO_INPUT_HH
