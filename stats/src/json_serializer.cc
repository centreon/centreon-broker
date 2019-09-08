/*
** Copyright 2013 Centreon
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

#include "com/centreon/broker/stats/json_serializer.hh"
#include "com/centreon/broker/misc/json_writer.hh"

using namespace com::centreon::broker::stats;

/**
 *  Default constructor.
 */
json_serializer::json_serializer() {}

/**
 *  Destructor.
 */
json_serializer::~json_serializer() {}

/**
 *  Serialize a property tree into a buffer.
 *
 *  @param[out] buffer  The output buffer.
 *  @param[in] tree     The tree.
 */
void json_serializer::serialize(std::string& buffer,
                                io::properties const& tree) const {
  misc::json_writer writer;
  writer.open_object();
  for (io::properties::const_iterator it = tree.begin(), end = tree.end();
       it != end; ++it) {
    writer.add_key(it->second.get_name());
    writer.add_string(it->second.get_value());
  }
  for (io::properties::children_list::const_iterator
           it = tree.children().begin(),
           end = tree.children().end();
       it != end; ++it) {
    writer.add_key(it->first);
    std::string sub;
    serialize(sub, it->second);
    writer.merge(sub);
  }
  writer.close_object();
  buffer.append(writer.get_string());
}
