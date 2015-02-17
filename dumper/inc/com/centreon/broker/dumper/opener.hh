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

#ifndef CCB_DUMPER_OPENER_HH
#  define CCB_DUMPER_OPENER_HH

#  include <string>
#  include "com/centreon/broker/io/endpoint.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                        dumper {
  /**
   *  @class opener opener.hh "com/centreon/broker/dumper/opener.hh"
   *  @brief Open a dumper stream.
   *
   *  Open a dumper stream.
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
    misc::shared_ptr<io::stream> open(QString const& id);
    void                         set_path(std::string const& path);
    void                         set_tagname(std::string const& tagname);

   private:
    std::string                  _path;
    bool                         _is_in;
    bool                         _is_out;
    std::string                  _tagname;
  };
}

CCB_END()

#endif // !CCB_DUMPER_OPENER_HH
