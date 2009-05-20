/*
** status.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/15/09 Matthieu Kermagoret
** Last update 05/20/09 Matthieu Kermagoret
*/

#ifndef STATUS_H_
# define STATUS_H_

# include <sys/types.h>
# include "event.h"

namespace        CentreonBroker
{
  /**
   *  This is the root class of status events : host, program and service
   *  status events.
   */
  class          Status : public Event
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
    enum         TimeT
    {
      STATUS_UPDATE_TIME = 0,
      TIMET_NB
    };
    short   shorts_[SHORT_NB];
    time_t  timets_[TIMET_NB];
    void    InternalCopy(const Status& se) throw ();

   public:
            Status();
            Status(const Status& se);
            ~Status();
    Status& operator=(const Status& se);
    // Getters
    short   GetEventHandlerEnabled() const throw ();
    short   GetFailurePredictionEnabled() const throw ();
    short   GetFlapDetectionEnabled() const throw ();
    short   GetNotificationsEnabled() const throw ();
    short   GetProcessPerformanceData() const throw ();
    time_t  GetStatusUpdateTime() const throw ();
    // Setters
    void    SetEventHandlerEnabled(short ehe) throw ();
    void    SetFailurePredictionEnabled(short fpe) throw ();
    void    SetFlapDetectionEnabled(short fde) throw ();
    void    SetNotificationsEnabled(short ne) throw ();
    void    SetProcessPerformanceData(short ppd) throw ();
    void    SetStatusUpdateTime(time_t sut) throw ();
  };
}

#endif /* !STATUS_H_ */
