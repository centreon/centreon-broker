/*
** Copyright 2011-2012 Centreon
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

#include "com/centreon/broker/io/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
stream::stream() {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) {
  _from = s._from;
  _to = s._to;
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  if (&s != this) {
    _from = s._from;
    _to = s._to;
  }
  return (*this);
}

/**
 *  @brief Read with timeout.
 *
 *  Default implementation calls read with no timeout.
 *
 *  @param[out] d         Data.
 *  @param[in]  deadline  Read timeout (unused in default
 *                        implementation).
 *  @param[out] timed_out Set to true if reading timed out, false
 *                        otherwise.
 */
void stream::read(
               misc::shared_ptr<data>& d,
               time_t deadline,
               bool* timed_out) {
  (void)deadline;
  if (timed_out)
    *timed_out = false;
  this->read(d);
  return ;
}

/**
 *  Read from another stream.
 *
 *  @param[in] from Stream to read from.
 */
void stream::read_from(misc::shared_ptr<stream> from) {
  _from = from;
  return ;
}

/**
 *  Generate statistics about the stream.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  (void)tree;
  return ;
}

/**
 *  Configuration update.
 */
void stream::update() {
  return ;
}

/**
 *  Write to another stream.
 *
 *  @param[in] to Stream to write to.
 */
void stream::write_to(misc::shared_ptr<stream> to) {
  _to = to;
  return ;
}
