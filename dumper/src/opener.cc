/*
** Copyright 2012,2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/dumper/opener.hh"
#include "com/centreon/broker/dumper/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

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
    _is_out(is_out) {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
opener::opener(opener const& o)
  : io::endpoint(o),
    _path(o._path),
    _is_in(o._is_in),
    _is_out(o._is_out),
    _tagname(o._tagname) {}

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
  _path = o._path;
  _is_in = o._is_in;
  _is_out = o._is_out;
  _tagname = o._tagname;
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
  return (misc::shared_ptr<io::stream>(
            new stream(_path, _tagname)));
}

/**
 *  Open a new stream.
 *
 *  @param[in] id     The identifier.
 *
 *  @return Opened stream.
 */
misc::shared_ptr<io::stream> opener::open(QString const& id) {
  return (misc::shared_ptr<io::stream>(
            new stream(_path + "-" + qPrintable(id), _tagname)));
}

/**
 *  Set the path.
 *
 *  @param[in] path Path.
 */
void opener::set_path(std::string const& path) {
  _path = path;
  return ;
}

/**
 *  Set the tagname.
 *
 *  @param[in] tagname Tagname.
 */
void opener::set_tagname(std::string const& tagname) {
  _tagname = tagname;
  return ;
}
