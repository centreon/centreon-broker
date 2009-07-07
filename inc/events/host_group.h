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

#ifndef EVENTS_HOST_GROUP_H_
# define EVENTS_HOST_GROUP_H_

# include <string>
# include "events/event.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    /**
     *  \class HostGroup host_group.h "events/host_group.h"
     *  \brief Represents a group of host inside Nagios.
     *
     *  XXX : need fix
     */
    class                HostGroup : public Event
    {
     private:
      enum               String
      {
	ACTION_URL = 0,
	ALIAS,
	HOST_GROUP_NAME,
	NOTES,
	NOTES_URL,
	STRING_NB
      };
      std::string        strings_[STRING_NB];
      /**
       *  \brief Copy all internal data of the given object to the current
       *         instance.
       */
      void               InternalCopy(const HostGroup& host_group);

     public:
      /**
       *  \brief HostGroup default constructor.
       */
                         HostGroup();
      /**
       *  \brief HostGroup copy constructor.
       */
                         HostGroup(const HostGroup& host_group);
      /**
       *  \brief HostGroup destructor.
       */
                         ~HostGroup();
      /**
       *  \brief Overload of the = operator.
       */
      HostGroup&         operator=(const HostGroup& host_group);
      /**
       *  \brief XXX : need fix
       */
      const std::string& GetActionUrl() const throw ();
      /**
       *  \brief Get the alias of the host group.
       */
      const std::string& GetAlias() const throw ();
      /**
       *  \brief Get the real name of the host group.
       */
      const std::string& GetHostGroupName() const throw ();
      /**
       *  \brief XXX : need fix
       */
      const std::string& GetNotes() const throw ();
      /**
       *  \brief XXX : need fix
       */
      const std::string& GetNotesUrl() const throw ();
      /**
       *  \brief Returns the type of this event (Event::HOSTGROUP).
       */
      int                GetType() const throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetActionUrl(const std::string& au);
      /**
       *  \brief Set the alias of the host group.
       */
      void               SetAlias(const std::string& a);
      /**
       *  \brief Set the real name of the host group.
       */
      void               SetHostGroupName(const std::string& hgn);
      /**
       *  \brief XXX : need fix
       */
      void               SetNotes(const std::string& n);
      /**
       *  \brief XXX : need fix
       */
      void               SetNotesUrl(const std::string& nu);
    };
  }
}

#endif /* !EVENTS_HOST_GROUP_H_ */
