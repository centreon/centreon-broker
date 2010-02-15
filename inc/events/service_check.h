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

#ifndef EVENTS_SERVICE_CHECK_H_
# define EVENTS_SERVICE_CHECK_H_

# include <string>
# include "events/check.h"

namespace         Events
{
  /**
   *  \class ServiceCheck service_check.h "events/service_check.h"
   *  \brief Check that has been executed on a service.
   *
   *  Once a check has been executed on a service, an object of this class is
   *  sent.
   */
  class           ServiceCheck : public Check
  {
   public:
                  ServiceCheck();
                  ServiceCheck(const ServiceCheck& service_check);
    virtual       ~ServiceCheck();
    ServiceCheck& operator=(const ServiceCheck& service_check);
    int           GetType() const;
  };
}

#endif /* !EVENTS_SERVICE_CHECK_H_ */
