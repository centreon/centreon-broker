/*
** Copyright 2011 Merethis
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

#ifndef CCE_SETABLE_ENDPOINT_HH_
# define CCE_SETABLE_ENDPOINT_HH_

# include <QList>
# include <QSharedPointer>
# include "com/centreon/broker/io/endpoint.hh"
# include "test/processing/feeder/setable_stream.hh"

/**
 *  @class setable_endpoint setable_endpoint.hh
 *  @brief Setable endpoint.
 *
 *  Endpoint that can be set to generate errors or not.
 */
class               setable_endpoint : public com::centreon::broker::io::endpoint {
 private:
  unsigned int      _initial_count;
  bool              _initial_store_events;
  unsigned int      _opened_streams;
  bool              _save_streams;
  QSharedPointer<volatile bool>
                    _should_succeed;
  QList<QSharedPointer<setable_stream> >
                    _streams;
  void              _internal_copy(setable_endpoint const& se);

 public:
                    setable_endpoint();
                    setable_endpoint(setable_endpoint const& se);
                    ~setable_endpoint();
  setable_endpoint& operator=(setable_endpoint const& se);
  void              close();
  QSharedPointer<com::centreon::broker::io::stream>
                    open();
  unsigned int      opened_streams() const;
  void              save_streams(bool save);
  void              set(bool should_succeed);
  void              set_initial_count(unsigned int cnt);
  void              set_initial_store_events(bool store);
  QList<QSharedPointer<setable_stream> >&
                    streams();
};

#endif /* !CCE_SETABLE_ENDPOINT_HH_ */
