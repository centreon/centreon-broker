/*
** comment.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/16/09 Matthieu Kermagoret
** Last update 06/16/09 Matthieu Kermagoret
*/

#ifndef COMMENT_H_
# define COMMENT_H_

# include <string>
# include "event.h"

namespace       CentreonBroker
{
  class                Comment : public Event
  {
   private:
    enum               Short
    {
      COMMENT_SOURCE = 0,
      COMMENT_TYPE,
      ENTRY_TYPE,
      EXPIRES,
      IS_PERSISTENT,
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
    void               InternalCopy(const Comment& comment);

   public:
                       Comment();
                       Comment(const Comment& comment);
                       ~Comment();
    Comment&           operator=(const Comment& comment);
    const std::string& GetAuthorName() const throw ();
    const std::string& GetCommentData() const throw ();
    short              GetCommentSource() const throw ();
    time_t             GetCommentTime() const throw ();
    short              GetCommentType() const throw ();
    time_t             GetDeletionTime() const throw ();
    time_t             GetEntryTime() const throw ();
    short              GetEntryType() const throw ();
    time_t             GetExpirationTime() const throw ();
    short              GetExpires() const throw ();
    const std::string& GetHost() const throw ();
    short              GetIsPersistent() const throw ();
    const std::string& GetService() const throw ();
    int                GetType() const throw ();
    void               SetAuthorName(const std::string& an);
    void               SetCommentData(const std::string& cd);
    void               SetCommentSource(short cs) throw ();
    void               SetCommentTime(time_t ct) throw ();
    void               SetCommentType(short ct) throw ();
    void               SetDeletionTime(time_t dt) throw ();
    void               SetEntryTime(time_t et) throw ();
    void               SetEntryType(short et) throw ();
    void               SetExpirationTime(time_t et) throw ();
    void               SetExpires(short e) throw ();
    void               SetHost(const std::string& h);
    void               SetIsPersistent(short ip) throw ();
    void               SetService(const std::string& s);
  };
}

#endif /* !COMMENT_H_ */
