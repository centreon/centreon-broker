/*
** Copyright 2012 Centreon
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

#include "com/centreon/broker/dumper/db_reader.hh"
#include "com/centreon/broker/dumper/db_writer.hh"
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
    _db(other._db),
    _name(other._name),
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
    _db = other._db;
    _name = other._name;
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
    return (new directory_dumper(_name, _path, _tagname, _cache));
  case dump_fifo:
    return (new fifo_dumper(_path, _tagname));
  case db_cfg_reader:
    return (new db_reader(_name, _db));
  case db_cfg_writer:
    return (new db_writer(_db));
  default:
    return (misc::shared_ptr<io::stream>());
  }
}

/**
 *  Set database parameters.
 *
 *  @param[in] db_cfg  Database parameters.
 */
void opener::set_db(database_config const& db_cfg) {
  _db = db_cfg;
  return ;
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
 *  Set the name of this endpoint.
 *
 *  @param[in] name  The name of this endpoint.
 */
void opener::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Set the persistent cache.
 *
 *  @param[in] cache  The persistent cache.
 */
void opener::set_cache(misc::shared_ptr<persistent_cache> cache) {
  _cache = cache;
}
