/*
** Copyright 2009-2011 MERETHIS
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

#include "events/custom_variable.hh"

using namespace com::centreon::broker::events;

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
  type = cv.type;
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
custom_variable::custom_variable() : type(0) {
  modified = false;
}

/**
 *  Copy constructor.
 *
 *  @param[in] cv Object to copy.
 */
custom_variable::custom_variable(custom_variable const& cv)
  : custom_variable_status(cv) {
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
  event::operator=(cv);
  _internal_copy(cv);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return CUSTOMVARIABLE.
 */
int custom_variable::get_type() const {
  return (CUSTOMVARIABLE);
}
