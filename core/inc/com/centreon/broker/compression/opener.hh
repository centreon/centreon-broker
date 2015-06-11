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

#ifndef CCB_COMPRESSION_OPENER_HH
#  define CCB_COMPRESSION_OPENER_HH

#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                        compression {
  /**
   *  @class opener opener.hh "com/centreon/broker/compression/opener.hh"
   *  @brief Open a compression stream.
   *
   *  Open a compression stream.
   */
  class                          opener : public io::endpoint {
  public:
                                 opener();
                                 opener(opener const& o);
                                 ~opener();
    opener&                      operator=(opener const& o);
    void                         close();
    misc::shared_ptr<io::stream> open();
    void                         set_level(int level = -1);
    void                         set_size(unsigned int size = 0);

  private:
    misc::shared_ptr<io::stream> _open(
                                   misc::shared_ptr<io::stream> stream);

    int                          _level;
    unsigned int                 _size;
  };
}

CCB_END()

#endif // !CCB_COMPRESSION_OPENER_HH
