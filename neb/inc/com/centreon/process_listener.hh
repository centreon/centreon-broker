/*
** Copyright 2012-2013 Merethis
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

#ifndef CC_PROCESS_LISTENER_HH
#  define CC_PROCESS_LISTENER_HH

#  include "com/centreon/namespace.hh"
#  include "com/centreon/process.hh"

CC_BEGIN()

/**
 *  @class process process_listener.hh "com/centreon/process_listener.hh"
 *  @brief Notify process events.
 *
 *  This class provide interface to notify process events.
 */
class          process_listener {
public:
  virtual      ~process_listener() throw () {}
  virtual void data_is_available(process& p) throw () = 0;
  virtual void data_is_available_err(process& p) throw () = 0;
  virtual void finished(process& p) throw () = 0;
};

CC_END()

#endif // !CC_PROCESS_LISTENER_HH
