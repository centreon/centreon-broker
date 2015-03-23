/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/dumper/directory_event.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper;

/**
 *  Default constructor.
 */
directory_event::directory_event() {

}

/**
 *  Constructor.
 *
 *  @param[in] path  The path of this directory event.
 *  @param[in] type  The type of this directory event.
 */
directory_event::directory_event(std::string const& path, type type)
  : _path(path),
    _type(type) {

}

/**
 *  Copy constructor.
 *
 *  @param[in] o  The object to copy.
 */
directory_event::directory_event(directory_event const& o)
  : _path(o._path) {
}

/**
 *  Assignment operator.
 *
 *  @param[in] o  The object to copy.
 *
 *  @return       A reference to this object.
 */
directory_event& directory_event::operator=(directory_event const& o) {
  if (this != &o) {
    _path = o._path;
  }
  return (*this);
}

/**
 *  Destructor.
 */
directory_event::~directory_event() {

}

/**
 *  Get the path of this event.
 *
 *  @return  The path of this event.
 */
std::string const& directory_event::get_path() const {
  return (_path);
}

/**
 *  Get the type of this event.
 *
 *  @return  The type of this event.
 */
directory_event::type directory_event::get_type() const {
  return (_type);
}
