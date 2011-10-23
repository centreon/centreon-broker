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

#ifndef CCB_NDO_STREAM_HH_
# define CCB_NDO_STREAM_HH_

# include "com/centreon/broker/namespace.hh"
# include "com/centreon/broker/ndo/input.hh"
# include "com/centreon/broker/ndo/output.hh"

CCB_BEGIN()

namespace                    ndo {
  /**
   *  @class stream stream.hh "com/centreon/broker/ndo/stream.hh"
   *  @brief NDO stream.
   *
   *  The class converts data to neb events back and forth.
   */
  class                      stream : public input,
                                      public output {
   public:
                             stream();
                             stream(stream const& s);
                             ~stream();
    stream&                  operator=(stream const& s);
    void                     process(bool in = false, bool out = true);
    QSharedPointer<io::data> read();
    void                     write(QSharedPointer<io::data> d);
  };
}

CCB_END()

#endif /* !CCB_NDO_STREAM_HH_ */
