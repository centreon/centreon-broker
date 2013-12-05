/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_CORRELATION_ENGINE_STATE_HH
#  define CCB_CORRELATION_ENGINE_STATE_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace          correlation {
  /**
   *  @class engine_state engine_state.hh "com/centreon/broker/correlation/engine_state.hh"
   *  @brief State of the correlation engine.
   *
   *  This class represent the state of the correlation engine.
   */
  class            engine_state : public io::data {
  public:
                   engine_state();
                   engine_state(engine_state const& es);
    virtual        ~engine_state();
    engine_state&  operator=(engine_state const& es);
    unsigned int   type() const;

    unsigned int   instance_id;
    bool           started;
  };
}

CCB_END()

#endif // !CCB_CORRELATION_STATE_HH
