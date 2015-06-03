/*
** Copyright 2011,2015 Merethis
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

#ifndef CCB_MULTIPLEXING_HOOKER_HH
#  define CCB_MULTIPLEXING_HOOKER_HH

#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        multiplexing {
  /**
   *  @class hooker hooker.hh "com/centreon/broker/multiplexing/hooker.hh"
   *  @brief Hook object.
   *
   *  Place a hook on the multiplexing engine.
   */
  class          hooker : public io::stream {
  public:
                 hooker();
                 hooker(hooker const& other);
    virtual      ~hooker();
    hooker&      operator=(hooker const& other);
    void         hook(bool should_hook);
    virtual void starting() = 0;
    virtual void stopping() = 0;

  protected:
    bool         _registered;
  };
}

CCB_END()

#endif // !CCB_MULTIPLEXING_HOOKER_HH
