/*
** Copyright 2011-2013,2017 Centreon
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

#include "com/centreon/broker/io/factory.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::io;

/**
 *  @brief Create a new stream.
 *
 *  This function is used to generate new streams after successful
 *  stream construction like for a feature negotiation.
 *
 *  @param[in] to          Stream on which the stream will work.
 *  @param[in] is_acceptor true if stream must be an accepting stream.
 *  @param[in] proto_name  Protocol name.
 *
 *  @return New stream.
 */
std::shared_ptr<stream> factory::new_stream(std::shared_ptr<stream> to,
                                            bool is_acceptor,
                                            std::string const& proto_name) {
  (void)to;
  (void)is_acceptor;
  throw exceptions::msg() << proto_name
                          << ": protocol does not support feature negotiation";
  return std::shared_ptr<stream>();
}
