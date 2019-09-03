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

#include "temporary_endpoint.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "temporary_stream.hh"

using namespace com::centreon::broker;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] id The temporary id.
 */
temporary_endpoint::temporary_endpoint(std::string const& id)
    : io::endpoint(false), _id(id) {}

/**
 *  Copy constructor.
 *
 *  @param[in] se Object to copy.
 */
temporary_endpoint::temporary_endpoint(temporary_endpoint const& se)
    : io::endpoint(se), _id(se._id) {}

/**
 *  Destructor.
 */
temporary_endpoint::~temporary_endpoint() {
  this->close();
}

/**
 *  Assignment operator.
 *
 *  @param[in] se Object to copy.
 *
 *  @return This object.
 */
temporary_endpoint& temporary_endpoint::operator=(
    temporary_endpoint const& se) {
  if (&se != this) {
    com::centreon::broker::io::endpoint::operator=(se);
    _id = se._id;
  }
  return *this;
}

/**
 *  Clone endpoint.
 */
com::centreon::broker::io::endpoint* temporary_endpoint::clone() const {
  return new temporary_endpoint(*this);
}

/**
 *  Close endpoint.
 */
void temporary_endpoint::close() {}

/**
 *  Open endpoint.
 *
 *  @return New temporary_stream.
 */
std::shared_ptr<io::stream> temporary_endpoint::open() {
  return std::shared_ptr<io::stream>(new temporary_stream);
}
