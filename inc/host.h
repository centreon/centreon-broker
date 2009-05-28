/*
** host.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/22/09 Matthieu Kermagoret
** Last update 05/28/09 Matthieu Kermagoret
*/

#ifndef HOST_H_
# define HOST_H_

# include <string>
# include <sys/types.h>
# include "host_service.h"
# include "host_status.h"

namespace              CentreonBroker
{
  class                Host : public HostService, public HostStatus
  {
   private:
    enum               Short
    {
      FLAP_DETECTION_ON_DOWN = 0,
      FLAP_DETECTION_ON_UNREACHABLE,
      FLAP_DETECTION_ON_UP,
      HAVE_2D_COORDS,
      NOTIFY_ON_DOWN,
      NOTIFY_ON_UNREACHABLE,
      STALK_ON_DOWN,
      STALK_ON_UNREACHABLE,
      STALK_ON_UP,
      X_2D,
      Y_2D,
      SHORT_NB
    };
    enum               String
    {
      ADDRESS = 0,
      ALIAS,
      STATUSMAP_IMAGE,
      VRML_IMAGE,
      STRING_NB
    };
    short              shorts_[SHORT_NB];
    std::string        strings_[STRING_NB];
    void               InternalCopy(const Host& h);

   public:
                       Host() throw ();
		       Host(const HostStatus& hs);
                       Host(const Host& h);
                       ~Host() throw ();
    Host&              operator=(const Host& h);
    // Getters
    const std::string& GetAddress() const throw ();
    const std::string& GetAlias() const throw ();
    short              GetFlapDetectionOnDown() const throw ();
    short              GetFlapDetectionOnUnreachable() const throw ();
    short              GetFlapDetectionOnUp() const throw ();
    short              GetHave2DCoords() const throw ();
    short              GetNotifyOnDown() const throw ();
    short              GetNotifyOnUnreachable() const throw ();
    short              GetStalkOnDown() const throw ();
    short              GetStalkOnUnreachable() const throw ();
    short              GetStalkOnUp() const throw ();
    const std::string& GetStatusmapImage() const throw ();
    int                GetType() const throw ();
    const std::string& GetVrmlImage() const throw ();
    short              GetX2D() const throw ();
    short              GetY2D() const throw ();
    // Setters
    void               SetAddress(const std::string& a);
    void               SetAlias(const std::string& a);
    void               SetFlapDetectionOnDown(short fdod) throw ();
    void               SetFlapDetectionOnUnreachable(short fdou) throw ();
    void               SetFlapDetectionOnUp(short fdou) throw ();
    void               SetHave2DCoords(short h2dc) throw ();
    void               SetNotifyOnDown(short nod) throw ();
    void               SetNotifyOnUnreachable(short nou) throw ();
    void               SetStalkOnDown(short sod) throw ();
    void               SetStalkOnUnreachable(short sou) throw ();
    void               SetStalkOnUp(short sou) throw ();
    void               SetStatusmapImage(const std::string& si);
    void               SetVrmlImage(const std::string& vi);
    void               SetX2D(short x_2d) throw ();
    void               SetY2D(short y_2d) throw ();
  };
}

#endif /* !HOST_H_ */
