/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_HANDLE_LISTENER_HH
#  define CC_HANDLE_LISTENER_HH

#  include "com/centreon/namespace.hh"
#  include "com/centreon/handle.hh"

CC_BEGIN()

/**
 *  @class handle_listener handle_listener.hh "com/centreon/handle_listener.hh"
 *  @brief Base for all handle_listener objects.
 *
 *  This class is an interface to receive notification from
 *  handle_manager for specific handle.
 */
class          handle_listener {
public:
               handle_listener();
  virtual      ~handle_listener() throw ();
  virtual void error(handle& h) = 0;
  virtual void read(handle& h);
  virtual bool want_read(handle& h);
  virtual bool want_write(handle& h);
  virtual void write(handle& h);
};

CC_END()

#endif // !CC_HANDLE_LISTENER_HH
