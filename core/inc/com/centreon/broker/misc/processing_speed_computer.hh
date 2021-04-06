/*
** Copyright 2015 Centreon
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

#ifndef CCB_MISC_PROCESSING_SPEED_COMPUTER_HH
#define CCB_MISC_PROCESSING_SPEED_COMPUTER_HH

#include <array>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace misc {
/**
 *  @class processing_speed_computer processing_speed_computer.hh
 * "com/centreon/broker/misc/processing_speed_computer.hh"
 *  @brief Compute processing speed.
 */
class processing_speed_computer {
 public:
  processing_speed_computer();
  processing_speed_computer(processing_speed_computer const&) = delete;
  ~processing_speed_computer() noexcept {}
  processing_speed_computer& operator=(processing_speed_computer const&) =
      delete;

  double get_processing_speed() const;
  void tick(int events = 1) noexcept;
  timestamp get_last_event_time() const;

  static int const window_length = 30;

 private:
  std::array<uint32_t, window_length> _event_by_seconds;
  timestamp _last_tick;
};
}  // namespace misc

CCB_END()

#endif  // !CCB_MISC_PROCESSING_SPEED_COMPUTER_HH
