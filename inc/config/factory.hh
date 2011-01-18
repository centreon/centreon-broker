/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef CONFIG_FACTORY_HH_
# define CONFIG_FACTORY_HH_

// Forward declarations.
namespace                            config {
  class                              interface;
  class                              logger; }
namespace                            interface {
  class                              destination;
  class                              source;
  class                              source_destination; }
namespace                            io {
  class                              acceptor; }
namespace                            logging {
  class                              backend; }

namespace                            config {
  namespace                          factory {
    io::acceptor*                    build_acceptor(
                                       config::interface const& conf);
    ::interface::destination*        build_destination(
                                       config::interface const& conf);
    logging::backend*                build_logger(
                                       config::logger const& conf);
    ::interface::source*             build_source(
                                       config::interface const& conf);
    ::interface::source_destination* build_sourcedestination(
                                       config::interface const& conf);
  }
}

#endif /* !CONFIG_FACTORY_HH_ */
