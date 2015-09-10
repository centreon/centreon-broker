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

#include <memory>
#include "com/centreon/broker/compression/opener.hh"
#include "com/centreon/broker/compression/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::compression;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
opener::opener() : io::endpoint(false), _level(-1), _size(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
opener::opener(opener const& o)
  : io::endpoint(o),
    _level(o._level),
    _size(o._size) {}

/**
 *  Destructor.
 */
opener::~opener() {}

/**
 *  Assignment operator.
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */
opener& opener::operator=(opener const& o) {
  if (this != &o) {
    io::endpoint::operator=(o);
    _level = o._level;
    _size = o._size;
  }
  return (*this);
}

/**
 *  Clone the opener.
 *
 *  @return This object.
 */
io::endpoint* opener::clone() const {
  std::auto_ptr<opener> copy(new opener(*this));
  if (!_from.isNull())
    copy->_from = _from->clone();
  return (copy.release());
}

/**
 *  Close the opener.
 */
void opener::close() {
  if (!_from.isNull())
    _from->close();
  return ;
}

/**
 *  Open a compression stream.
 *
 *  @return New compression object.
 */
misc::shared_ptr<io::stream> opener::open() {
  misc::shared_ptr<io::stream> retval;
  if (!_from.isNull())
    retval = _open(_from->open());
  return (retval);
}

/**
 *  Open a compression stream.
 *
 *  @return New compression object.
 */
misc::shared_ptr<io::stream> opener::open(QString const& id) {
  misc::shared_ptr<io::stream> retval;
  if (!_from.isNull())
    retval = _open(_from->open(id));
  return (retval);
}

/**
 *  Set the compression level.
 *
 *  @param[in] level Compression level (between 1 and 9, default is -1).
 */
void opener::set_level(int level) {
  _level = level;
  return ;
}

/**
 *  Set the compression buffer size.
 *
 *  @param[in] size Compression buffer size (default is 0 which means no
 *                  buffering).
 */
void opener::set_size(unsigned int size) {
  _size = size;
  return ;
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Open a compression stream.
 *
 *  @return New compression object.
 */
misc::shared_ptr<io::stream> opener::_open(
  misc::shared_ptr<io::stream> base) {
  misc::shared_ptr<io::stream> retval;
  if (!base.isNull()) {
    retval = misc::shared_ptr<io::stream>(new stream(_level, _size));
    retval->read_from(base);
    retval->write_to(base);
  }
  return (retval);
}
