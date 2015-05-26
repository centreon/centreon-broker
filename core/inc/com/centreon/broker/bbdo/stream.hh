/*
** Copyright 2013,2015 Merethis
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

#ifndef CCB_BBDO_STREAM_HH
#  define CCB_BBDO_STREAM_HH

#  include <QString>
#  include "com/centreon/broker/bbdo/input.hh"
#  include "com/centreon/broker/bbdo/output.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace            bbdo {
  /**
   *  @class stream stream.hh "com/centreon/broker/bbdo/stream.hh"
   *  @brief BBDO stream.
   *
   *  The class converts data to NEB events back and forth.
   */
  class              stream : public input, public output {
  public:
    enum             negociation_type {
      negociate_first = 1,
      negociate_second,
      negociated
    };

                     stream(bool is_in, bool is_out);
                     stream(stream const& other);
                     ~stream();
    stream&          operator=(stream const& other);
    void             negociate(negociation_type neg);
    void             process(bool in = false, bool out = true);
    void             read(misc::shared_ptr<io::data>& d);
    void             set_coarse(bool coarse);
    void             set_negociate(
                       bool negociate,
                       QString const& extensions = QString());
    void             set_timeout(int timeout);
    void             statistics(io::properties& tree) const;
    unsigned int     write(misc::shared_ptr<io::data> const& d);

  private:
    bool             _coarse;
    QString          _extensions;
    bool             _input_read;
    bool             _negociate;
    bool             _negociated;
    bool             _output_write;
    int              _timeout;
  };
}

CCB_END()

#endif // !CCB_BBDO_STREAM_HH
