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

#ifndef HOST_H_
# define HOST_H_

# include <string>
# include <sys/types.h>
# include "host_service.h"
# include "host_status.h"

namespace                CentreonBroker
{
  namespace              Events
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
}

#endif /* !HOST_H_ */
