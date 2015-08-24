/*
** Copyright 2009-2013 Merethis
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/instance_configuration.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
instance_configuration::instance_configuration()
  : loaded(false) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] i Object to copy.
 */
instance_configuration::instance_configuration(
  instance_configuration const& i) : io::data(i) {
  _internal_copy(i);
}

/**
 *  Destructor.
 */
instance_configuration::~instance_configuration() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] i Object to copy.
 */
instance_configuration& instance_configuration::operator=(
  instance_configuration const& i) {
  io::data::operator=(i);
  _internal_copy(i);
  return (*this);
}

/**
 *  Get the type of the event.
 *
 *  @return The event_type.
 */
unsigned int instance_configuration::type() const {
  return (io::events::data_type<io::events::neb, neb::de_instance_configuration>::value);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the instance object to the current
 *         instance.
 *
 *  Copy data defined within the instance class. This method is used by
 *  the copy constructor and the assignment operator.
 *
 *  @param[in] i Object to copy.
 */
void instance_configuration::_internal_copy(instance_configuration const& i) {
  loaded = i.loaded;
  return ;
}
