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

#include "com/centreon/broker/notification/objects/acknowledgement.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
acknowledgement::acknowledgement() :
  _type(unknown),
  _acknowledgement_type(0),
  _host_id(0),
  _service_id(0) {}

/**
 *  Default copy constructor.
 *
 * @param[in] obj The object to copy.
 */
acknowledgement::acknowledgement(acknowledgement const& obj) {
  acknowledgement::operator=(obj);
}

/**
 *  Copy operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return Itself.
 */
acknowledgement& acknowledgement::operator=(acknowledgement const& obj) {
  if (this != &obj) {
    _type = obj._type;
    _acknowledgement_type = obj._acknowledgement_type;
    _host_id = obj._host_id;
    _service_id = obj._service_id;
  }
  return (*this);
}

/**
 *  Get the type of the acknowledgement.
 *
 *  @return The type of the acknowkledgement.
 */
acknowledgement::type acknowledgement::get_type() const throw() {
  return (_type);
}

/**
 *  Set the type of the acknowledgement.
 *
 *  @param[in] val The type of the acknowledgement.
 */
void acknowledgement::set_type(type val) throw() {
  _type = val;
}

/**
 *  Get the acknowledgement type registered in the database.
 *
 *  @return The acknowledgement type registered in the database.
 */
int acknowledgement::get_acknowledgement_type() const throw() {
  return (_acknowledgement_type);
}

/**
 *  Set the acknowledgement type registered in the database.
 *
 *  @param[in] val The acknowledgement type registered in the database.
 */
void acknowledgement::set_acknowledgement_type(int val) throw() {
  _acknowledgement_type = val;
}

/**
 *  Get the host id linked to this acknowledgement.
 *
 *  @return The host id linked to this acknowledgement. 0 if no host.
 */
unsigned int acknowledgement::get_host_id() const throw() {
  return (_host_id);
}

/**
 *  Set the host id linked to this acknowledgement.
 *
 *  @param[in] val The host id linked to this acknowledgement.
 */
void acknowledgement::set_host_id(unsigned int val) throw() {
  _host_id = val;
}

/**
 *  Get the service id linked to this acknowledgement.
 *
 *  @return the service id linked to this acknowledgement. 0 if no service.
 */
unsigned int acknowledgement::get_service_id() const throw()  {
  return (_service_id);
}

/**
 *  Set the service id linked to this acknowledgement.
 *
 *  @param[in] val The service id linked to this acknowledgement.
 */
void acknowledgement::set_service_id(unsigned int val) throw() {
  _service_id = val;
}
