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

#include "events/custom_variable.hh"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] cv Object to copy.
 */
void custom_variable::_internal_copy(custom_variable const& cv) {
  default_value = cv.default_value;
  host_id       = cv.host_id;
  modified      = cv.modified;
  name          = cv.name;
  service_id    = cv.service_id;
  type          = cv.type;
  update_time   = cv.update_time;
  value         = cv.value;
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
custom_variable::custom_variable()
  : host_id(0),
    modified(false),
    service_id(0),
    type(0),
    update_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] cv Object to copy.
 */
custom_variable::custom_variable(custom_variable const& cv)
  : Event(cv) {
  _internal_copy(cv);
}

/**
 *  Destructor.
 */
custom_variable::~custom_variable() {}

/**
 *  Assignment operator.
 *
 *  @param[in] cv Object to copy.
 *
 *  @return This object.
 */
custom_variable& custom_variable::operator=(custom_variable const& cv) {
  Event::operator=(cv);
  _internal_copy(cv);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return CUSTOMVARIABLE.
 */
int custom_variable::GetType() {
  return (CUSTOMVARIABLE);
}
