/*
** Copyright 2011 Merethis
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

#include "storage/exceptions/perfdata.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage::exceptions;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
perfdata::perfdata() throw () {}

/**
 *  Copy constructor.
 *
 *  @param[in] pd Object to copy.
 */
perfdata::perfdata(perfdata const& pd) throw ()
  : com::centreon::broker::exceptions::basic(pd) {}

/**
 *  Destructor.
 */
perfdata::~perfdata() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] pd Object to copy.
 *
 *  @return This object.
 */
perfdata& perfdata::operator=(perfdata const& pd) throw () {
  com::centreon::broker::exceptions::basic::operator=(pd);
  return (*this);
}
