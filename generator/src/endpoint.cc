/*
** Copyright 2017 Centreon
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

#include "com/centreon/broker/generator/endpoint.hh"
#include "com/centreon/broker/generator/receiver.hh"
#include "com/centreon/broker/generator/sender.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::generator;

/**
 *  Constructor.
 *
 *  @param[in] type  Kind of stream to create in open().
 */
endpoint::endpoint(endpoint::endpoint_type type)
    : io::endpoint(false), _type(type) {}

/**
 *  Destructor.
 */
endpoint::~endpoint() {}

/**
 *  Open a new stream.
 *
 *  @return A new receiver or sender object.
 */
std::shared_ptr<io::stream> endpoint::open() {
  std::shared_ptr<io::stream> s;
  if (_type == type_receiver)
    s = std::make_shared<receiver>();
  else if (_type == type_sender)
    s = std::make_shared<sender>();
  return (s);
}
