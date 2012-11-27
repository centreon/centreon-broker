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

#ifndef CCB_FILE_OPENER_HH
#  define CCB_FILE_OPENER_HH

#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                        file {
  /**
   *  @class opener opener.hh "com/centreon/broker/file/opener.hh"
   *  @brief Open a file stream.
   *
   *  Open a file stream.
   */
  class                          opener : public io::endpoint {
  public:
                                 opener(bool is_in, bool is_out);
                                 opener(opener const& o);
                                 ~opener();
    opener&                      operator=(opener const& o);
    io::endpoint*                clone() const;
    void                         close();
    misc::shared_ptr<io::stream> open();
    void                         set_filename(QString const& filename);
    void                         set_max_size(unsigned long long max);

   private:
    QString                      _filename;
    bool                         _is_in;
    bool                         _is_out;
    unsigned long long           _max_size;
  };
}

CCB_END()

#endif /* !CCB_FILE_OPENER_HH_ */
