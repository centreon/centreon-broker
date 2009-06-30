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

#ifndef EVENTS_ACKNOWLEDGEMENT_H_
# define EVENTS_ACKNOWLEDGEMENT_H_

# include <ctime> // for time_t
# include <string>
# include "events/event.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    /**
     *  \class Acknowledgement acknowledgement.h "events/acknowledgement.h"
     *  \brief Represents an acknowledgement inside Nagios.
     *
     *  When some service or host is critical, Nagios will emit notifications
     *  according to its configuration. To stop the notification process, a
     *  user can acknowledge the problem.
     */
    class                Acknowledgement : public Event
    {
     private:
      enum               Short
      {
	ACKNOWLEDGEMENT_TYPE = 0,
	IS_STICKY,
	NOTIFY_CONTACTS,
	PERSISTENT_COMMENT,
	STATE,
	SHORT_NB
      };
      enum               String
      {
	AUTHOR_NAME = 0,
	COMMENT,
	HOST,
	SERVICE,
	STRING_NB
      };
      enum               TimeT
      {
	ENTRY_TIME = 0,
	TIMET_NB
      };
      short              shorts_[SHORT_NB];
      std::string        strings_[STRING_NB];
      time_t             timets_[TIMET_NB];
      /**
       *  \brief Copy all internal data of the given object to the current
       *         instance.
       */
      void               InternalCopy(const Acknowledgement& ack);

     public:
      /**
       *  \brief Acknowledgement default constructor.
       */
                         Acknowledgement();
      /**
       *  \brief Acknowledgement copy constructor.
       */
                         Acknowledgement(const Acknowledgement& ack);
      /**
       *  \brief Acknowledgement destructor.
       */
                         ~Acknowledgement();
      /**
       *  \brief Overload of the = operator.
       */
      Acknowledgement&   operator=(const Acknowledgement& ack);
      /**
       *  \brief Get the type of the acknowledgement.
       */
      short              GetAcknowledgementType() const throw ();
      /**
       *  \brief Get the name of the acknowledgement author.
       */
      const std::string& GetAuthorName() const throw ();
      /**
       *  \brief Get the comment associated with the acknowledgement.
       */
      const std::string& GetComment() const throw ();
      /**
       *  \brief XXX : need fix
       */
      time_t             GetEntryTime() const throw ();
      /**
       *  \brief Get the name of the host associated with the acknowledgement.
       */
      const std::string& GetHost() const throw ();
      /**
       *  \brief Determines whether or not the acknowledgement is sticky.
       */
      short              GetIsSticky() const throw ();
      /**
       *  \brief Determines whether or not contacts should stil be notified.
       */
      short              GetNotifyContacts() const throw ();
      /**
       *  \brief XXX : need fix
       */
      short              GetPersistentComment() const throw ();
      /**
       *  \brief Get the name of the service associated with the
       *         acknowledgement.
       */
      const std::string& GetService() const throw ();
      /**
       *  \brief XXX : need fix
       */
      short              GetState() const throw ();
      /**
       *  \brief Returns the type of this event (Event::ACKNOWLEDGEMENT).
       */
      int                GetType() const throw ();
      /**
       *  \brief Set the type of the acknowledgement.
       */
      void               SetAcknowledgementType(short at) throw ();
      /**
       *  \brief Set the name of the acknowledgement author.
       */
      void               SetAuthorName(const std::string& an);
      /**
       *  \brief Set the comment associated with the acknowledgement.
       */
      void               SetComment(const std::string& c);
      /**
       *  \brief XXX : need fix
       */
      void               SetEntryTime(time_t et) throw ();
      /**
       *  \brief Set the name of the host associated with the acknowledgement.
       */
      void               SetHost(const std::string& h);
      /**
       *  \brief Set whether or not the acknowledgement is sticky.
       */
      void               SetIsSticky(short is) throw ();
      /**
       *  \brief Set whether or not contacts should still be notified.
       */
      void               SetNotifyContacts(short nc) throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetPersistentComment(short pc) throw ();
      /**
       *  \brief Set the name of the service associated with the
       *         acknowledgement.
       */
      void               SetService(const std::string& s);
      /**
       *  \brief XXX : need fix
       */
      void               SetState(short s) throw ();
    };
  }
}

#endif /* !EVENTS_ACKNOWLEDGEMENT_H_ */
