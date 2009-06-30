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

#ifndef SERVICE_H_
# define SERVICE_H_

# include <string>
# include <sys/types.h>
# include "events/host_service.h"
# include "events/service_status.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    class                Service : public HostService, public ServiceStatus
    {
     private:
      enum               Short
      {
	FLAP_DETECTION_ON_CRITICAL = 0,
	FLAP_DETECTION_ON_OK,
	FLAP_DETECTION_ON_UNKNOWN,
	FLAP_DETECTION_ON_WARNING,
	FRESHNESS_CHECKS_ENABLED,
	IS_VOLATILE,
	NOTIFY_ON_CRITICAL,
	NOTIFY_ON_UNKNOWN,
	NOTIFY_ON_WARNING,
	STALK_ON_CRITICAL,
	STALK_ON_OK,
	STALK_ON_UNKNOWN,
	STALK_ON_WARNING,
	SHORT_NB
      };
      enum               String
      {
	FAILURE_PREDICTION_OPTIONS = 0,
	STRING_NB
      };
      short              shorts_[SHORT_NB];
      std::string        strings_[STRING_NB];
      void               InternalCopy(const Service& s);

     public:
                         Service() throw ();
			 Service(const ServiceStatus& ss);
			 Service(const Service& s);
			 ~Service() throw ();
      Service&           operator=(const Service& s);
      // Getters
      const std::string& GetFailurePredictionOptions() const throw ();
      short              GetFlapDetectionOnCritical() const throw ();
      short              GetFlapDetectionOnOk() const throw ();
      short              GetFlapDetectionOnUnknown() const throw ();
      short              GetFlapDetectionOnWarning() const throw ();
      short              GetFreshnessChecksEnabled() const throw ();
      short              GetIsVolatile() const throw ();
      short              GetNotifyOnCritical() const throw ();
      short              GetNotifyOnUnknown() const throw ();
      short              GetNotifyOnWarning() const throw ();
      short              GetStalkOnCritical() const throw ();
      short              GetStalkOnOk() const throw ();
      short              GetStalkOnUnknown() const throw ();
      short              GetStalkOnWarning() const throw ();
      int                GetType() const throw ();
      // Setters
      void               SetFailurePredictionOptions(const std::string& fpo);
      void               SetFlapDetectionOnCritical(short fdoc) throw ();
      void               SetFlapDetectionOnOk(short fdoo) throw ();
      void               SetFlapDetectionOnUnknown(short fdou) throw ();
      void               SetFlapDetectionOnWarning(short fdow) throw ();
      void               SetFreshnessChecksEnabled(short fce) throw ();
      void               SetIsVolatile(short iv) throw ();
      void               SetNotifyOnCritical(short noc) throw ();
      void               SetNotifyOnUnknown(short nou) throw ();
      void               SetNotifyOnWarning(short now) throw ();
      void               SetStalkOnCritical(short soc) throw ();
      void               SetStalkOnOk(short soo) throw ();
      void               SetStalkOnUnknown(short sou) throw ();
      void               SetStalkOnWarning(short sow) throw ();
    };
  }
}

#endif /* !SERVICE_H_ */
