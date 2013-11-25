/*
** Copyright 2013 Merethis
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

#ifndef CCB_STATS_GENERATOR_HH
#  define CCB_STATS_GENERATOR_HH

#  include <QThread>
#  include <map>
#  include <utility>

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         stats {
        // Forward declaration.
        class           config;

        /**
         *  @class generator generator.hh "com/centreon/broker/stats/generator.hh"
         *  @brief Generate statistics about Broker.
         *
         *  Standalone thread that generates statistics about Broker at
         *  regular interval.
         */
        class           generator : public QThread {
          Q_OBJECT

        public:
                        generator();
                        ~generator() throw ();
          void          exit();
          void          run(
                          config const& cfg,
                          unsigned int instance_id);

        private:
                        generator(generator const& right);
          generator&    operator=(generator const& right);
          void          run();

          unsigned int  _instance_id;
          int           _interval;
          std::map<
                 std::string,
                 std::pair<unsigned int, unsigned int> >
                        _plugins;
          volatile bool _should_exit;
          std::string   _tag;
        };
      }
    }
  }
}

#endif // !CCB_STATS_GENERATOR_HH
