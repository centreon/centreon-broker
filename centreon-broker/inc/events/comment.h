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

#ifndef EVENTS_COMMENT_H_
# define EVENTS_COMMENT_H_

# include <ctime> // for time_t
# include <string>
# include "events/event.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    /**
     *  \class Comment comment.h "events/comment.h"
     *  \brief Represents a comment inside Nagios.
     *
     *  XXX : need fix
     */
    class                Comment : public Event
    {
     private:
      enum               Short
      {
	COMMENT_TYPE = 0,
	ENTRY_TYPE,
	EXPIRES,
	PERSISTENT,
	SOURCE,
	SHORT_NB
      };
      enum               String
      {
	AUTHOR_NAME = 0,
	COMMENT_DATA,
	HOST,
	SERVICE,
	STRING_NB
      };
      enum               TimeT
      {
	COMMENT_TIME = 0,
	DELETION_TIME,
	ENTRY_TIME,
	EXPIRATION_TIME,
	TIMET_NB
      };
      short              shorts_[SHORT_NB];
      std::string        strings_[STRING_NB];
      time_t             timets_[TIMET_NB];
      /**
       *  \brief Copy all internal data of the given object to the current
       *         instance.
       */
      void               InternalCopy(const Comment& comment);

     public:
      /**
       *  \brief Comment default constructor.
       */
                         Comment();
      /**
       *  \brief Comment copy constructor.
       */
                         Comment(const Comment& comment);
      /**
       *  \brief Comment destructor.
       */
                         ~Comment();
      /**
       *  \brief Overload of the = operator.
       */
      Comment&           operator=(const Comment& comment);
      /**
       *  \brief Get the name of the comment author.
       */
      const std::string& GetAuthorName() const throw ();
      /**
       *  \brief Get the comment content.
       */
      const std::string& GetCommentData() const throw ();
      /**
       *  \brief Get the date on which the comment was made.
       */
      time_t             GetCommentTime() const throw ();
      /**
       *  \brief Get the type of the comment.
       */
      short              GetCommentType() const throw ();
      /**
       *  \brief Get the time on which the comment was deleted.
       */
      time_t             GetDeletionTime() const throw ();
      /**
       *  \brief XXX : need fix
       */
      time_t             GetEntryTime() const throw ();
      /**
       *  \brief XXX : need fix
       */
      short              GetEntryType() const throw ();
      /**
       *  \brief Get the date on which the comment expires.
       */
      time_t             GetExpirationTime() const throw ();
      /**
       *  \brief Determines whether or not the comment expires.
       */
      short              GetExpires() const throw ();
      /**
       *  \brief Get the name of the host associated with the comment.
       */
      const std::string& GetHost() const throw ();
      /**
       *  \brief Determines whether or not the comment is persistent.
       */
      short              GetPersistent() const throw ();
      /**
       *  \brief Get the name of the service associated with the comment.
       */
      const std::string& GetService() const throw ();
      /**
       *  \brief Get the comment source.
       */
      short              GetSource() const throw ();
      /**
       *  \brief Returns the type of this event (Event::COMMENT).
       */
      int                GetType() const throw ();
      /**
       *  \brief Set the name of the comment author.
       */
      void               SetAuthorName(const std::string& an);
      /**
       *  \brief Set the comment content.
       */
      void               SetCommentData(const std::string& cd);
      /**
       *  \brief Set the date on which the comment was made.
       */
      void               SetCommentTime(time_t ct) throw ();
      /**
       *  \brief Set the type of the comment.
       */
      void               SetCommentType(short ct) throw ();
      /**
       *  \brief Set the date on which the comment was deleted.
       */
      void               SetDeletionTime(time_t dt) throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetEntryTime(time_t et) throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetEntryType(short et) throw ();
      /**
       *  \brief Set the date on which the comment expires.
       */
      void               SetExpirationTime(time_t et) throw ();
      /**
       *  \brief Set whether or not the comment expires.
       */
      void               SetExpires(short e) throw ();
      /**
       *  \brief Set the name of the host associated with the comment.
       */
      void               SetHost(const std::string& h);
      /**
       *  \brief Set whether or not the comment is persistent.
       */
      void               SetPersistent(short ip) throw ();
      /**
       *  \brief Set the name of the service associated with the comment.
       */
      void               SetService(const std::string& s);
      /**
       *  \brief Set the comment source.
       */
      void               SetSource(short cs) throw ();
    };
  }
}

#endif /* !COMMENT_H_ */
