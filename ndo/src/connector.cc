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

#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"
#include "com/centreon/broker/ndo/connector.hh"
#include "com/centreon/broker/ndo/input.hh"
#include "com/centreon/broker/ndo/output.hh"
#include "com/centreon/broker/ndo/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::ndo;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] is_in  Connector should act as input.
 *  @param[in] is_out Connector should act as output.
 */
connector::connector(bool is_in, bool is_out)
  : io::endpoint(false), _is_in(is_in), _is_out(is_out) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c)
  : io::endpoint(c), _is_in(c._is_in), _is_out(c._is_out) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  io::endpoint::operator=(c);
  _is_in = c._is_in;
  _is_out = c._is_out;
  return (*this);
}

/**
 *  Clone the connector.
 *
 *  @return This object.
 */
io::endpoint* connector::clone() const {
  std::auto_ptr<connector> copy(new connector(*this));
  if (!_from.isNull())
    copy->_from = _from->clone();
  return (copy.release());
}

/**
 *  Close the connector.
 */
void connector::close() {
  return ;
}

/**
 *  Open the connector.
 */
misc::shared_ptr<io::stream> connector::open() {
  misc::shared_ptr<io::stream> retval;
  if (!_from.isNull())
    retval = _open(_from->open());
  return (retval);
}

/**
 *  Open the connector.
 */
misc::shared_ptr<io::stream> connector::open(QString const& id) {
  misc::shared_ptr<io::stream> retval;
  if (!_from.isNull())
    retval = _open(_from->open(id));
  return (retval);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Open the connector.
 */
misc::shared_ptr<io::stream> connector::_open(
  misc::shared_ptr<io::stream> stream) {
  misc::shared_ptr<io::stream> ndo_stream;
  if (!stream.isNull()) {
    if (_is_in) {
      if (_is_out)
        ndo_stream = misc::shared_ptr<io::stream>(new ndo::stream);
      else
        ndo_stream = misc::shared_ptr<io::stream>(new ndo::input);
    }
    else
      ndo_stream = misc::shared_ptr<io::stream>(new ndo::output);
    ndo_stream->read_from(stream);
    ndo_stream->write_to(stream);
  }
  return (ndo_stream);
}
