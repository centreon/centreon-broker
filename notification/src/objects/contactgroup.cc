/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/notification/objects/contactgroup.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
contactgroup::contactgroup() {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
contactgroup::contactgroup(contactgroup const& obj) {
  contactgroup::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return  A reference to this object.
 */
contactgroup& contactgroup::operator=(contactgroup const& obj) {
  if (this != &obj) {
    _alias = obj._alias;
  }
  return (*this);
}

/**
 *  Get the name of this contactgroup.
 *
 *  @return  The name of this contactgroup.
 */
std::string const& contactgroup::get_name() const {
  return (_name);
}

/**
 *  Set the name of this contactgroup.
 *
 *  @param[in] name  The name of this contactgroup.
 */
void contactgroup::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Get the alias of this contactgroup.
 *
 *  @return  The alias of this contactgroup.
 */
std::string const& contactgroup::get_alias() const {
  return (_alias);
}

/**
 *  Set the alias of this contactgroup.
 *
 *  @param[in] alias  The alias of this contactgroup.
 */
void contactgroup::set_alias(std::string const& alias) {
  _alias = alias;
}
