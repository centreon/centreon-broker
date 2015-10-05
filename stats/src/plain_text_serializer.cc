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

#include "com/centreon/broker/stats/plain_text_serializer.hh"
#include "com/centreon/broker/stats/metric.hh"

using namespace com::centreon::broker::stats;

/**
 *  Default constructor.
 */
plain_text_serializer::plain_text_serializer() {}

/**
 *  Destructor.
 */
plain_text_serializer::~plain_text_serializer() {}

/**
 *  Serialize a property tree into a buffer.
 *
 *  @param[out] buffer  The output buffer.
 *  @param[in] tree     The tree.
 */
void plain_text_serializer::serialize(
                              std::string& buffer,
                              io::properties const& tree) const {
  _serialize(buffer, tree, 0);
}

/**
 *  Serialize a property tree into a buffer, implementation.
 *
 *  @param[out] buffer  The output buffer.
 *  @param[in] tree     The tree.
 *  @param[in] indent   The indent to use.
 */
void plain_text_serializer::_serialize(
                              std::string& buffer,
                              io::properties const& tree,
                              unsigned int indent) const {
  std::string indent_string(indent * 2, ' ');
  for (io::properties::const_iterator
         it(tree.begin()),
         end(tree.end());
       it != end;
       ++it) {
    buffer.append(indent_string);
    buffer.append(it->second.get_name());
    buffer.append("=");
    buffer.append(it->second.get_value());
    buffer.append("\n");
  }
  if (tree.children().size() != 0) {
    for (io::properties::children_list::const_iterator
           it = tree.children().begin(),
           end = tree.children().end();
         it != end;
         ++it) {
      if (!it->first.empty())
        buffer.append(indent_string).append(it->first);
        buffer.append("=").append("\n");
      _serialize(buffer, it->second, indent + 1);
    }
  }
  return ;
}
