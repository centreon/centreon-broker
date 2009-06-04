/*
** service.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/22/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#ifndef SERVICE_H_
# define SERVICE_H_

# include <string>
# include <sys/types.h>
# include "host_service.h"
# include "service_status.h"

namespace              CentreonBroker
{
  class                Service : public HostService, public ServiceStatus
  {
   private:
    enum               Short
    {
      DEFAULT_ACTIVE_CHECKS_ENABLED = 0,
      DEFAULT_EVENT_HANDLER_ENABLED,
      DEFAULT_FAILURE_PREDICTION_ENABLED,
      DEFAULT_FLAP_DETECTION_ENABLED,
      DEFAULT_NOTIFICATIONS_ENABLED,
      DEFAULT_PASSIVE_CHECKS_ENABLED,
      DEFAULT_PROCESS_PERFORMANCE_DATA,
      FLAP_DETECTION_ON_CRITICAL,
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
    short              GetDefaultActiveChecksEnabled() const throw ();
    short              GetDefaultEventHandlerEnabled() const throw ();
    short              GetDefaultFailurePredictionEnabled() const throw ();
    short              GetDefaultFlapDetectionEnabled() const throw ();
    short              GetDefaultNotificationsEnabled() const throw ();
    short              GetDefaultPassiveChecksEnabled() const throw ();
    short              GetDefaultProcessPerformanceData() const throw ();
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
    short              GetStalkOnUnknown() const throw ();
    short              GetStalkOnWarning() const throw ();
    int                GetType() const throw ();
    // Setters
    void               SetDefaultActiveChecksEnabled(short dace) throw ();
    void               SetDefaultEventHandlerEnabled(short dehe) throw ();
    void               SetDefaultFailurePredictionEnabled(short dfpe) throw ();
    void               SetDefaultFlapDetectionEnabled(short dfde) throw ();
    void               SetDefaultNotificationsEnabled(short dne) throw ();
    void               SetDefaultPassiveChecksEnabled(short dpce) throw ();
    void               SetDefaultProcessPerformanceData(short dppd) throw ();
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
    void               SetStalkOnUnknown(short sou) throw ();
    void               SetStalkOnWarning(short sow) throw ();
  };
}

#endif /* !SERVICE_H_ */
