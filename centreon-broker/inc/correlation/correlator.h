/*
**  Copyright 2009 MERETHIS
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

#ifndef CORRELATION_CORRELATOR_H_
# define CORRELATION_CORRELATOR_H_

# include <list>
# include <map>
# include "correlation/node.h"
# include "events/event.h"
# include "events/host.h"

namespace                    Correlation
{
  /**
   *  \class Correlator correlator.h "correlation/correlator.h"
   *  \brief Create event correlation.
   *
   *  This class aggregate data from multiple Nagios events and generates
   *  according issues which are used to group those events.
   */
  class                      Correlator
  {
   private:
    std::map<int, Node>      hosts_;
    std::list<Events::Issue> issues_;
    std::map<int, Node>      services_;
    void                     CorrelateHost(Events::Host& host);
    void                     InternalCopy(const Correlator& correlator);

   public:
                             Correlator();
                             Correlator(const Correlator& correlator);
                             ~Correlator();
    Correlator&              operator=(const Correlator& correlator);
    void                     Event(Events::Event& event);
  };
}

#endif /* !CORRELATION_CORRELATOR_H_ */
