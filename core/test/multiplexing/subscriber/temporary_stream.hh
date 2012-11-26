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

#ifndef CCB_TEMPORARY_STREAM_HH
#  define CCB_TEMPORARY_STREAM_HH

#  include <QQueue>
#  include <QMutex>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

/**
 *  @class temporary_stream temporary_stream.hh
 *  @brief Temporary stream.
 */
class               temporary_stream : public io::stream {
public:
                    temporary_stream();
                    temporary_stream(temporary_stream const& ss);
                    ~temporary_stream();
  temporary_stream& operator=(temporary_stream const& ss);
  void              process(bool in = false, bool out = true);
  void              read(misc::shared_ptr<io::data>& d);
  void              write(misc::shared_ptr<io::data> const& d);

private:
  QQueue<misc::shared_ptr<io::data> >
                    _events;
  mutable QMutex    _eventsm;
};

CCB_END()

#endif // !CCB_TEMPORARY_STREAM_HH
