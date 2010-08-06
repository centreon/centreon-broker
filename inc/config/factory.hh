/*
**  Copyright 2010 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef CONFIG_FACTORY_HH_
# define CONFIG_FACTORY_HH_

# include "config/interface.hh"
# include "config/logger.hh"
# include "interface/destination.h"
# include "interface/source.h"
# include "interface/sourcedestination.h"
# include "io/acceptor.h"
# include "logging/backend.hh"

namespace                         config
{
  namespace                       factory
  {
    IO::Acceptor*                 build_acceptor(
                                    config::interface const& conf);
    Interface::Destination*       build_destination(
                                    config::interface const& conf);
    logging::backend*             build_logger(
                                    config::logger const& conf);
    Interface::Source*            build_source(
                                    config::interface const& conf);
    Interface::SourceDestination* build_sourcedestination(
                                    config::interface const& conf);
  }
}

#endif /* !CONFIG_FACTORY_HH_ */
