/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/notification/objects/contact.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
contact::contact() : _id(0) {}

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
uint32_t contact::get_id() const throw() {
  return (_id);
}

/**
 *  Set the id of this contact.
 *
 *  @param[in] id  The id of this contact.
 */
void contact::set_id(uint32_t id) {
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
