/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/ceof/ceof_serializer.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::ceof;

/**
 *  Constructor.
 *
 *  @param[in,out] writer        The writer to use.
 */
ceof_serializer::ceof_serializer(ceof_writer& writer)
    : ceof_visitor(true, false), _writer(writer) {}

/**
 *  Destructor.
 */
ceof_serializer::~ceof_serializer() throw() {}

/**
 *  Serialize.
 *
 *  @param[in] name  The name.
 *  @param[in] value The value.
 */
void ceof_serializer::serialize(std::string const& name,
                                std::string const& value) {
  _writer.add_key(name);
  _writer.add_value(value);
}
