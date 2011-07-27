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

#ifndef CCB_STORAGE_PARSER_HH_
# define CCB_STORAGE_PARSER_HH_

# include <QList>
# include <QString>
# include "com/centreon/broker/storage/perfdata.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   storage {
        /**
         *  @class parser parser.hh "com/centreon/broker/storage/parser.hh"
         *  @brief Parse raw perfdatas.
         *
         *  Parse perfdata as given by plugins output and convert them to a
         *  perfdata structure.
         */
        class     parser {
         public:
                  parser();
                  parser(parser const& p);
                  ~parser();
          parser& operator=(parser const& p);
          void    parse_perfdata(QString const& str,
                    QList<perfdata>& pd);
        };
      }
    }
  }
}

#endif /* !CCB_STORAGE_PARSER_HH_ */
