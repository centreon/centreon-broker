/*
** Copyright 2015,2020 Centreon
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

#ifndef CCB_MULTIPLEXING_SUBSCRIBER_HH
#define CCB_MULTIPLEXING_SUBSCRIBER_HH

#include <memory>
#include <string>

#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace multiplexing {

/**
 *  @class subscriber subscriber.hh
 * "com/centreon/broker/multiplexing/subscriber.hh"
 *  @brief Handle registration of a muxer.
 *
 *  Handle registration of a muxer against the multiplexing engine.
 */
class subscriber {
  muxer _muxer;

 public:
  /**
   *  Constructor.
   *
   *  @param[in] name        Name associated to the muxer.
   *  @param[in] persistent  Whether or not the muxer is persistent.
   */
  subscriber(std::string const& name, bool persistent = false)
      : _muxer(name, persistent) {
    multiplexing::engine::instance().subscribe(&_muxer);
  }

  /**
   *  Destructor.
   */
  ~subscriber() { multiplexing::engine::instance().unsubscribe(&_muxer); }

  muxer& get_muxer() { return _muxer; }
  const muxer& get_muxer() const { return _muxer; }
  subscriber(subscriber const&) = delete;
  subscriber& operator=(subscriber const&) = delete;
};
}  // namespace multiplexing

CCB_END()

#endif  // !CCB_MULTIPLEXING_SUBSCRIBER_HH
