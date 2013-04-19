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

#ifndef CCB_SETABLE_ENDPOINT_HH
#  define CCB_SETABLE_ENDPOINT_HH

#  include <QList>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "test/processing/feeder/setable_stream.hh"

/**
 *  @class setable_endpoint setable_endpoint.hh
 *  @brief Setable endpoint.
 *
 *  Endpoint that can be set to generate errors or not.
 */
class               setable_endpoint : public com::centreon::broker::io::endpoint {
public:
                    setable_endpoint();
                    setable_endpoint(setable_endpoint const& se);
                    ~setable_endpoint();
  setable_endpoint& operator=(setable_endpoint const& se);
  com::centreon::broker::io::endpoint*
                    clone() const;
  void              close();
  com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::stream>
                    open();
  com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::stream>
                    open(QString const& id);
  unsigned int      opened_streams() const;
  void              set_initial_count(unsigned int cnt);
  void              set_initial_replay_events(bool replay);
  void              set_initial_store_events(bool store);
  void              set_process(bool in, bool out);
  void              set_succeed(bool succeed);
  QList<com::centreon::broker::misc::shared_ptr<setable_stream> >&
                    streams();

private:
  void              _internal_copy(setable_endpoint const& se);

  unsigned int      _initial_count;
  bool              _initial_replay_events;
  bool              _initial_store_events;
  unsigned int      _opened_streams;
  bool              _should_succeed;
  QList<com::centreon::broker::misc::shared_ptr<setable_stream> >
                    _streams;
};

#endif // !CCB_SETABLE_ENDPOINT_HH
