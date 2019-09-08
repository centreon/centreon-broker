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

#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/muxer.hh"

using namespace com::centreon::broker::multiplexing;

/**
 *  Constructor.
 *
 *  @param[in] name        Name associated to the muxer.
 *  @param[in] persistent  Whether or not the muxer is persistent.
 */
subscriber::subscriber(std::string const& name, bool persistent)
    : _muxer(new muxer(name, persistent)) {
  multiplexing::engine::instance().subscribe(_muxer.get());
}

/**
 *  Destructor.
 */
subscriber::~subscriber() {
  multiplexing::engine::instance().unsubscribe(_muxer.get());
}

/**
 *  Get muxer.
 *
 *  @return Muxer.
 */
muxer& subscriber::get_muxer() const {
  return (*_muxer);
}
