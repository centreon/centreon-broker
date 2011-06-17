/*
** Copyright 2011 Merethis
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

#ifndef CCB_CONFIG_PARSER_HH_
# define CCB_CONFIG_PARSER_HH_

# include <QDomElement>
# include <QList>
# include <QString>
# include "com/centreon/broker/config/state.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   config {
        /**
         *  @class parser parser.hh "com/centreon/broker/config/parser.hh"
         *  @brief Parse configuration file.
         *
         *  Parse a configuration file and generate appropriate objects for further
         *  handling.
         */
        class     parser {
         private:
          void    _parse_endpoint(QDomElement& elem, endpoint& e);
          void    _parse_logger(QDomElement& elem, logger& l);

         public:
                  parser();
                  parser(parser const& p);
                  ~parser();
          parser& operator=(parser const& p);
          void    parse(QString const& file, state& s);
        };
      }
    }
  }
}

#endif /* !CCB_CONFIG_PARSER_HH_ */
