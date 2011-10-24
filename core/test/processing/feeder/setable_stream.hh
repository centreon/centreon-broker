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

#ifndef CCE_SETABLE_STREAM_HH_
# define CCE_SETABLE_STREAM_HH_

# include <QList>
# include <QSharedPointer>
# include "com/centreon/broker/io/stream.hh"

/**
 *  @class setable_stream setable_stream.hh
 *  @brief Setable stream.
 *
 *  Stream that can be set to generate errors or not.
 */
class             setable_stream : public com::centreon::broker::io::stream {
 private:
  unsigned int    _count;
  bool            _process_in;
  bool            _process_out;
  bool            _replay_events;
  QList<QSharedPointer<com::centreon::broker::io::data> >
                  _replay;
  unsigned int    _sleep_time;
  bool            _store_events;
  QList<QSharedPointer<com::centreon::broker::io::data> >
                  _stored_events;
  void            _internal_copy(setable_stream const& ss);

 public:
                  setable_stream();
                  setable_stream(setable_stream const& ss);
                  ~setable_stream();
  setable_stream& operator=(setable_stream const& ss);
  unsigned int    get_count() const;
  QList<QSharedPointer<com::centreon::broker::io::data> > const&
                  get_stored_events() const;
  void            process(bool in = false, bool out = true);
  QSharedPointer<com::centreon::broker::io::data>
                  read();
  void            set_count(unsigned int cnt);
  void            set_replay_events(bool replay);
  void            set_sleep_time(unsigned int ms);
  void            set_store_events(bool store);
  void            write(QSharedPointer<com::centreon::broker::io::data> d);
};

#endif /* !CCE_SETABLE_STREAM_HH_ */
