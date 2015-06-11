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
#include "com/centreon/broker/dumper/directory_dumper.hh"
#include "com/centreon/broker/dumper/fifo_dumper.hh"

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
opener::opener() : endpoint(false), _type(opener::dump) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
opener::opener(opener const& other)
  : io::endpoint(other),
    _path(other._path),
    _tagname(other._tagname),
    _type(other._type),
    _cache(other._cache) {}

/**
 *  Destructor.
 */
opener::~opener() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
opener& opener::operator=(opener const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _path = other._path;
    _tagname = other._tagname;
    _type = other._type;
    _cache = other._cache;
  }
  return (*this);
}

/**
 *  Open a new stream.
 *
 *  @return Opened stream.
 */
misc::shared_ptr<io::stream> opener::open() {
  switch (_type) {
  case dump:
    return (new stream(_path, _tagname));
  case dump_dir:
    return (new directory_dumper(_path, _tagname, _cache));
  case dump_fifo:
    return (new fifo_dumper(_path, _tagname));
  default:
    return (new stream(_path, _tagname));
  }
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

/**
 *  Set the type of this dumper endpoint.
 *
 *  @param[in] type  The type of this dumper endpoint.
 */
void opener::set_type(dumper_type type) {
  _type = type;
}

/**
 *  Set the cache associated with this dumper endpoint.
 *
 *  @param[in] cache  The cache associated with this dumper endpoint.
 */
void opener::set_cache(misc::shared_ptr<persistent_cache> cache) {
  _cache = cache;
}
