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
**
** For more information: contact@centreon.com
*/

#ifndef MULTIPLEXING_INTERNAL_HH_
# define MULTIPLEXING_INTERNAL_HH_

# include <list>
# include "concurrency/mutex.hh"

namespace                       multiplexing {
  // Forward declaration.
  class                         subscriber;

  // Internal multiplexing variables.
  extern std::list<subscriber*> gl_subscribers;
  extern concurrency::mutex     gl_subscribersm;
}

#endif /* !MULTIPLEXING_INTERNAL_HH_ */
