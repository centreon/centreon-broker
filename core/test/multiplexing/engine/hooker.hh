/*
** Copyright 2011-2012 Merethis
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

#ifndef HOOKER_HH
#  define HOOKER_HH

#  include <QQueue>
#  include "com/centreon/broker/multiplexing/hooker.hh"

#  define HOOKMSG1 "my first hooking message (when engine is started)"
#  define HOOKMSG2 "my second hooking message (when multiplexing events)"
#  define HOOKMSG3 "my third hooking message (when engine is stopped)"

using namespace com::centreon::broker;

/**
 *  @class hooker hooker.hh "test/multiplexing/engine/hooker.hh"
 *  @brief Test hook class.
 *
 *  Simple class that hook events from the multiplexing engine.
 */
class     hooker : public multiplexing::hooker {
public:
          hooker();
          hooker(hooker const& h);
          ~hooker();
  hooker& operator=(hooker const& h);
  misc::shared_ptr<io::data>
          read();
  void    starting();
  void    stopping();
  void    write(misc::shared_ptr<io::data> d);

private:
  QQueue<misc::shared_ptr<io::data> >
          _queue;
};

#endif // !HOOKER_HH
