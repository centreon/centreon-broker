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

#include <QCoreApplication>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/file/opener.hh"
#include "com/centreon/broker/file/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::file;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
opener::opener(bool is_in, bool is_out)
  : endpoint(false),
    _is_in(is_in),
    _is_out(is_out),
    _max_size(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
opener::opener(opener const& o)
  : io::endpoint(o),
    _filename(o._filename),
    _is_in(o._is_in),
    _is_out(o._is_out),
    _max_size(o._max_size) {}

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
  io::endpoint::operator=(o);
  _filename = o._filename;
  _is_in = o._is_in;
  _is_out = o._is_out;
  _max_size = o._max_size;
  return (*this);
}

/**
 *  Clone the opener.
 *
 *  @return This object.
 */
io::endpoint* opener::clone() const {
  return (new opener(*this));
}

/**
 *  Close the opener.
 */
void opener::close() {
  return ;
}

/**
 *  Open a new stream.
 *
 *  @return Opened stream.
 */
misc::shared_ptr<io::stream> opener::open() {
  QString filename(_filename);
  return (misc::shared_ptr<io::stream>(
            new stream(qPrintable(filename), _max_size)));
}

/**
 *  Open a new stream.
 *
 *  @param[in] id The identifier.
 *
 *  @return Opened stream.
 */
misc::shared_ptr<io::stream> opener::open(QString const& id) {
  return (misc::shared_ptr<io::stream>(
                  new stream(qPrintable(_filename + "-" + id), _max_size)));
}

/**
 *  Set the filename.
 *
 *  @param[in] filename Filename.
 */
void opener::set_filename(QString const& filename) {
  _filename = filename;
  return ;
}

/**
 *  Set the maximum allowed file size.
 *
 *  @param[in] max Maximum file size in bytes.
 */
void opener::set_max_size(unsigned long long max) {
  _max_size = max;
  return ;
}
