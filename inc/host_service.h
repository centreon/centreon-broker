/*
** host_service.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/22/09 Matthieu Kermagoret
** Last update 05/22/09 Matthieu Kermagoret
*/

#ifndef HOST_SERVICE_H_
# define HOST_SERVICE_H_

# include <string>

namespace              CentreonBroker
{
  class                HostService
  {
   private:
    enum               Double
    {
      FIRST_NOTIFICATION_DELAY = 0,
      HIGH_FLAP_THRESHOLD,
      LOW_FLAP_THRESHOLD,
      NOTIFICATION_INTERVAL,
      DOUBLE_NB
    };
    enum               Short
    {
      FRESHNESS_THRESHOLD = 0,
      NOTIFY_ON_DOWNTIME,
      NOTIFY_ON_FLAPPING,
      NOTIFY_ON_RECOVERY,
      RETAIN_NONSTATUS_INFORMATION,
      RETAIN_STATUS_INFORMATION,
      SHORT_NB
    };
    enum               String
    {
      ACTION_URL,
      DISPLAY_NAME,
      ICON_IMAGE,
      ICON_IMAGE_ALT,
      NOTES,
      NOTES_URL,
      STRING_NB
    };
    double             doubles_[DOUBLE_NB];
    short              shorts_[SHORT_NB];
    std::string        strings_[STRING_NB];
    void               InternalCopy(const HostService& hs);

   public:
                       HostService() throw ();
                       HostService(const HostService& hs);
                       ~HostService() throw ();
    HostService&       operator=(const HostService& hs);
    // Getters
    const std::string& GetActionUrl() const throw ();
    const std::string& GetDisplayName() const throw ();
    double             GetFirstNotificationDelay() const throw ();
    short              GetFreshnessThreshold() const throw ();
    double             GetHighFlapThreshold() const throw ();
    const std::string& GetIconImage() const throw ();
    const std::string& GetIconImageAlt() const throw ();
    double             GetLowFlapThreshold() const throw ();
    const std::string& GetNotes() const throw ();
    const std::string& GetNotesUrl() const throw ();
    double             GetNotificationInterval() const throw ();
    short              GetNotifyOnDowntime() const throw ();
    short              GetNotifyOnFlapping() const throw ();
    short              GetNotifyOnRecovery() const throw ();
    short              GetRetainNonstatusInformation() const throw ();
    short              GetRetainStatusInformation() const throw ();
    // Setters
    void               SetActionUrl(const std::string& au);
    void               SetDisplayName(const std::string& dn);
    void               SetFirstNotificationDelay(double fnd) throw ();
    void               SetFreshnessThreshold(short ft) throw ();
    void               SetHighFlapThreshold(double hft) throw ();
    void               SetIconImage(const std::string& ii);
    void               SetIconImageAlt(const std::string& iia);
    void               SetLowFlapThreshold(double lft) throw ();
    void               SetNotes(const std::string& n);
    void               SetNotesUrl(const std::string& nu);
    void               SetNotificationInterval(double ni) throw ();
    void               SetNotifyOnDowntime(short nod) throw ();
    void               SetNotifyOnFlapping(short nof) throw ();
    void               SetNotifyOnRecovery(short nor) throw ();
    void               SetRetainNonstatusInformation(short rni) throw ();
    void               SetRetainStatusInformation(short rsi) throw ();
  };
}

#endif /* !HOST_SERVICE_H_ */
