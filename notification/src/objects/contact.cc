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

#include "com/centreon/broker/notification/objects/contact.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
contact::contact() :
  _id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
contact::contact(contact const& obj) {
  _id = obj._id;
  _description = obj._description;
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to be copied.
 *
 *  @return  A reference to this object.
 */
contact& contact::operator=(contact const& obj) {
  if (this != &obj) {
    _id = obj._id;
    _description = obj._description;
  }
  return (*this);
}

/**
 *  Get the id of this contact.
 *
 *  @return  The id of this contact.
 */
unsigned int contact::get_id() const throw() {
  return (_id);
}

/**
 *  Set the id of this contact.
 *
 *  @param[in] id  The id of this contact.
 */
void contact::set_id(unsigned int id) {
  _id = id;
}

/**
 *  Get the description of this contact.
 *
 *  @return   The description of this contact.
 */
std::string const& contact::get_description() const throw() {
  return (_description);
}

/**
 *  Set the description of this contact.
 *
 *  @param[in] desc  The description of this contact.
 */
void contact::set_description(std::string const& desc) {
  _description = desc;
}
