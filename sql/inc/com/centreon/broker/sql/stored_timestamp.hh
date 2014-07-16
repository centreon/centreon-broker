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

#ifndef CCB_SQL_STORED_TIMESTAMP_HH
#  define CCB_SQL_STORED_TIMESTAMP_HH

#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace sql {
  /**
   *  @class stored_timestamp stored_timestamp.hh "com/centreon/broker/sql/stored_timestamp.hh"
   *  @brief Store a timestamp associated to an instance.
   *
   *  The instance can be in a responsive or unresponsive state, and its id is stored.
   */
  class           stored_timestamp {
  public:
    enum          state_type {
                  responsive,
                  unresponsive
    };
                  stored_timestamp() throw();
                  stored_timestamp(unsigned int id, state_type s) throw();
                  stored_timestamp(stored_timestamp const& right) throw();
                  ~stored_timestamp() throw();

    unsigned int  get_id() const throw();
    state_type    get_state() const throw();
    void          set_state(state_type state) throw();
    void          update_timestamp() throw();
    timestamp     get_timestamp() const throw();
    void          set_timestamp(timestamp ts) throw();
    bool          timestamp_outdated(unsigned int timeout) const throw();

  private:

    unsigned int  _id;
    timestamp     _ts;
    state_type    _state;
  };
}

CCB_END()

#endif //!CCB_SQL_STORED_TIMESTAMP_HH
