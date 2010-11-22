/*
**  Copyright 2010 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include "events/custom_variable_status.hh"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] cvs Object to copy.
 */
void custom_variable_status::_internal_copy(custom_variable_status const& cvs) {
  host_id = cvs.host_id;
  modified = cvs.modified;
  name = cvs.name;
  service_id = cvs.service_id;
  update_time = cvs.update_time;
  value = cvs.value;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
custom_variable_status::custom_variable_status()
  : host_id(0),
    modified(true),
    service_id(0),
    update_time(time(NULL)) {}

/**
 *  Copy constructor.
 *
 *  @param[in] cvs Object to copy.
 */
custom_variable_status::custom_variable_status(custom_variable_status const& cvs)
  : Event(cvs) {
  _internal_copy(cvs);
}

/**
 *  Destructor.
 */
custom_variable_status::~custom_variable_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] cvs Object to copy.
 *
 *  @return This object.
 */
custom_variable_status& custom_variable_status::operator=(custom_variable_status const& cvs) {
  Event::operator=(cvs);
  _internal_copy(cvs);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return CUSTOMVARIABLESTATUS.
 */
int custom_variable_status::GetType() const {
  return (CUSTOMVARIABLESTATUS);
}
