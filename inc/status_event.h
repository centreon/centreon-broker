/*
** status_event.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/15/09 Matthieu Kermagoret
** Last update 05/15/09 Matthieu Kermagoret
*/

#ifndef STATUS_EVENT_H_
# define STATUS_EVENT_H_

# include <sys/types.h>
# include "event.h"

namespace        CentreonBroker
{
  class          StatusEvent : public Event
  {
   private:
    enum         Short
    {
      EVENT_HANDLER_ENABLED = 0,
      FAILURE_PREDICTION_ENABLED,
      FLAP_DETECTION_ENABLED,
      NOTIFICATIONS_ENABLED,
      PROCESS_PERFORMANCE_DATA,
      SHORT_NB
    };
    short        shorts_[SHORT_NB];
    time_t       status_update_time_;
    void         InternalCopy(const StatusEvent& se) throw ();

   public:
                 StatusEvent();
                 StatusEvent(const StatusEvent& se);
                 ~StatusEvent();
    StatusEvent& operator=(const StatusEvent& se);
    // Getters
    short        GetEventHandlerEnabled() const throw ();
    short        GetFailurePredictionEnabled() const throw ();
    short        GetFlapDetectionEnabled() const throw ();
    short        GetNotificationsEnabled() const throw ();
    short        GetProcessPerformanceData() const throw ();
    time_t       GetStatusUpdateTime() const throw ();
    // Setters
    void         SetEventHandlerEnabled(short ehe) throw ();
    void         SetFailurePredictionEnabled(short fpe) throw ();
    void         SetFlapDetectionEnabled(short fde) throw ();
    void         SetNotificationsEnabled(short ne) throw ();
    void         SetProcessPerformanceData(short ppd) throw ();
    void         SetStatusUpdateTime(time_t sut) throw ();
  };
}

#endif /* !STATUS_EVENT_H_ */
